#pragma once

#include <QString>
#include <vector>
#include "shared/abstract/Card.hpp"

struct ClientPlayerState {
    QString name;

    int handSize{0};
    int drawStackSize{0};
    int discardStackSize{0};
    const Card* topDiscardCard{nullptr};

    std::vector<const Card*> cardsInPlay;

    bool isActive{false};
};

struct ClientGameState {
    QString selfName;
    QString currentPhase;

    int currentPlayerActions{0};
    int currentPlayerPurchasesLeft{0};
    int currentPlayerMoney{0};

    Hand hand;
    std::vector<const Card*> cardsInPlay;

    int selfDrawStackSize{0};
    int selfDiscardStackSize{0};
    const Card* selfTopDiscardCard{nullptr};
    
    std::vector<ClientPlayerState> otherPlayers;
};