#pragma once

#include <random>

#include "../player/Player.hpp"
#include "Strategy.hpp"
#include "Rule.hpp"
#include "DifficultyProfile.hpp"

enum class SelectionMode{
    EXACTLY,
    AT_MOST,
    AT_LEAST
};

struct SelectionParameters{
    size_t nbToSelect;
    std::string label;
    std::string presentation;
    SelectionMode mode;
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
        std::mt19937 rng;                       // Seed pour l'aléatoire, on peut le définir direct pour faire des tests

        std::vector<Rule> collect(const GameView& view, bool buy) const; // Préparation des rule qu'on applique
        static bool isPurchaseValid(const GameView& view, const Card* card); // Vérifie qu'on peut acheter la carte
        static bool isPlayValid(const GameView& view, const Card* card); // Vérifie qu'on peut jouer la carte

        bool chance(double p) const; // Renvoie true à p*100% de chance
        size_t randomIndex(size_t n) const; // Renvoie un indice entre 0 (inclus) et n-1 (inclus)

        const Card* finish(const Card* c, const std::string& why); // Termine la trace lorsqu'on a fait notre choix
        std::vector<const Card*> finishList(std::vector<const Card*> chosen, const std::string& why);   // Même qu'au-dessus mais avec une liste

    public:

        Bot(Hand a_hand, CardStack a_drawStack, CardStack a_discardStack,std::string playerName, Strategy s, DifficultyProfile p, std::uint32_t seed) 
        : Player(a_hand, a_drawStack, a_discardStack, playerName)
        {
            strat = s;
            profile = p;
            rng = (std::mt19937)seed;
            if(profile.transitionJitter > 0){
                std::uniform_int_distribution<int> d(-profile.transitionJitter, profile.transitionJitter);
                jitter = d(rng);
            }
        }

        void setStrategy(Strategy s){strat = s;};
        void setRng(std::mt19937 r){rng = r;}
        void setProfile(DifficultyProfile p){profile = p;}

        bool isHuman() const {return false;}
        const Card* chooseBuy(const GameView& v);
        const Card* chooseAction(const GameView& v);
        std::vector<const Card*> chooseFromList(const GameView& v, DecisionType type, SelectionParameters params);
        void playTurn(GameView& v);
};