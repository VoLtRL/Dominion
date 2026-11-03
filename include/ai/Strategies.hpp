#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Strategy.hpp"

// Biblio pour les strats

namespace Strategies
{
    using Distrib = std::map<const Card *, int>;

    // Fonctions utiles pour les strats
    namespace detail
    {
        inline const Card *C(const char *name) { return Factory::get(name); }

        inline bool inSupply(const Distrib &d, const Card *c)
        {
            auto it = d.find(c);
            return it != d.end() && it->second > 0;
        }

        inline int supplyOf(const Distrib &d, const Card *c)
        {
            auto it = d.find(c);
            return it == d.end() ? 0 : it->second;
        }

        inline Rule playRule(const Card *c, int prio = 1)
        {
            return Rule(cond::always(), c, prio, "play(" + c->getName() + ")");
        }

        // Ordre de défausse : on garde le plus utile.
        inline std::vector<DecisionRule> decisions(bool late)
        {
            std::vector<const Card *> discard = late
                ? std::vector<const Card *>{C("Curse"), C("Copper"), C("Silver"), C("Estate")}
                : std::vector<const Card *>{C("Curse"), C("Estate"), C("Province"), C("Duchy"), C("Gardens"), C("Copper"), C("Silver")};
            std::vector<const Card *> gain = late
                ? std::vector<const Card *>{C("Province"), C("Duchy"), C("Estate"), C("Gold"), C("Silver"), C("Copper")}
                : std::vector<const Card *>{C("Gold"), C("Silver"), C("Copper")};
            return {DecisionRule(DecisionType::DISCARD, cond::always(), discard, "discard"),
                    DecisionRule(DecisionType::GAIN, cond::always(), gain, "gain")};
        }

        inline Phase makeUrgency(const Distrib &d)
        {
            return Phase("urgency", cond::always(), Rule::basicUrgencyBuyRules(d), {}, DecisionRule::basicUrgencyDecisionRules());
        }
    }

    // BigMoneyTerminal : BigMoney + une carte (terminale ou non)
    struct TerminalOptions
    {
        int copies = 2;               // Nb d'exemplaires visé
        int secondCopyDeckSize = 16;  // Taille minimale pour commencer à acheter +
        bool overGold = false;        // true : carte avant gold
        int buildTurn = 3;            // Nb tour avant de rentrer dans la 2e phase
        double greenDensity = 1.2;    // Densité coins avant de rentrer dans la 3e phase
        int greenTurnSafety = 15;     // Au cas où le tour où on force la 3e phase
        int tier = 2;
        bool attack = false;          // Moat dans royaume = pénalisé
        bool needsCurse = false;      // Pas de curse dans royaume = pénalisé
    };

    inline std::function<double(Distrib)> terminalViability(const Card *card, TerminalOptions o)
    {
        return [card, o](Distrib d) -> double
        {
            if (!detail::inSupply(d, card))
                return 0.0;
            double v = 1.0;
            if (o.needsCurse && !detail::inSupply(d, detail::C("Curse")))
                v -= 0.5;
            if (o.attack && detail::inSupply(d, detail::C("Moat")))
                v -= 0.1;
            // Possible mais compliqué car pas beaucoup d'exemplaires possibles
            if (detail::supplyOf(d, card) < o.copies)
                v -= 0.2;
            return v;
        };
    }

    inline Strategy bigMoneyTerminal(const Card *card, const Distrib &dist, TerminalOptions o = {})
    {
        using detail::C;
        const std::string n = card->getName();
        const std::vector<Rule> play = {detail::playRule(card)};
        const auto early = detail::decisions(false);        // Règles de base tôt dans la game
        const auto late = detail::decisions(true);          // --           --tard dans la game

        // start : 2 premiers tours on prends de l'argent
        Phase start("start", cond::always(),
                    {Rule(cond::canBuyCard(C("Silver")), C("Silver"), 1, "buy(Silver)"),
                     Rule(cond::canBuyCard(C("Copper")), C("Copper"), 0, "buy(Copper)")},
                    play, early);

        // build : on commence à prendre la carte
        Condition wantCard = cond::canBuyCard(card) &&
                             (cond::ownedLessThan(card, 1) ||
                              (cond::ownedLessThan(card, o.copies) && cond::deckSizeAtLeast(o.secondCopyDeckSize)));
        Phase build("build", cond::turnAtLeast(o.buildTurn),
                    {Rule(wantCard, card, o.overGold ? 4 : 2, "buy(" + n + ")"),
                     Rule(cond::canBuyCard(C("Gold")), C("Gold"), 3, "buy(Gold)"),
                     Rule(cond::canBuyCard(C("Silver")), C("Silver"), 1, "buy(Silver)"),
                     Rule(cond::always(), C("Copper"), 0, "buy(Copper)")},
                    play, early);

        // green : plus d'achats, on farm victory et coinns
        Condition enoughCopies = cond::ownedAtLeast(card, o.copies) || cond::leftAtMost(card, 0) || cond::turnAtLeast(o.greenTurnSafety);
        Condition greenGate = (cond::moneyDensityAtLeast(o.greenDensity) && enoughCopies) || cond::provincesLeftAtMost(3);
        Phase green("green", greenGate,
                    {Rule(cond::canBuyCard(C("Province")), C("Province"), 4, "buy(Province)"),
                     Rule(cond::canBuyCard(C("Duchy")) && cond::provincesLeftAtMost(5), C("Duchy"), 3, "buy(Duchy)"),
                     Rule(cond::canBuyCard(C("Gold")), C("Gold"), 2, "buy(Gold)"),
                     Rule(cond::canBuyCard(C("Silver")), C("Silver"), 1, "buy(Silver)"),
                     Rule(cond::always(), C("Copper"), 0, "buy(Copper)")},
                    play, early);

                
        // endgame : rush pour les victory points
        Phase endgame("endgame", cond::provincesLeftAtMost(3),
                      {Rule(cond::canBuyCard(C("Province")), C("Province"), 4, "buy(Province)"),
                       Rule(cond::canBuyCard(C("Duchy")), C("Duchy"), 3, "buy(Duchy)"),
                       Rule(cond::canBuyCard(C("Estate")), C("Estate"), 2, "buy(Estate)"),
                       Rule(cond::canBuyCard(C("Gold")), C("Gold"), 1, "buy(Gold)"),
                       Rule(cond::always(), C("Copper"), 0, "buy(Copper)")},
                      play, late);

        Strategy s("bigMoneyTerminal(" + n + ")", o.tier, {start, build, green, endgame},
                   detail::makeUrgency(dist), nullptr, terminalViability(card, o),
                   {{"copies", o.copies}, {"secondCopyDeckSize", o.secondCopyDeckSize}, {"buildTurn", o.buildTurn}});
        s.finalize();
        return s;
    }

    // Pour récupérer les presets facilement
    inline bool terminalPreset(const std::string &name, TerminalOptions &o)
    {
        o = TerminalOptions{};
        if (name == "Smithy")           { o.copies = 1; o.secondCopyDeckSize = 0; }
        else if (name == "Militia")     { o.copies = 1; o.secondCopyDeckSize = 0; o.attack = true; }
        else if (name == "Witch")       { o.copies = 2; o.secondCopyDeckSize = 0; o.overGold = true; o.attack = true; o.needsCurse = true; }
        else if (name == "Library")     { o.copies = 1; o.secondCopyDeckSize = 0; }
        else if (name == "CouncilRoom") { o.copies = 1; o.secondCopyDeckSize = 0; }
        else if (name == "Moneylender") { o.copies = 1; o.secondCopyDeckSize = 0; }
        // Non terminales : mesurées ~ au niveau de BigMoney (un Silver à 5 pièces), donc jamais choisies
        else if (name == "Laboratory")  { o.copies = 1; o.secondCopyDeckSize = 0; }
        else if (name == "Market")      { o.copies = 1; o.secondCopyDeckSize = 0; }
        else return false;
        return true;
    }

    inline Strategy bigMoneyPreset(const std::string &cardName, const Distrib &dist)
    {
        TerminalOptions o;
        terminalPreset(cardName, o);
        return bigMoneyTerminal(detail::C(cardName.c_str()), dist, o);
    }

    // Gardens rush : Atelier gagne des Jardins, on vide 3 piles (atelier, gardens et estate la plupart du temps)
    inline Strategy gardensRush(const Distrib &dist, int maxWorkshops = 6)
    {
        using detail::C;
        const Card *workshop = C("Workshop"), *gardens = C("Gardens"), *estate = C("Estate"), *copper = C("Copper");
        const std::vector<Rule> play = {detail::playRule(workshop)};
        auto gain = [&](std::vector<const Card *> r)
        { return DecisionRule(DecisionType::GAIN, cond::always(), std::move(r), "gain"); };
        auto discard = DecisionRule(DecisionType::DISCARD, cond::always(),
                                    {C("Curse"), C("Estate"), C("Province"), C("Duchy"), C("Gardens"), C("Copper")}, "discard");

        Phase start("start", cond::always(),
                    {Rule(cond::canBuyCard(workshop) && cond::ownedLessThan(workshop, maxWorkshops), workshop, 3, "buy(Workshop)"),
                     Rule(cond::always(), copper, 0, "buy(Copper)")},
                    play, {gain({workshop, gardens, estate, copper}), discard});

        Phase rush("rush", cond::ownedAtLeast(workshop, 4) || cond::turnAtLeast(6),
                   {Rule(cond::canBuyCard(C("Province")), C("Province"), 5, "buy(Province)"),
                    Rule(cond::canBuyCard(gardens), gardens, 4, "buy(Gardens)"),
                    Rule(cond::canBuyCard(workshop) && cond::ownedLessThan(workshop, maxWorkshops), workshop, 3, "buy(Workshop)"),
                    Rule(cond::canBuyCard(estate), estate, 2, "buy(Estate)"),
                    Rule(cond::always(), copper, 0, "buy(Copper)")},
                   play, {gain({gardens, estate, copper}), discard});

        Phase finish("finish", cond::leftAtMost(gardens, 3) || cond::emptyPilesAtLeast(1),
                     {Rule(cond::canBuyCard(C("Province")), C("Province"), 5, "buy(Province)"),
                      Rule(cond::canBuyCard(gardens), gardens, 4, "buy(Gardens)"),
                      Rule(cond::canBuyCard(C("Duchy")) && cond::leftAtMost(gardens, 0), C("Duchy"), 3, "buy(Duchy)"),
                      Rule(cond::canBuyCard(estate), estate, 2, "buy(Estate)"),
                      Rule(cond::always(), copper, 0, "buy(Copper)")},
                     play, {gain({gardens, estate, copper}), discard});

        auto viability = [](Distrib d) -> double
        {
            if (!detail::inSupply(d, detail::C("Workshop")) || !detail::inSupply(d, detail::C("Gardens")))
                return 0.0;
            double v = 0.8;
            if (detail::supplyOf(d, detail::C("Gardens")) < 8) v -= 0.2;
            return v;
        };

        Strategy s("gardensRush", 3, {start, rush, finish}, detail::makeUrgency(dist), nullptr, viability,
                   {{"maxWorkshops", maxWorkshops}});
        s.finalize();
        return s;
    }

    // Sélection : meilleure strat pour une distrib et un tier
    inline std::vector<Strategy> candidates(const Distrib &dist)
    {
        std::vector<Strategy> res;
        Strategy bm = Strategy::bigMoney(dist);
        bm.finalize();
        res.push_back(bm);
        res.push_back(gardensRush(dist));
        for (const char *name : {"Witch", "Smithy", "Militia", "Library", "Moneylender", "CouncilRoom"})
            res.push_back(bigMoneyPreset(name, dist));
        return res;
    }

    inline Strategy select(const Distrib &dist, int maxTier = 4)
    {
        std::vector<Strategy> all = candidates(dist);
        size_t best = 0; // BigMoney
        double bestV = all[0].getViability(dist);
        for (size_t i = 1; i < all.size(); ++i)
        {
            if (all[i].getTier() > maxTier)
                continue;
            double v = all[i].getViability(dist);
            if (v > bestV || (best == 0 && v >= bestV))
            {
                bestV = v;
                best = i;
            }
        }
        return all[best];
    }
}