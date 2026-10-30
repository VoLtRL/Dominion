#pragma once

#include <QString>
#include <map>
#include <vector>
#include "shared/abstract/Card.hpp"
#include "shared/structs/GamePhase.hpp"
#include "shared/structs/Selection.hpp"

struct ClientSelectionState {
    SelectionMode mode{SelectionMode::PLAY_CARD};
    SelectionConstraint constraint{SelectionConstraint::EXACTLY};
    int numberOfCards{1};
    bool isPending{false};
    std::vector<const Card*> availableCards;

    bool canSelectAnother(int selectedCount) const {
        return constraint != SelectionConstraint::AT_MOST && constraint != SelectionConstraint::EXACTLY ? true : selectedCount < numberOfCards;
    }

    bool isValid(int selectedCount) const {
        switch (constraint) {
            case SelectionConstraint::NONE:
                return selectedCount == 0;
            case SelectionConstraint::AT_LEAST:
                return selectedCount >= numberOfCards;
            case SelectionConstraint::AT_MOST:
                return selectedCount <= numberOfCards;
            case SelectionConstraint::EXACTLY:
                return selectedCount == numberOfCards;
        }
        return false;
    }
};

struct ClientPlayerState {
    QString name;

    bool isYourself{false};

    int handSize{0};
    int drawStackSize{0};
    int discardStackSize{0};
    const Card* topDiscardCard{nullptr};

    std::vector<const Card*> cardsInPlay;

    bool isActive{false};
};

struct ClientGameState {
    QString selfName;
    QString activePlayerName;
    GamePhase currentPhase;

    int currentPlayerActions{0};
    int currentPlayerPurchasesLeft{0};
    int currentPlayerMoney{0};

    Hand hand;
    std::vector<const Card*> cardsInPlay;
    std::map<const Card*, int> shop;
    
    std::vector<ClientPlayerState> players;
    ClientSelectionState selection;

    const ClientPlayerState* getYourself() const {
        for (const auto& player : players) {
            if (player.isYourself) {
                return &player;
            }
        }
        return nullptr;
    }

    bool isLocalPlayerActive() const {
        const ClientPlayerState* yourself = getYourself();
        return yourself && yourself->isYourself && activePlayerName == yourself->name;
    }

    bool canPlayCard(const Card* card) const {

        if (isLocalPlayerActive() && currentPhase == GamePhase::ACTION && currentPlayerActions > 0 && (card->isA(CardType::ACTION) || card->isA(CardType::TREASURE))) {
            return true;
        }

        return false;
    }

    bool canBuyCard(const Card* card) const {
        if (isLocalPlayerActive() && currentPhase == GamePhase::BUY && currentPlayerPurchasesLeft > 0 && currentPlayerMoney >= card->getCost()) {
            return true;
        }

        return false;
    }

    bool canSelectAnotherCard(int selectedCount) const {
        return selection.canSelectAnother(selectedCount);
    }

    bool isSelectionValid(int selectedCount) const {
        return selection.isValid(selectedCount);
    }
};