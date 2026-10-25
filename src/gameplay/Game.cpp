#include "../../include/gameplay/Game.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>



const std::vector<Player> &Game::getPlayers() const { return players; }

const Player &Game::getActivePlayer() const { return *activePlayer; }

void Game::addPlayer(Player player) { players.push_back(player); }

void Game::setActivePlayer(Player *player) { activePlayer = player; }

void Game::setCurrentGameMode(const GameMode *gameMode) {
  currentGameMode = gameMode;
}
void Game::insertCardDistribution(
    const std::map<const Card *, int> &cardDistribution) {
  for (const auto &pair : cardDistribution) {
    currentCardDistribution[pair.first] += pair.second;
  }
}

void Game::setCurrentCardDistribution(
    const std::map<const Card *, int> &cardDistribution) {
  currentCardDistribution = cardDistribution;
}

const GameMode *Game::getCurrentGameMode() const { return currentGameMode; }

void Game::clearRevealedCards() { revealedCards.clear(); }

void Game::ActionPhase() {
  while (currentPlayerActions > 0) {
    std::cout << "You have " << currentPlayerActions << " actions left."
              << std::endl;
    SelectionParameters params = InputManager::createSelectionParameters(1, "Play one card", HUDFormat(), SelectionMode::EXACTLY, {CardType::ACTION}, activePlayer->getHand().getCards());
    std::vector<const Card*> card = InputManager::chooseCardsFromSelection(*activePlayer, getCurrentPhase(), params);
    if (card.empty()) {
      break;
    }
    std::cout << "You played the " << card[0]->getName() << std::endl;
    playAction(*activePlayer, card[0]);
    if (card[0]->isA(CardType::ATTACK)) {
      std::cout << "Attacking other players" << std::endl;
      playAttackCard(card[0]);
    }
    currentPlayerActions--;
    std::cout << "Your current hand is : " << std::endl;
    InputManager::displaySelection(activePlayer->getHand().getCards());
  }
  useTreasureCards();
  std::cout << "Action phase ended." << std::endl;
}

void Game::useTreasureCards() {

  SelectionParameters params = InputManager::createSelectionParameters(activePlayer->getHand().getSize(), "Play treasures", HUDFormat(), SelectionMode::AT_MOST, {CardType::TREASURE}, activePlayer->getHand().getCards(), true);
  std::vector<const Card*> treasureCards = InputManager::chooseCardsFromSelection(*activePlayer, getCurrentPhase(), params);
  for (const Card *card : treasureCards) {
    playTreasure(*activePlayer, card);
    continue;
  }
}

void Game::BuyPhase() {
  while (currentPlayerPurchaseLeft) {
    std::cout << "You have " << currentPlayerPurchaseLeft << " purchases left"
              << std::endl;
    std::map<int, std::vector<const Card *>, std::greater<int>> display;

    for (const auto &pair : currentCardDistribution) {
      const Card *card = pair.first;
      int quantity = pair.second;

      if (quantity > 0) {
        display[card->getCost()].push_back(card);
      }
    }

    for (auto pair : display) {
      std::cout << pair.first;
      InputManager::displaySelection(pair.second);
    }

    std::string card_name;

    std::cout << "Which card do you want to buy ? Enter the name of the card "
                 "or -1 to skip buying a card"
              << std::endl;
    std::cout << "You have " << currentPlayerMoney << " coins." << std::endl;

    std::cin >> card_name;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (card_name == "-1") {
      std::cout << "You chose not to buy any card." << std::endl;
      break;
    }

    const Card *card = Factory::get(card_name);
    if (!card) {
      std::cout << "Invalid card name. Please try again." << std::endl;
      continue;
    }
    if (card->getCost() > currentPlayerMoney) {
      std::cout
          << "You don't have enough money to buy this card. Please try again."
          << std::endl;
      continue;
    }
    if (currentCardDistribution[card] <= 0) {
      std::cout << "This card is out of stock. Please try again." << std::endl;
      continue;
    }
    currentCardDistribution[card]--;
    currentPlayerMoney -= card->getCost();
    activePlayer->addCardToDiscardStack(card);
    addPlayerPurchases(-1);
  }
}

void Game::hasGameEnded() {
  int emptyStacks = 0;
  for (const TableStack &stack : KingdomStacks) {
    if (stack.getQuantity() == 0) {
      emptyStacks++;
    }
  }
  for (const TableStack &stack : BasicStacks) {
    if (stack.getQuantity() == 0) {
      emptyStacks++;
    }
    if (stack.getCard()->getName() == "Province" && stack.getQuantity() == 0) {
      std::cout << "Game over! The game has ended due to the Province stack "
                   "being empty."
                << std::endl;
      std::cout << "The winner is: " << getWinner().getString() << std::endl;
      exit(0);
    }
  }

  if (emptyStacks >= 3) {
    std::cout << "Game over! The game has ended due to three empty stacks."
              << std::endl;
    std::cout << "The winner is: " << getWinner().getString() << std::endl;
    exit(0);
  }
}

void Game::playTurn(Player &player) {
  setActivePlayer(&player);
  player.addTurnPlayed();
  InputManager::displaySelection(activePlayer->getHand().getCards());
  std::cout << "It is " << activePlayer->getName() << "'s turn." << std::endl;
  ActionPhase();
  BuyPhase();
  discardCardsInPlay();
  player.discardHand();
  player.drawCardsToHand(5);
  hasGameEnded();
}

void Game::addGold(int amount) { currentPlayerMoney += amount; }

void Game::discardCardsInPlay() {
  for (auto card : CardsInPlay) {
    activePlayer->addCardToDiscardStack(card);
  }
  CardsInPlay.clear();
}

Player Game::getWinner() {
  Player winner = players[0];
  int maxScore = 0;

  for (const Player &player : players) {
    if (player.getVictoryPoints() > maxScore) {
      maxScore = player.getVictoryPoints();
      winner = player;
    }
  }

  return winner;
}

void Game::launchGame() {
  std::cout << "Welcome to Dominion!" << std::endl;
  std::cout << "Available game modes:" << std::endl;
  for (size_t i = 0; i < gameModes.size(); ++i) {
    std::cout << i + 1 << ". " << gameModes[i].getName() << " - "
              << gameModes[i].getDescription() << std::endl;
  }
  std::cout
      << "Please select a game mode by entering the corresponding number: ";
  int choice;
  std::cin >> choice;

  std::cin.ignore();

  if (choice < 1 || static_cast<size_t>(choice) > gameModes.size()) {
    std::cout << "Invalid choice." << std::endl;
    return;
  }
  setCurrentGameMode(&gameModes[choice - 1]);
  std::cout << "selected " << currentGameMode->getName() << std::endl;

  std::cout << "Will you play alone ? Enter the number of players (4 players "
               "max) [default : 1 player]"
            << std::endl;
  choice = 1;
  std::cin >> choice;
  std::cin.ignore();
  if (choice < 1 || choice > 4) {
    std::cout << "Invalid choice." << std::endl;
  }
  std::cout << "You have chosen to play with " << choice << " players."
            << std::endl;

  auto usualDistribution = std::map<const Card *, int>();
  usualDistribution.insert({{Factory::get("Copper"), 60 - 7 * choice},
                            {Factory::get("Curse"), 10 * (choice - 1)},
                            {Factory::get("Silver"), 40},
                            {Factory::get("Gold"), 30}});
  switch (choice) {
  case 1:
  case 2:
    usualDistribution.insert({
        {Factory::get("Estate"), 8},
        {Factory::get("Duchy"), 8},
        {Factory::get("Province"), 8},
    });
    break;
  case 3:
  case 4:
    usualDistribution.insert({
        {Factory::get("Estate"), 12},
        {Factory::get("Duchy"), 12},
        {Factory::get("Province"), 12},
    });
    break;
  }

  auto cardDistribution = currentGameMode->getCardDistribution();
  auto combinedDistribution = std::map<const Card *, int>(usualDistribution);
  combinedDistribution.insert(cardDistribution.begin(), cardDistribution.end());
  setCurrentCardDistribution(combinedDistribution);

  for (int i = 0; i < choice; ++i) {
    std::cout << "Enter the name of player " << i + 1 << ": ";
    std::string playerName;
    std::cin >> playerName;
    std::cin.ignore();
    CardStack drawStack = CardStack(
        {Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Estate"), Factory::get("Estate"),
         Factory::get("Estate")});

    drawStack.shuffle();
    Hand hand;
    CardStack discardStack = CardStack();
    hand.refillHandFromDrawStack(drawStack, discardStack);
    Player player(hand, drawStack, discardStack, playerName);
    addPlayer(player);
  }

  while (true) {
    for (Player &player : players) {
      currentPlayerActions = 1;
      currentPlayerPurchaseLeft = 1;
      currentPlayerMoney = 0;
      playTurn(player);
    }
  }

  std::cout << "Game is over!" << std::endl;
  std::cout << "The winner is:" << getWinner().getString() << std::endl;
}

const std::vector<TableStack> &Game::getKingdomStacks() {
  return KingdomStacks;
}

const std::vector<TableStack> &Game::getBasicStacks() { return BasicStacks; }

std::vector<const Card *> Game::getBuyableCardsBelowCost(int cost) const {
  std::vector<const Card *> validCards;
  for (const auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if (card->getCost() <= cost && quantity > 0) {
      validCards.push_back(card);
    }
  }
  return validCards;
}

const Card *Game::getCardFromCardDistrib(std::string cardName) {
  for (const auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if (card->getName() == cardName) {
      if (quantity > 0) {
        return Factory::get(card->getName());
      }
      return nullptr;
    }
  }
  return nullptr;
}

bool Game::isStackEmpty(std::string cardName) const {
  for (const auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if (card->getName() == cardName) {
      return quantity == 0;
    }
  }
  return true;
}

std::string Game::HUDFormat() const {
  std::string hud = "[" + activePlayer->getString() + " | " +
                    std::to_string(currentPlayerMoney) + " coins | " +
                    std::to_string(currentPlayerActions) + " actions | " +
                    std::to_string(currentPlayerPurchaseLeft) +
                    " purchases left] ";
  return hud;
}

void Game::activateReactions(GameEvent *event) {
  for (Player &player : players) {
    auto possibleReactions = player.getAvailableReactions();

    for (const Reaction *reactionCard : possibleReactions) {
      if (reactionCard->canReact(event, this, &player)) {
        reactionCard->react(event, this, &player);
      }
    }
  }
}

std::vector<Player *> Game::getOtherPlayers() {
  std::vector<Player *> res;
  for (Player &player : players) {
    if (player != *activePlayer) {
      res.push_back(&player);
    }
  }
  return res;
}

void Game::playAttackCard(const Card *card) {
  std::vector<Player *> others = getOtherPlayers();
  AttackEvent event(card, *activePlayer,
                    std::set(others.begin(), others.end()));
  activateReactions(&event);

  std::set<Player *> newTargets;

  for (Player *p : event.getTargets()) {
    std::cout << p->getName() << std::endl;
    if (!event.isIgnoredFor(*p)) {
      newTargets.insert(p);
    }
  }
  const Attack* attack = dynamic_cast<const Attack*>(card);
  attack->attack(*this, *activePlayer, newTargets);
}

void Game::playAction(Player &p, const Card *card) {
  p.removeCardFromHand(card);
  addCardToPlay(card);
  const Action* action = dynamic_cast<const Action*>(card);
  action->play(*this, p);
}

void Game::playTreasure(Player&p, const Card* card){
  const Treasure* treasure = dynamic_cast<const Treasure*>(card);
  addGold(treasure->getCoins());
}
