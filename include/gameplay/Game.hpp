#pragma once

#include <vector>

#include "Factory.hpp"
#include "../player/Player.hpp"
#include "GameMode.hpp"
#include "TableStack.hpp"

class Game {
private:
  std::vector<Player> players;
  const static std::vector<GameMode> gameModes;
  std::vector<TableStack> KingdomStacks;
  std::vector<TableStack> BasicStacks;

  Player *activePlayer;
  const GameMode *currentGameMode;

  std::map<const Card *, int> currentCardDistribution;

  int currentPlayerMoney = 0;
  int currentPlayerActions = 0;
  int currentPlayerPurchaseLeft = 0;
  std::vector<const Card *> CardsInPlay;

  std::vector<const Card *> trash;

public:
  Game();

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
