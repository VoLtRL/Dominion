#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "shared/player/Player.hpp"
#include "shared/gameplay/Factory.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "shared/gameplay/TableStack.hpp"
#include "shared/structs/GamePhase.hpp"

class Game {
private:
  std::vector<Player> players;
  
  std::vector<TableStack> KingdomStacks;
  std::vector<TableStack> BasicStacks;

  Player *activePlayer = nullptr;

  GamePhase currentPhase;

  std::map<const Card *, int> currentCardDistribution;

  int currentPlayerMoney = 0;
  int currentPlayerActions = 0;
  int currentPlayerPurchaseLeft = 0;
  std::vector<const Card *> CardsInPlay;
  std::vector<const Card *> revealedCards;

  std::vector<const Card *> trash;

  const Card* pendingCard;
  std::vector<Player*> pendingPlayers;
  std::set<Player*> currentTargets;
  CardType currentType;

public:
  Game() = default;

  bool initialize(const GameMode& gameMode,const std::vector<QString> &playerNames);
  Player* getPlayerByName(const std::string& name);

  const std::vector<TableStack> &getKingdomStacks();
  const std::vector<TableStack> &getBasicStacks();

  void addPlayer(Player player);
  const std::vector<Player> &getPlayers() const;

  const Player &getActivePlayer() const;
  GamePhase getCurrentPhase() const { return currentPhase; }
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

  void useTreasureCards();
  void ActionPhase();

  void BuyPhase();

  void playTurn(Player &player);

  void addGold(int amount);
  void trashCard(const Card *card) { trash.push_back(card); }

  void addPlayerActions(int amount) { currentPlayerActions += amount; }

  void addPlayerPurchases(int amount) { currentPlayerPurchaseLeft += amount; }

  void discardCardsInPlay();

  void launchGame();

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
  void putCardsinDiscardStack(std::vector<const Card *> &cards);


  void clearRevealedCards();
  std::vector<const Card *> getBuyableCardsBelowCost(int cost) const;
  void buyCard(const Card *card);
  bool isStackEmpty(std::string cardName) const;

  std::string HUDFormat() const;

  const Card *getCardFromCardDistrib(std::string cardName);

  //void activateReactions(GameEvent *event);

  void playAttackCard(const Card *card);

  std::vector<Player *> getOtherPlayers();

  void playAction(Player& p, const Card* card);

  void playTreasure(Player& p, const Card* card);

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

  void passToNextPlayer();
  bool performCleanup();
  bool hasGameEnded() const;
  Player getWinner() const;

  void setCurrentTargets(std::set<Player*> targets){
    currentTargets = targets;
  }

  void setPendingCard(const Card* card){
    pendingCard = card;
  }

  void setPendingPlayers(std::vector<Player*> p){
    pendingPlayers = p;
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
    for(const Player& p : players) {
      if(p.getName() == name) { return const_cast<Player*>(&p);}
    }
    return nullptr;
  }

  bool waitingForPlayers() const {
    return !pendingPlayers.empty();
  }

  const Card* getPendingCard() const {
    return pendingCard;
  }

  std::set<Player*> getCurrentTargets() const {
    return currentTargets;
  }
};
