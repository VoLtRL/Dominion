#pragma once
#include <functional>
#include <string>

#include "GameView.hpp"
#include "../gameplay/Factory.hpp"

// Boolean class used to organize the bot's behavior
class Condition
{
    using FN = std::function<bool(const GameView &, int)>;

private:
    FN f;
    std::string name;

public:
    Condition() : f([](const GameView &, int)
                    { return true; }),
                  name("always") {}
    Condition(FN fun, std::string n) : f(std::move(fun)), name(n) {}

    bool operator()(const GameView &v, int i = 0) const { return f(v, i); }
    const std::string &name() const { return name; }

    friend Condition operator&&(Condition a, Condition b)
    {
        std::string n = "(" + a.name + " && " + b.name + ")";
        return Condition([a, b](const GameView &v, int i)
                         { return a(v, i) && b(v, i); }, n);
    }
    friend Condition operator||(Condition a, Condition b)
    {
        std::string n = "(" + a.name + " || " + b.name + ")";
        return Condition([a, b](const GameView &v, int i)
                         { return a(v, i) || b(v, i); }, n);
    }
    friend Condition operator!(Condition a)
    {
        std::string n = "!" + a.name;
        return Condition([a](const GameView &v, int i)
                         { return !a(v, i); }, n);
    }
};

// Bibliothèque de conditions réutilisables.
namespace cond
{

    inline Condition always() { return Condition(); }
    inline Condition never()
    {
        return Condition([](const GameView &, int)
                         { return false; }, "never");
    }

    inline Condition provincesLeftAtMost(int n)
    {
        return Condition([n](const GameView &v, int j)
                         { return v.getSupplyCount(Factory::get("Province")) <= n + j; },
                         "provinces<=" + std::to_string(n));
    }
    inline Condition turnAtLeast(int n)
    {
        return Condition([n](const GameView &v, int j)
                         { return v.getTurn() >= n - j; },
                         "turn>=" + std::to_string(n));
    }
    inline Condition moneyDensityAtLeast(double d)
    {
        return Condition([d](const GameView &v, int)
                         { return v.getMoneyDensity() >= d; },
                         "density>=" + std::to_string(d));
    }
    inline Condition coinsAtLeast(int n)
    {
        return Condition([n](const GameView &v, int)
                         { return v.getCoins() >= n; },
                         "coins>=" + std::to_string(n));
    }
    inline Condition emptyPilesAtLeast(int n)
    {
        return Condition([n](const GameView &v, int)
                         { return v.getEmptyPilesCount() >= n; },
                         "emptyPiles>=" + std::to_string(n));
    }
    inline Condition ownedLessThan(const Card *c, int n)
    {
        return Condition([c, n](const GameView &v, int)
                         { return v.isOwned(c) < n; },
                         "owned(" + c->getName() + ")<" + std::to_string(n));
    }
    inline Condition ownedAtLeast(const Card *c, int n)
    {
        return Condition([c, n](const GameView &v, int)
                         { return v.isOwned(c) >= n; },
                         "owned(" + c->getName() + ")>=" + std::to_string(n));
    }
    inline Condition deckSizeAtLeast(int n)
    {
        return Condition([n](const GameView &v, int)
                         { return v.getDeckSize() >= n; },
                         "deckSize>=" + std::to_string(n));
    }
    inline Condition inKingdom(const Card *c)
    {
        return Condition([c](const GameView &v, int)
                         { return v.isInKingdom(c); },
                         "inKingdom(" + c->getName() + ")");
    }
    inline Condition behindBy(int points)
    {
        return Condition([points](const GameView &v, int)
                         { return v.getScoreDiff() <= -points; },
                         "behindBy>=" + std::to_string(points));
    }
    inline Condition aheadBy(int points)
    {
        return Condition([points](const GameView &v, int)
                         { return v.getScoreDiff() >= points; },
                         "aheadBy>=" + std::to_string(points));
    }

}