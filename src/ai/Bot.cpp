#include "../../include/ai/Bot.hpp"

void Bot::beginDecision(const GameView& view){
    strat.resolvePhase(view, jitter);
    trace = DecisionTrace{};
    trace.phase = strat.getCurrentPhase().getName();
}

std::vector<Rule> Bot::collect(const GameView& view, bool buy) const {
    std::vector<Rule> res;
    std::set<std::string> seen;

    auto append = [&](const Phase& p) {
        const std::vector<Rule> rules = buy ? p.getBuyRules() : p.getPlayRules();
        for(const Rule& r : rules){
            const Card* card = r.getCard();

            // Si on a déjà check la carte où qu'elle s'applique pas on skip
            if(seen.count(card->getName()) || !r.applies(view, jitter)){
                continue;
            }

            // Si on peut pas l'acheter ou la jouer on skip
            if(!(buy ? isPurchaseValid(view, card) : isPlayValid(view, card))){
                continue;
            }

            seen.insert(card->getName());
            res.push_back(r);
        }
    };

    append(strat.getUrgency());                     // D'abord les trucs urgents
    append(strat.getCurrentPhase());                // Ensuite la phase actuelle
    if(strat.getFallback()){                        // Enfin le fallback si besoin
        Strategy& fallback = *strat.getFallback();
        fallback.resolvePhase(view, jitter, 0);
        append(fallback.getCurrentPhase());
    }

    return res;
}

bool Bot::isPurchaseValid(const GameView& view, const Card* card){
    return view.getPurchases() > 0 && view.getSupplyCount(card) > 0 && card->getCost() <= view.getCoins();
}

bool Bot::isPlayValid(const GameView& view, const Card* card){
    return view.getActions() > 0 && view.inHand(card) > 0;
}

bool Bot::chance(double p) const {
    if(p <= 0.0){
        return false;
    }
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng) < p;
}

size_t Bot::randomIndex(size_t n) const {
    return std::uniform_int_distribution<std::size_t>(0, n - 1)(rng);
}

const Card* Bot::finish(const Card* c, const std::string& why) {
    trace.cardsChosen = {c};
    trace.rule = why;
    return c;
}

std::vector<const Card*> Bot::finishList(std::vector<const Card*> chosen, const std::string& why) {
    trace.cardsChosen = chosen;
    trace.rule = why;
    return chosen;
}

const Card* Bot::chooseBuy(const GameView& v){
    beginDecision(v);
    std::vector<Rule> rules = collect(v, true);
    if(rules.empty()){
        return finish(nullptr, "none");
    }

    if(chance(profile.skipBuyRate)) {
        trace.noiseApplied = true;
        return finish(nullptr, "skip(noise)");
    }

    size_t i = 0;
    if(rules.size() > 1 && chance(profile.buyErrorRate)) {
        size_t maxI = std::min<size_t>(2, rules.size()-1);
        i = 1 + randomIndex(maxI); // Mauvaise option genre 2e ou 3e
        trace.noiseApplied = true;
    }
    return finish(rules[i].getCard(), rules[i].getLabel());
}

const Card* Bot::chooseAction(const GameView& v){
    beginDecision(v);
    std::vector<Rule> rules = collect(v, false);
    if(rules.empty()){
        return finish(nullptr, "none");
    }

    size_t i = 0;
    if(chance(profile.playErrorRate)){
        trace.noiseApplied = true;
        if(rules.size() == 1){
            // Oubli de jouer une carte
            return finish(nullptr, "forgot(noise)");
        }
        i = randomIndex(rules.size()); // Mauvais ordre
    }
    return finish(rules[i].getCard(), rules[i].getLabel());
}

std::vector<const Card*> Bot::chooseFromList(const GameView& v, DecisionType type, SelectionParameters params) {
    beginDecision(v);
    std::vector<const Card*>& pool = params.selection;
    if(pool.empty()){
        return finishList({}, "no-candidate");
    }

    const size_t n = std::min(params.nbToSelect, pool.size());
    size_t lo = 0, hi = n;
    switch(params.mode){
        case SelectionMode::EXACTLY:
            lo = n;
            hi = n;
            break;
        
        case SelectionMode::AT_MOST:
            lo = 0;
            hi = n;
            break;

        case SelectionMode::AT_LEAST:
            lo = n;
            hi = n;
            break;
    }

    // Noise : on sélectionne aléatoirement une carte valide
    if(chance(profile.minorDecisionErrorRate)) {
        trace.noiseApplied = true;
        const size_t count = lo + randomIndex(hi - lo + 1);
        std::vector<size_t> idx(pool.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), rng);

        std::vector<const Card*> res;
        for(size_t i = 0; i < count ; ++i){
            res.push_back(pool[idx[i]]);
        }
        return finishList(std::move(res), "random(noise)");
    }

    // Classement : urgence -> phase active -> fallback
    std::vector<const Card*> ranking;
    std::string label;
    auto gather = [&](const Phase& p){
        for(const DecisionRule& dr : p.getDecisionRules()){
            if(dr.getType() != type || !dr.applies(v, jitter)){
                continue;
            }
            if(label.empty()){
                label = dr.getLabel();
            }
            ranking.insert(ranking.end(), dr.getCards().begin(), dr.getCards().end());
        }
    };
    gather(strat.getUrgency());
    gather(strat.getCurrentPhase());
    if(strat.getFallback()){
        Strategy& fallback = *strat.getFallback();
        fallback.resolvePhase(v, jitter);
        gather(fallback.getCurrentPhase());
    }

    // Sélection des cartes du classement
    std::vector<bool> taken(pool.size(), false);
    std::vector<const Card*> res;
    for(const Card* card : ranking){
        for(size_t i = 0; i < pool.size() && res.size() < hi; ++i){
            if(!taken[i] && pool[i] == card) {
                taken[i] = true;
                res.push_back(pool[i]);
            }
        }
        if(res.size() >= hi){
            break;
        }
    }

    // Sélection du surplus si on en a pas assez
    for(size_t i = 0; i < pool.size() && res.size() < lo; ++i){
        if(!taken[i]){
            taken[i] = true;
            res.push_back(pool[i]);
        }
    }

    if(label.empty()){
        label = params.label.empty() ? "default-first" : params.label;
    }
    return finishList(std::move(res), label);
}

void Bot::playTurn(GameView& view) {
    
}