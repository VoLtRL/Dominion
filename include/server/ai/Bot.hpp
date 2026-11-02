#pragma once

#include <random>

#include "../../shared/player/Player.hpp"
#include "Strategy.hpp"
#include "DifficultyProfile.hpp"
#include "shared/structs/Selection.hpp"

struct SelectionParameters{
    size_t nbToSelect;
    std::string label;
    std::string presentation;
    SelectionConstraint mode;
    std::vector<const Card*> selection;
    bool allowAll;
};


struct DecisionTrace {
    std::string phase;
    std::string rule;
    bool noiseApplied = false;
    std::vector<const Card*> cardsChosen;
};

class Bot : public Player {
    private:

        Strategy strat;                         // Stratégie que le bot va suivre
        int jitter = 0;                         // Décalage random selon la difficulté
        void beginDecision(const GameView& v);  // Sélection de la phase + préparation de la trace
        DecisionTrace trace;                    // Un peu un historique de ce qu'il s'est passé ce tour là, genre pour les logs
        DifficultyProfile profile;              // Difficulté
        mutable std::mt19937 rng;                       // Seed pour l'aléatoire, on peut le définir direct pour faire des tests
        bool strategized = false;

        std::vector<Rule> collect(const GameView& view, bool buy) const; // Préparation des rule qu'on applique
        bool chance(double p) const; // Renvoie true à p*100% de chance
        size_t randomIndex(size_t n) const; // Renvoie un indice entre 0 (inclus) et n-1 (inclus)

        const Card* finish(const Card* c, const std::string& why); // Termine la trace lorsqu'on a fait notre choix
        std::vector<const Card*> finishList(std::vector<const Card*> chosen, const std::string& why);   // Même qu'au-dessus mais avec une liste

    public:
        static bool isPurchaseValid(const GameView& view, const Card* card); // Vérifie qu'on peut acheter la carte
        static bool isPlayValid(const GameView& view, const Card* card); // Vérifie qu'on peut jouer la carte

        static constexpr double actionCooldown = 2.0;             // Temps minimum entre 2 actions du bot (en secondes)
        Bot(Hand a_hand, CardStack a_drawStack, CardStack a_discardStack,std::string playerName, Strategy s, DifficultyProfile p, std::uint32_t seed) 
        : Player(a_hand, a_drawStack, a_discardStack, playerName, false), strat(s), profile(p), rng((std::mt19937)seed)
        {
            if(profile.transitionJitter > 0){
                std::uniform_int_distribution<int> d(-profile.transitionJitter, profile.transitionJitter);
                jitter = d(rng);
            }
        }

        Bot(std::string playerName, Strategy s, DifficultyProfile p, std::uint32_t seed=0)
        : Player(playerName), strat(s), profile(p), rng((std::mt19937)seed)
        {
            if(profile.transitionJitter > 0){
                std::uniform_int_distribution<int> d(-profile.transitionJitter, profile.transitionJitter);
                jitter = d(rng);
            }
        }

        Bot(std::string playerName, DifficultyProfile p)
        : Player(playerName), profile(p)
        {
            if(profile.transitionJitter > 0){
                std::uniform_int_distribution<int> d(-profile.transitionJitter, profile.transitionJitter);
                jitter = d(rng);
            }
        }

        void setStrategy(Strategy s){strat = s; strategized = true;};
        void setRng(std::mt19937 r){rng = r;}
        void setProfile(DifficultyProfile p){profile = p;}

        const Card* chooseBuy(const GameView& v);
        const Card* chooseAction(const GameView& v);
        std::vector<const Card*> chooseFromList(const GameView& v, SelectionMode type, SelectionParameters params);
        void showDecisionTrace() const;
        bool hasDecided() const { return !trace.rule.empty() && trace.rule != "none"; }
        void ResolvePhase(const GameView& v) {
            strat.resolvePhase(v, jitter);
        }
        bool hasStrategy() const {
            return strategized;
        }
};