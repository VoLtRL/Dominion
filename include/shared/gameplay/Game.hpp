#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "shared/player/Player.hpp"
#include "shared/gameplay/Factory.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "shared/gameplay/TableStack.hpp"
#include "shared/structs/GamePhase.hpp"
#include "shared/structs/Selection.hpp"

class Game {
public:
  enum class SelectionContext {
    NONE,
    WORKSHOP_GAIN,
    REMODEL_TRASH,
    REMODEL_GAIN,
    MINE_TRASH,
    MINE_GAIN,
    CHAPEL_TRASH,
    CELLAR_DISCARD,
    MILITIA_DISCARD,
    BUREAUCRAT_TOPDECK,
    FEAST_GAIN,
    THRONE_ROOM_PLAY,
    THIEF_TRASH,
    THIEF_GAIN,
    SPY_CARD,
    LIBRARY_ACTION,
    CHANCELLOR_DISCARD,
    MONEYLENDER_TRASH
  };

private:
  std::vector<std::unique_ptr<Player>> players;
  
  std::vector<TableStack> KingdomStacks;
  std::vector<TableStack> BasicStacks;

  Player *activePlayer = nullptr;

  GamePhase currentPhase;
  PendingInteractionPhase pendingInteractionPhase{PendingInteractionPhase::NONE};

  std::map<const Card *, int> currentCardDistribution;

  int currentPlayerMoney = 0;
  int currentPlayerActions = 0;
  int currentPlayerPurchaseLeft = 0;
  std::vector<const Card *> CardsInPlay;
  std::vector<const Card *> revealedCards;

  std::vector<const Card *> trash;

  Player* pendingActor = nullptr;
  const Card* pendingCard = nullptr;
  CardType currentType;

  EventType pendingEvent = EventType::INVALID_EVENT;

  std::vector<Player*> pendingPlayers;
  std::set<Player*> currentTargets;

  Player* selectionPlayer = nullptr;
  std::vector<const Card*> selectionCards;

  std::vector<const Card*> pendingThiefTreasures;
  std::vector<const Card*> pendingLibrarySkipped;

  Player *pendingSelectionTarget = nullptr;

  SelectionMode selectionMode = SelectionMode::NONE;
  SelectionConstraint selectionConstraint = SelectionConstraint::NONE;
  size_t selectionCount = 0;
  SelectionContext selectionContext = SelectionContext::NONE;
  int selectionLimitCost = 0;

public:
  Game() = default;

  bool initialize(const GameMode& gameMode,const std::vector<QString> &playerNames);
  Player* getPlayerByName(const std::string& name);

  const std::vector<TableStack> &getKingdomStacks();
  const std::vector<TableStack> &getBasicStacks();

  void addPlayer(std::unique_ptr<Player> player);
  std::vector<std::unique_ptr<Player>> getPlayers();
  const std::vector<std::unique_ptr<Player>> &getPlayers() const;

  const Player* getActivePlayer() const;
  GamePhase getCurrentPhase() const { return currentPhase; }
  PendingInteractionPhase getCurrentPendingInteractionPhase() const { return pendingInteractionPhase; }
  int getCurrentPlayerMoney() const { return currentPlayerMoney; }
  int getCurrentPlayerActions() const { return currentPlayerActions; }
  int getCurrentPlayerPurchasesLeft() const { return currentPlayerPurchaseLeft; }
  void setActivePlayer(Player *player);

  void setCurrentCardDistribution(const std::map<const Card *, int> &cardDistribution);
  void insertCardDistribution(const std::map<const Card *, int> &cardDistribution);

  std::map<const Card *, int> getCurrentCardDistribution() const {
    return currentCardDistribution;
  }

  const std::vector<const Card *> &getCardsInPlay() const {
    return CardsInPlay;
  }
  void addCardToPlay(const Card *card) { CardsInPlay.push_back(card); }


  bool isPlayerTurn(Player *player) const { return player == activePlayer; }

  void addGold(int amount);
  void trashCard(const Card *card) { trash.push_back(card); }

  void addPlayerActions(int amount) { currentPlayerActions += amount; }

  void addPlayerPurchases(int amount) { currentPlayerPurchaseLeft += amount; }

  void discardCardsInPlay();

  std::vector<std::string> parseLines(const std::string &) const;

  std::vector<const Card *>
  chooseCardsFromSelection(const std::vector<const Card *> &selection,
                           size_t numberOfCardsToChoose,
                           const std::string &action, std::string mode,
                           const std::set<CardType> &allowedTypes) const;
  void displaySelection(const std::vector<const Card *> &selection) const;

  const std::vector<const Card *> &getRevealedCards() const {
    return revealedCards;
  }
  void addRevealedCard(const Card *card) { revealedCards.push_back(card); }


  void clearRevealedCards();
  std::vector<const Card *> getBuyableCardsBelowCost(int cost) const;
  bool isStackEmpty(std::string cardName) const;

  std::string HUDFormat() const;

  const Card *getCardFromCardDistrib(std::string cardName);

  void playAttackCard(const Card *card);

  std::vector<Player *> getOtherPlayers();


  bool canPlayAction(Player* p, const Card* card) const;
  bool tryPlayAction(Player* p, const Card* card);

  bool canPlayTreasure(Player* p, const Card* card) const;
  bool tryPlayTreasure(Player* p, const Card* card);
  bool tryPlayAllTreasures(Player* p);

  bool canPlayAttack(Player* p, const Card* card) const;
  bool tryPlayAttack(Player* p, const Card* card);

  bool canPlayReaction(Player* p, const Card* card, EventType type, Player* actor);
  bool tryPlayReaction(Player* p, const Card* card, EventType type, Player* actor);
  bool applyProtection(Player*p, Player* attacker, const Card* card);

  bool canBuyCard(Player* p, const Card* card) const;
  bool tryBuyCard(Player* p, const Card* card);

  bool tryNextPhase(Player* p);

  bool trySendSelection(Player* p, const std::vector<const Card*>& selection);
  void requestSelection(Player *p, const std::vector<const Card*> &cards,
                        size_t count, SelectionMode mode,
                        SelectionConstraint constraint, SelectionContext context,
                        int limitCost = 0);
  bool hasPendingSelection() const { return selectionPlayer != nullptr; }
  Player *getSelectionPlayer() const { return selectionPlayer; }
  const std::vector<const Card*> &getSelectionCards() const { return selectionCards; }
  SelectionMode getSelectionMode() const { return selectionMode; }
  SelectionConstraint getSelectionConstraint() const { return selectionConstraint; }
  size_t getSelectionCount() const { return selectionCount; }
  SelectionContext getSelectionContext() const { return selectionContext; }
  int getSelectionLimitCost() const { return selectionLimitCost; }
  void clearSelection();
  void setPendingThiefTreasures(const std::vector<const Card*> &cards) {
    pendingThiefTreasures = cards;
  }
  const std::vector<const Card*> &getPendingThiefTreasures() const {
    return pendingThiefTreasures;
  }
  void clearPendingThiefTreasures() { pendingThiefTreasures.clear(); }
  std::vector<const Card*> &getPendingLibrarySkipped() { return pendingLibrarySkipped; }
  void clearPendingLibrarySkipped() { pendingLibrarySkipped.clear(); }
  void setPendingSelectionTarget(Player *p) { pendingSelectionTarget = p; }
  Player *getPendingSelectionTarget() const { return pendingSelectionTarget; }

  void passToNextPlayer();
  bool performCleanup();
  void resetCurrentPlayerStats(); 

  bool hasGameEnded() const;
  Player* getWinner() const;

  void setCurrentTargets(std::set<Player*> targets){
    currentTargets = targets;
  }

  void setPendingCard(const Card* card){
    pendingCard = card;
  }

  void setPendingActor(Player* player) {
    pendingActor = player;
  }

  Player* getPendingActor() const {
    return pendingActor;
  }

  void setPendingEvent(EventType event) {
    pendingEvent = event;
  }

  EventType getPendingEvent() const {
    return pendingEvent;
  }

  void setPendingPlayers(std::vector<Player*> p){
    pendingPlayers = p;
  }

  void clearPendingPlayers() {
    pendingPlayers.clear();
  }
  const std::vector<Player*> &getPendingPlayers() const {
    return pendingPlayers;
  }

  bool isPlayerTargetted(Player* p) const {
    return std::find(currentTargets.begin(), currentTargets.end(), p) != currentTargets.end();
  }

  bool isPlayerPending(Player* p) const {
    return std::find(pendingPlayers.begin(), pendingPlayers.end(), p) != pendingPlayers.end();
  }

  bool removePendingPlayers(Player* p){
    auto i = std::find(pendingPlayers.begin(), pendingPlayers.end(), p);
    if(i == pendingPlayers.end()) { return false; }
    pendingPlayers.erase(i);
    return true;
  }

  void setCurrentType(CardType t) {
    currentType = t;
  }

  CardType getCurrentType() {
    return currentType;
  }

  Player* getPlayerFromName(std::string name) const {
    for(const std::unique_ptr<Player>& p : players) {
      if(p->getName() == name) { return p.get(); }
    }
    return nullptr;
  }

  const Card* getPendingCard() const {
    return pendingCard;
  }

  std::set<Player*> getCurrentTargets() const {
    return currentTargets;
  }

  std::map<const Card*, int> getCurrentCardDistrib() const {
    return currentCardDistribution;
  }
 
  bool waitingForPlayers() const {
    return !pendingPlayers.empty();
  }

  bool canExecuteTurnAction(Player* p) const;

  void removeCardFromPlay(const Card* card);


};
