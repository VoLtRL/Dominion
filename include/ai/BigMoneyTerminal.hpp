#pragma once
#include <memory>
#include <string>
#include "../abstract/Card.hpp"
#include "Strategy.hpp"

// Big Money + 1 carte terminale (Smithy, Militia, Witch...).
struct TerminalBMConfig {
    std::string name;
    const Card* terminal = nullptr;
    int tier = 2;
    int firstPriority = 55;     // > 60 = acheté avant l'Or
    int maxCopies = 2;          // plafond sur le deck total
    int secondAtDeckSize = 16;  // taille du deck à partir de laquelle on prend la 2e
    int duchyAt = 4;            // Provinces restantes -> phase de verdissement
    int estateAt = 2;           // Provinces restantes -> Domaines + fin de partie
    bool needsCurses = false;   // Witch : inutile une fois les Malédictions épuisées
};

inline std::shared_ptr<Strategy> makeBigMoneyTerminal(const TerminalBMConfig& c) {
    using namespace cond;
    auto s = std::make_shared<Strategy>();
    s->setName(c.name);
    s->setTier(c.tier);
    const Card* t = c.terminal;
    s->setViability([t](std::map<const Card*, int> v) { return v.at(t) ? 1.0 : 0.0; });
    s->setParams({{"firstPriority", c.firstPriority}, {"maxCopies", c.maxCopies},
                 {"secondAtDeckSize", c.secondAtDeckSize}, {"duchyAt", c.duchyAt},
                 {"estateAt", c.estateAt}});

    const Condition usable = c.needsCurses ? strat::cursesLeft() : always();

    // Phase 1 : construction
    Phase build;
    build.name = "Construction";
    build.entry = always();
    build.buyRules = {
        makeRule(Cards::Province, 100, ownedAtLeast(Cards::Gold, 1) && strat::safeProvince(), -1, "Province"),
        makeRule(Cards::Gold,      60, always(), -1, "Gold"),
        makeRule(t, c.firstPriority, usable, 1, "terminal#1"),
    };
    if (c.maxCopies > 1)
        build.buyRules.push_back(makeRule(t, c.firstPriority - 1,
            usable && deckSizeAtLeast(c.secondAtDeckSize), c.maxCopies, "terminal#2"));
    build.buyRules.push_back(makeRule(Cards::Silver, 10, always(), -1, "Silver"));
    build.playRules = { makeRule(t, 50, always(), -1, "play-terminal") };
    build.decisionRules = strat::commonDecisions(false);

    // Phase 2 : verdissement
    Phase green;
    green.name = "Greening";
    green.entry = provincesLeftAtMost(c.duchyAt);
    green.buyRules = {
        makeRule(Cards::Province, 100, strat::safeProvince(), -1, "Province"),
        makeRule(Cards::Duchy,     70, always(), -1, "Duchy"),
        makeRule(Cards::Estate,    40, provincesLeftAtMost(c.estateAt), -1, "Estate"),
        makeRule(Cards::Gold,      30, always(), -1, "Gold"),
        makeRule(Cards::Silver,    10, always(), -1, "Silver"),
    };
    green.playRules = build.playRules;
    green.decisionRules = strat::commonDecisions(true);

    // Phase 3 : fin
    Phase end;
    end.name = "Fin";
    end.entry = provincesLeftAtMost(c.estateAt);
    end.buyRules = {
        makeRule(Cards::Province, 100, strat::safeProvince(), -1, "Province"),
        makeRule(Cards::Duchy,     60, always(), -1, "Duchy"),
        makeRule(Cards::Estate,    40, always(), -1, "Estate"),
    };
    end.playRules = build.playRules;
    end.decisionRules = strat::commonDecisions(true);

    s->phases = {build, green, end};
    strat::addEndgameUrgency(*s);
    strat::withBigMoneyFallback(*s);
    return s;
}

// --- Les trois stratégies -------------------------------------------------

// ★★ : 1 Smithy, une 2e quand le deck dépasse ~16 cartes.
inline std::shared_ptr<Strategy> makeBigMoneySmithy() {
    TerminalBMConfig c;
    c.name = "BigMoneySmithy";
    c.terminal = Cards::Smithy;
    c.firstPriority = 55;
    c.secondAtDeckSize = 16;
    return makeBigMoneyTerminal(c);
}

// ★★ : l'attaque ralentit l'adversaire ; même rythme que Smithy.
inline std::shared_ptr<Strategy> makeBigMoneyMilitia() {
    TerminalBMConfig c;
    c.name = "BigMoneyMilitia";
    c.terminal = Cards::Militia;
    c.firstPriority = 55;
    c.secondAtDeckSize = 18;
    return makeBigMoneyTerminal(c);
}

// ★★ : 2 Witch le plus vite possible (avant l'Or), verdissement plus tôt.
inline std::shared_ptr<Strategy> makeBigMoneyWitch() {
    TerminalBMConfig c;
    c.name = "BigMoneyWitch";
    c.terminal = Cards::Witch;
    c.firstPriority = 65;
    c.secondAtDeckSize = 0;
    c.duchyAt = 5;
    c.needsCurses = true;
    return makeBigMoneyTerminal(c);
}