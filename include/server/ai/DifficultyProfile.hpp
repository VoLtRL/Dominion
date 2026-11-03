#pragma once

#include <string>

struct DifficultyProfile{

    std::string name;                       // Juste son nom
    int maxTier = 4;                        // Le tier max de stratégie que ce profile peut utiliser
    double buyErrorRate = 0.0;              // Proba de prendre la 2e ou 3e option d'achat
    double skipBuyRate = 0.0;               // Proba de rien acheter
    int transitionJitter = 0;               // Décalage max (intervalle centré en 0 [-t,t]) tiré au début de partie
    double playErrorRate = 0.0;             // Proba de mal faire une action
    double minorDecisionErrorRate = 0.0;    // Proba d'avoir un résultat aproximatif sur la défausse, le gain ou l'écart
    int simulationDepth = 0;                // Honnnêtement je pense pas l'utiliser, mais c'est pour tester des coups en avance

    static DifficultyProfile Easy() {
        return {"Easy", 2, 0.35, 0.15, 3, 0.25, 0.40, 0};
    }
    static DifficultyProfile Medium() {
        return {"Medium", 3, 0.15, 0.05, 2, 0.10, 0.20, 0};
    }
    static DifficultyProfile Hard() {
        return {"Hard", 4, 0.05, 0.01, 1, 0.03, 0.05, 0};
    }
    static DifficultyProfile Extreme() {
        return {"Extreme", 4, 0.0, 0.0, 0, 0.0, 0.0, 50};   // Bon lui pas sûr qu'il aura le système de calcul de coups à l'avance
    }
};