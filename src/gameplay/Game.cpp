#include "../../include/gameplay/Game.hpp"
#include <algorithm>
#include <iostream>
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

std::vector<std::string> Game::parseLines(const std::string &str) const {
  std::vector<std::string> lines;
  std::string line;
  for (const char c : str) {
    if (c == '\n') {
      lines.push_back(line);
      line.clear();
    } else {
      line += c;
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

void Game::displaySelection(const std::vector<const Card *> &selection) const {
  const std::string gap = "  ";
  const size_t cardsPerRow = 5;

  for (size_t start = 0; start < selection.size(); start += cardsPerRow) {
    const size_t end = std::min(start + cardsPerRow, selection.size());

    std::vector<std::vector<std::string>> cards;
    std::vector<size_t> widths;
    size_t maxHeight = 0;

    // Découper chaque carte en lignes
    for (size_t i = start; i < end; ++i) {
      std::vector<std::string> lines = parseLines(selection[i]->getString());
      widths.push_back(lines.empty() ? 0 : lines[0].size());
      maxHeight = std::max(maxHeight, lines.size());
      cards.push_back(std::move(lines));
    }

    // Étirer les cartes trop courtes : lignes vides avant la bordure basse
    for (size_t c = 0; c < cards.size(); ++c) {
      auto &lines = cards[c];
      if (lines.size() >= maxHeight || lines.size() < 2)
        continue;

      const size_t w = widths[c];
      // ligne vide : "|" + espaces + "|"
      const std::string emptyRow = "|" + std::string(w - 2, ' ') + "|";

      const size_t missing = maxHeight - lines.size();
      lines.insert(lines.end() - 1, missing, emptyRow);
    }

    // Affichage ligne par ligne
    for (size_t row = 0; row < maxHeight; ++row) {
      for (size_t c = 0; c < cards.size(); ++c) {
        if (row < cards[c].size())
          std::cout << cards[c][row];
        else
          std::cout << std::string(widths[c], ' ');

        if (c + 1 < cards.size())
          std::cout << gap;
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
}

std::vector<const Card *>
Game::chooseCardsFromSelection(const std::vector<const Card *> &selection,
                               size_t numberOfCardsToChoose,
                               const std::string &action, std::string mode,
                               const std::set<CardType> &allowedTypes) const {
  if (mode != "exact" && mode != "most" && mode != "least") {
    mode = "exact";
  }

  if (mode != "most") {
    numberOfCardsToChoose = std::min(numberOfCardsToChoose, selection.size());
  }

  std::vector<const Card *> chosenCards;
  std::vector<bool> taken(selection.size(), false);

  std::string allowedTypesStr;
  for (const CardType &t : allowedTypes) {
    if (!allowedTypesStr.empty()) {
      allowedTypesStr += ", ";
    }
    switch (t) {
    case CardType::ACTION:
      allowedTypesStr += "Action";
      break;
    case CardType::TREASURE:
      allowedTypesStr += "Treasure";
      break;
    case CardType::VICTORY:
      allowedTypesStr += "Victory";
      break;
    case CardType::CURSE:
      allowedTypesStr += "Curse";
      break;
    case CardType::ATTACK:
      allowedTypesStr += "Attack";
      break;
    case CardType::REACTION:
      allowedTypesStr += "Reaction";
      break;
    }
  }

  if (mode == "exact") {
    std::cout << "Choose exactly " << numberOfCardsToChoose << " card(s) to "
              << action << ":\n";
  } else if (mode == "most") {
    std::cout << "Choose at most " << numberOfCardsToChoose
              << " card(s) (0 to finish) to " << action << ":\n";
  } else {
    std::cout << "Choose at least " << numberOfCardsToChoose
              << " card(s) (0 to finish once done) to " << action << ":\n";
  }

  displaySelection(selection);

  while (true) {
    const bool allTaken = chosenCards.size() == selection.size();
    const bool reachedN = chosenCards.size() >= numberOfCardsToChoose;

    // Arrêt automatique
    if (allTaken || (mode != "least" && reachedN))
      break;

    const bool canStop = (mode == "most") || (mode == "least" && reachedN);

    std::cout << "[" << chosenCards.size() << " chosen] Enter a card number"
              << (canStop ? " (0 to finish)" : "") << ": ";

    int index;
    if (!(std::cin >> index)) {
      if (std::cin.eof()) {
        break;
      }
      std::cin.clear();
      std::cout << "Invalid input. Please try again.\n";
      continue;
    }

    if (index == 0 && canStop)
      break;

    if (index < 1 || static_cast<size_t>(index) > selection.size()) {
      std::cout << "Invalid index. Please try again.\n";
      continue;
    }
    if (taken[index - 1]) {
      taken[index - 1] = false;
      auto chosen = std::find(chosenCards.begin(), chosenCards.end(),
                              selection[index - 1]);
      if (chosen != chosenCards.end()) {
        chosenCards.erase(chosen);
      }
      std::cout << "Card deselected.\n";
      continue;
    }

    if (selection[index - 1]->getTypes().empty() ||
        std::none_of(allowedTypes.begin(), allowedTypes.end(), [&](CardType t) {
          return selection[index - 1]->isA(t);
        })) {
      std::cout << "Invalid card type. Please try again.\n";
      continue;
    }

    taken[index - 1] = true;
    chosenCards.push_back(selection[index - 1]);
  }

  return chosenCards;
}

void Game::ActionPhase() {
  while (currentPlayerActions > 0) {
    std::cout << "You have " << currentPlayerActions << " actions left."
              << std::endl;
    std::vector<const Card *> cards = activePlayer->getHand().getCards();
    for (const Card *c : cards) {
      if (!c->isA(CardType::ACTION)) {
        cards.erase(std::remove(cards.begin(), cards.end(), c), cards.end());
      }
    }
    if (cards.empty()) {
      std::cout << "You have no action cards to play." << std::endl;
      break;
    }
    auto card = chooseCardsFromSelection(cards, 1, "play", "exact",
                                         {CardType::ACTION})[0];
    if (!card) {
      std::cout << "No card selected. Please try again." << std::endl;
      continue;
    }
    std::cout << "You played the " << card->getName() << std::endl;
    activePlayer->playCard(card, *this);
    currentPlayerActions--;
  }
  useTreasureCards();
  std::cout << "Action phase ended." << std::endl;
}

void Game::useTreasureCards() {
  std::vector<const Card *> treasureCards =
      activePlayer->getHand().getSpecificCards(CardType::TREASURE);
  if (treasureCards.empty()) {
    std::cout << "You have no treasure cards to use." << std::endl;
    return;
  }

  std::cout << "You have the following treasure cards in your hand:"
            << std::endl;
  displaySelection(treasureCards);

  std::string input;

  while (!treasureCards.empty() && input != "all" && input != "none") {
    std::cout << "Enter the index of the treasure card you want to use, (all "
                 "and none accepted): ";

    std::cin >> input;
    std::cin.ignore();
    if (input == "all") {
      for (const Card *card : treasureCards) {
        activePlayer->playCard(card, *this);
        continue;
      }
    } else if (input == "none") {
      return;
    } else {
      int index = std::stoi(input);
      if (index < 1 || static_cast<size_t>(index) > treasureCards.size()) {
        std::cout << "Invalid index. Please try again." << std::endl;
        continue;
      } else {
        const Card *card = treasureCards[index - 1];
        activePlayer->playCard(card, *this);
      }
    }
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
      displaySelection(pair.second);
    }

    std::string card_name;

    std::cout << "Which card do you want to buy ? Enter the name of the card "
                 "or -1 to skip buying a card"
              << std::endl;
    std::cout << "You have " << currentPlayerMoney << " coins." << std::endl;
    std::cin >> card_name;

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
  displaySelection(activePlayer->getHand().getCards());
  std::cout << "It is " << player.getString() << "'s turn." << std::endl;
  ActionPhase();
  BuyPhase();
  player.discardHand();

  player.drawCards(5);
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
    if (player.getScore() > maxScore) {
      maxScore = player.getScore();
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
  if (choice < 1 || static_cast<size_t>(choice) > gameModes.size()) {
    std::cout << "Invalid choice." << std::endl;
    return;
  }
  setCurrentGameMode(&gameModes[choice - 1]);

  std::cout << "Will you play alone ? Enter the number of players (4 players "
               "max) [default : 1 player]"
            << std::endl;
  choice = 1;
  std::cin >> choice;
  if (choice < 1 || choice > 4) {
    std::cout << "Invalid choice." << std::endl;
  }
  std::cout << "You have chosen to play with " << choice << " players."
            << std::endl;

  auto usualDistribution = std::map<const Card *, int>();
  usualDistribution.insert({
      {Factory::get("Copper"), 60 - 7 * choice},
      {Factory::get("Curse"), 10 * (choice - 1)},
  });
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

  auto cardDistribution = gameModes[choice - 1].getCardDistribution();
  auto combinedDistribution = std::map<const Card *, int>(usualDistribution);
  combinedDistribution.insert(cardDistribution.begin(), cardDistribution.end());
  setCurrentCardDistribution(combinedDistribution);

  for (int i = 0; i < choice; ++i) {
    std::cout << "Enter the name of player " << i + 1 << ": ";
    std::string playerName;
    std::cin >> playerName;
    std::cin.ignore(); // Ignore the newline character
    std::cin.sync();   // Clear the input buffer
    CardStack drawStack = CardStack(
        {Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Estate"), Factory::get("Estate"),
         Factory::get("Estate")});

    drawStack.shuffle();
    Hand hand;
    CardStack discardStack = CardStack();
    hand.refillHandFromDrawStack(drawStack, discardStack);
    Player player(hand, drawStack, discardStack);
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
