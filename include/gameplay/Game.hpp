#pragma once

#include <vector>

#include "Factory.hpp"
#include "../player/Player.hpp"
#include "GameMode.hpp"
#include "TableStack.hpp"

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

    GameMode("Changement de taille",
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

  std::map<const Card *, int> currentCardDistribution;

  int currentPlayerMoney = 0;
  int currentPlayerActions = 0;
  int currentPlayerPurchaseLeft = 0;
  std::vector<const Card *> CardsInPlay;

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

  void setCurrentCardDistribution(const std::map<const Card *, int> &cardDistribution);
  void insertCardDistribution(const std::map<const Card *, int> &cardDistribution);

  const std::vector<const Card *> &getCardsInPlay() const {
    return CardsInPlay;
  }
  void addCardToPlay(const Card *card) { CardsInPlay.push_back(card); }

  void useTreasureCards(Player &player, Game &game);
  void ActionPhase(Player player);

  void BuyPhase(Player player);

  void playTurn(Player player);

  void addGold(int amount);
  void trashCard(const Card *card) { trash.push_back(card); }

  void addPlayerActions(int amount) { currentPlayerActions += amount; }

  void addPlayerPurchases(int amount) { currentPlayerPurchaseLeft += amount; }

  Player getWinner();

  void launchGame();

  std::vector<std::string> parseLines(const std::string &) const;

  std::vector<const Card *> chooseCardsFromSelection(const std::vector<const Card *> &selection,
                           size_t numberOfCardsToChoose,
                           const std::string &action, std::string mode,
                           const std::set<CardType> &allowedTypes) const;
  void displaySelection(const std::vector<const Card *> &selection) const;

  void hasGameEnded();

};
