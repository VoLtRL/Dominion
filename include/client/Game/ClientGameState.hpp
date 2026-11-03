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

    bool operator==(const ClientSelectionState& other) const {
        return mode == other.mode
                && constraint == other.constraint
                && numberOfCards == other.numberOfCards
                && isPending == other.isPending
                && availableCards == other.availableCards;
    }

    bool operator!=(const ClientSelectionState& other) const {
        return !(*this == other);
    }

};



struct ClientPlayerState {
    QString name;
    Qt::GlobalColor color{Qt::white};

    bool isYourself{false};
    
    int handSize{0};
    int drawStackSize{0};
    int discardStackSize{0};
    const Card* topDiscardCard{nullptr};

    std::vector<const Card*> cardsInPlay;

    bool isActive{false};

    bool operator==(const ClientPlayerState& other) const {
        return name == other.name
                && color == other.color
                && isYourself == other.isYourself
                && handSize == other.handSize
                && drawStackSize == other.drawStackSize
                && discardStackSize == other.discardStackSize
                && topDiscardCard == other.topDiscardCard
                && cardsInPlay == other.cardsInPlay
                && isActive == other.isActive;
    }

    bool operator!=(const ClientPlayerState& other) const {
        return !(*this == other);
    }

};

struct ClientGameState {
    QString selfName;
    QString activePlayerName;
    GamePhase currentPhase;
    PendingInteractionPhase currentPendingInteractionPhase{PendingInteractionPhase::NONE};

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

        if (isLocalPlayerActive() &&
            ((currentPhase == GamePhase::ACTION && currentPlayerActions > 0 && card->isA(CardType::ACTION)) ||
             (currentPhase == GamePhase::BUY && card->isA(CardType::TREASURE)))) {
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

    bool canReact(const Card* card) const {
        if (isLocalPlayerActive() && currentPendingInteractionPhase == PendingInteractionPhase::REACTION && (card == nullptr || card->isA(CardType::REACTION))) {
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

    bool operator==(const ClientGameState& other) const {
        return selfName == other.selfName
                && activePlayerName == other.activePlayerName
                && currentPhase == other.currentPhase
                && currentPendingInteractionPhase == other.currentPendingInteractionPhase
                && currentPlayerActions == other.currentPlayerActions
                && currentPlayerPurchasesLeft == other.currentPlayerPurchasesLeft
                && currentPlayerMoney == other.currentPlayerMoney
                && hand.getCards() == other.hand.getCards()
                && cardsInPlay == other.cardsInPlay
                && shop == other.shop
                && players == other.players
                && selection == other.selection;
    }

    bool operator!=(const ClientGameState& other) const {
        return !(*this == other);
    }

};