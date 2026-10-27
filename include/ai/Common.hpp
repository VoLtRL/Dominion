#pragma once
#include <algorithm>
#include <initializer_list>
#include <memory>
#include <vector>
#include "Condition.hpp"
#include "Rule.hpp"
#include "Strategy.hpp"

// Briques partagées par les stratégies.
namespace strat {
using Kingdom = std::map<const Card*, int>;
inline bool contains(const Kingdom& k, const Card* c) {
    return std::find(k.begin(), k.end(), c) != k.end();
}

inline int countIn(const Kingdom& k, std::initializer_list<const Card*> cards) {
    int n = 0;
    for (const Card* c : cards) n += contains(k, c) ? 1 : 0;
    return n;
}

// --- Conditions sans jitter (sécurité : ne doivent pas être faussées par la difficulté)
inline Condition provincesAtMostExact(int n) {
    return Condition([n](const GameView& v, int) { return v.getSupplyCount(Factory::get("Province")) <= n; },
                     "provincesExact<=" + std::to_string(n));
}
inline Condition lastProvince() { return provincesAtMostExact(1); }

inline Condition cursesLeft() {
    return Condition([](const GameView& v, int) { return v.getSupplyCount(Factory::get("Curse")) > 0; }, "cursesLeft");
}

// Ne pas acheter la dernière Province si cela fait perdre la partie
// (acheter = +6 points ; perdu si on est déjà à -6 ou pire).
inline Condition safeProvince() { return !(lastProvince() && cond::behindBy(6)); }

// --- Règles d'urgence communes (évaluées avant la phase) ------------------
inline void addEndgameUrgency(Strategy& s) {
    s.getUrgency().setName("Urgency");
    s.getUrgency().setBuyRules({
        // Dernière Province : on la prend si elle nous fait gagner.
        Rule(Factory::get("Province"), 200, Condition(lastProvince() && !cond::behindBy(6)), -1, "finish-win"),
        // Fin proche et pas assez pour une Province : Duché de panique.
        makeRule(Factory::get("Province"), 150, provincesAtMostExact(2) && !cond::coinsAtLeast(8), -1, "panic-duchy"),
    });
}

// --- Décisions communes (défausse, écart, gain) ---------------------------
// `green` = true en phase de verdissement (le gain vise les points).
inline std::vector<DecisionRule> commonDecisions(bool green) {
    using namespace cond;
    std::vector<DecisionRule> d;
    d.push_back(makeDecision(DecisionType::Discard,
        {Cards::Curse, Cards::Estate, Cards::Duchy, Cards::Province, Cards::Copper, Cards::Silver},
        always(), "discard-junk-first"));
    d.push_back(makeDecision(DecisionType::Trash,
        {Cards::Curse, Cards::Estate}, always(), "trash-junk"));
    if (green)
        d.push_back(makeDecision(DecisionType::Gain,
            {Cards::Province, Cards::Duchy, Cards::Gold, Cards::Estate, Cards::Silver},
            always(), "gain-points"));
    else
        d.push_back(makeDecision(DecisionType::Gain,
            {Cards::Gold, Cards::Silver, Cards::Copper}, always(), "gain-money"));
    return d;
}

// Attache Big Money comme repli (finalisé, car le registre ne le voit pas).
inline void withBigMoneyFallback(Strategy& s) {
    auto bm = makeBigMoney();
    bm->finalize();
    s.fallback = bm;
}

} // namespace strat