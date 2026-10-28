#pragma once

#include <vector>

#include "shared/events/AttackEvent.hpp"
#include "shared/player/Player.hpp"
#include "shared/gameplay/Factory.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "shared/gameplay/TableStack.hpp"
#include "shared/structs/GamePhase.hpp"

class Game {
private:
  std::vector<Player> players;
  const std::vector<GameMode> gameModes = {
      GameMode("Les premières parties",
               "Configuration recommandée pour découvrir le jeu",
               {{Factory::get("Workshop"), 10},
                {Factory::get("Woodcutter"), 10},
                {Factory::get("Cellar"), 10},
                {Factory::get("Moat"), 10},
                {Factory::get("Smithy"), 10},
                {Factory::get("Market"), 10},
                {Factory::get("Militia"), 10},
                {Factory::get("Mine"), 10},
                {Factory::get("Remodel"), 10},
                {Factory::get("Village"), 10}}),

      GameMode("Richesses et trésors",
               "Aventurier, Bureaucrate, Chancelier, Chapelle, Festin, "
               "Laboratoire, Marché, Mine, Prêteur sur gages, Salle du Trône",
               {{Factory::get("Adventurer"), 10},
                {Factory::get("Bureaucrat"), 10},
                {Factory::get("Chancellor"), 10},
                {Factory::get("Chapel"), 10},
                {Factory::get("Feast"), 10},
                {Factory::get("Laboratory"), 10},
                {Factory::get("Market"), 10},
                {Factory::get("Mine"), 10},
                {Factory::get("Moneylender"), 10},
                {Factory::get("ThroneRoom"), 10}}),

      GameMode("Interaction",
               "Bibliothèque, Bureaucrate, Chambre du conseil, Chancelier, "
               "Douves, Espion, Festival, Milice, Village, Voleur",
               {{Factory::get("Library"), 10},
                {Factory::get("Bureaucrat"), 10},
                {Factory::get("CouncilRoom"), 10},
                {Factory::get("Chancellor"), 10},
                {Factory::get("Moat"), 10},
                {Factory::get("Spy"), 10},
                {Factory::get("Festival"), 10},
                {Factory::get("Militia"), 10},
                {Factory::get("Village"), 10},
                {Factory::get("Thief"), 10}}),

      GameMode(
          "Changement de taille",
          "Atelier, Bûcheron, Cave, Chapelle, Festin, Jardins, Laboratoire, "
          "Sorcière, Village, Voleur",
          {{Factory::get("Workshop"), 10},
           {Factory::get("Woodcutter"), 10},
           {Factory::get("Cellar"), 10},
           {Factory::get("Chapel"), 10},
           {Factory::get("Feast"), 10},
           {Factory::get("Gardens"), 12},
           {Factory::get("Laboratory"), 10},
           {Factory::get("Witch"), 10},
           {Factory::get("Village"), 10},
           {Factory::get("Thief"), 10}}),

      GameMode("Place du Village",
               "Bibliothèque, Bûcheron, Bureaucrate, Cave, Festival, Forgeron, "
               "Marché, Rénovation, Salle du Trône, Village",
               {{Factory::get("Library"), 10},
                {Factory::get("Woodcutter"), 10},
                {Factory::get("Bureaucrat"), 10},
                {Factory::get("Cellar"), 10},
                {Factory::get("Festival"), 10},
                {Factory::get("Smithy"), 10},
                {Factory::get("Market"), 10},
                {Factory::get("Remodel"), 10},
                {Factory::get("ThroneRoom"), 10},
                {Factory::get("Village"), 10}})};

  std::vector<TableStack> KingdomStacks;
  std::vector<TableStack> BasicStacks;

  Player *activePlayer = nullptr;
  const GameMode *currentGameMode = nullptr;

  GamePhase currentPhase;

  std::map<const Card *, int> currentCardDistribution;

  int currentPlayerMoney = 0;
  int currentPlayerActions = 0;
  int currentPlayerPurchaseLeft = 0;
  std::vector<const Card *> CardsInPlay;
  std::vector<const Card *> revealedCards;

  std::vector<const Card *> trash;

public:
  Game() = default;

  const std::vector<TableStack> &getKingdomStacks();
  const std::vector<TableStack> &getBasicStacks();

  void addPlayer(Player player);
  const std::vector<Player> &getPlayers() const;

  const Player &getActivePlayer() const;
  void setActivePlayer(Player *player);

  void setCurrentGameMode(const GameMode *gameMode);
  const GameMode *getCurrentGameMode() const;

  void setCurrentCardDistribution(
      const std::map<const Card *, int> &cardDistribution);
  void
  insertCardDistribution(const std::map<const Card *, int> &cardDistribution);

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

  void activateReactions(GameEvent *event);

  void playAttackCard(const Card *card);

  std::vector<Player *> getOtherPlayers();

  void playAction(Player& p, const Card* card);

  void playTreasure(Player& p, const Card* card);

  bool canPlayAction(Player* p, const Card* card) const;
  bool tryPlayAction(Player* p, const Card* card);

  bool canPlayTreasure(Player* p, const Card* card) const;
  bool tryPlayTreasure(Player* p, const Card* card);
  bool tryPlayAllTreasures(Player* p);

  bool canBuyCard(Player* p, const Card* card) const;
  bool tryBuyCard(Player* p, const Card* card);

  bool tryNextPhase(Player* p); 

  bool trySendSelection(Player* p, const std::vector<const Card*>& selection);

  void passToNextPlayer();
  void performCleanup();
  bool hasGameEnded() const;
  Player getWinner() const;
};
