#include "shared/gameplay/Game.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>
#include <random>

bool Game::initialize(const GameMode& gameMode,const std::vector<QString> &playerNames) {
  setCurrentCardDistribution(gameMode.getCardDistribution());
  for (const QString &name : playerNames) {
    addPlayer(std::make_unique<Player>(name.toStdString()));
  }

  std::cout << "Game initialized with " << players.size() << " players." << std::endl;

  auto seed = std::mt19937(std::random_device{}());

  std::shuffle(players.begin(), players.end(), seed);

  activePlayer = players[0].get();
  currentPhase = GamePhase::ACTION;

  currentPlayerMoney = 0;
  currentPlayerActions = 1;
  currentPlayerPurchaseLeft = 1;

  CardsInPlay.clear();
  revealedCards.clear();
  trash.clear();

  auto usualDistribution = std::map<const Card *, int>();
    usualDistribution.insert({{Factory::get("Copper"), 60 - 7 * players.size()},
                            {Factory::get("Curse"), 10 * (players.size() - 1)},
                            {Factory::get("Silver"), 40},
                            {Factory::get("Gold"), 30}});
  switch (players.size()) {
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

  auto combinedDistribution = std::map<const Card *, int>(usualDistribution);

  combinedDistribution.insert(currentCardDistribution.begin(), currentCardDistribution.end());

  setCurrentCardDistribution(combinedDistribution);

  for (const std::unique_ptr<Player>& player : players) {
    CardStack drawStack = CardStack(
      {Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
        Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
        Factory::get("Estate"), Factory::get("Estate"),
        Factory::get("Estate")});
    drawStack.shuffle();
    Hand hand;
    CardStack discardStack = CardStack();
    hand.refillHandFromDrawStack(drawStack, discardStack);
    player->setHand(hand);
    player->setDrawStack(drawStack);
    player->setDiscardStack(discardStack);
  }

  return true;
}

Player* Game::getPlayerByName(const std::string& name) {
  for (const auto& player : players) {
    if (player->getString() == name) {
      return player.get();
    }
  }
  return nullptr;
}


const std::vector<std::unique_ptr<Player>>& Game::getPlayers() const { return players; }

Player* Game::getActivePlayer() const { return activePlayer; }

void Game::addPlayer(std::unique_ptr<Player> player) { players.push_back(std::move(player)); }

void Game::setActivePlayer(Player *player) { activePlayer = player; }

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
      if (selection[i] == nullptr) {
        continue;
      }
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

  std::vector<const Card *> validSelection;
  if (allowedTypes.find(CardType::ALL) != allowedTypes.end()) {
    validSelection = selection;
  } else {
    for (const Card *card : selection) {
      std::set<CardType> types = card->getTypes();
      for (CardType t : types) {
        if (allowedTypes.find(t) != allowedTypes.end()) {
          validSelection.push_back(card);
        }
      }
    }
  }

  std::cout << HUDFormat();

  if (validSelection.empty()) {
    std::cout << "There are no valid cards in this selection." << std::endl;
    return {};
  }

  size_t allowedCount = std::min(numberOfCardsToChoose, validSelection.size());

  if ((mode == "exact" && allowedCount == validSelection.size()) ||
      (mode == "least" && allowedCount >= validSelection.size())) {
    std::cout << "You need to choose all cards" << std::endl;
    return validSelection;
  }

  if (mode == "exact") {
    std::cout << "Choose exactly " << allowedCount;
  } else if (mode == "most") {
    std::cout << "Choose at most " << allowedCount;
  } else {
    std::cout << "Choose at least " << allowedCount;
  }
  std::cout << " card(s) to " << action;

  std::cout << (mode == "exact" ? ":\n" : " (0 to finish):\n");

  displaySelection(validSelection);

  // Indices into `selection`, in order of choice
  std::vector<size_t> chosen;

  while (true) {
    const bool reachedN = chosen.size() >= allowedCount;

    // Automatic stop
    if (chosen.size() == allowedCount || (mode != "least" && reachedN)) {
      break;
    }

    const bool canStop = (mode == "most") || (mode == "least" && reachedN);

    std::cout << "[" << chosen.size() << " chosen] Enter a card number"
              << (canStop ? " (0 to finish)" : "") << ": ";

    std::string input = "";
    try {
      if (!std::getline(std::cin, input)) {
        input = -1;
        break; // input stream closed
      }
    } catch (const std::exception &) {
      input = -1;
    }

    if (allowedTypes.find(CardType::TREASURE) != allowedTypes.end()) {
      if (input == "all") {
        if (validSelection.size() > allowedCount && mode != "least") {
          std::cout << "Too many cards!" << std::endl;
          continue;
        }
        chosen.clear();
        for (size_t i = 0; i < validSelection.size(); i++) {
          chosen.push_back(i + 1);
        }
        continue;
      }
      if (input == "none") {
        if (mode != "most" && allowedCount > 0) {
          std::cout << "Too few cards chosen!" << std::endl;
          continue;
        }
      }
    }

    int index = -1;
    try {
      size_t pos = 0;
      index = std::stoi(input, &pos);
      if (pos != input.size()) {
        input = -1; // reject things like "12abc"
      }
    } catch (const std::exception &) {
      std::cout << "Invalid index." << std::endl;
      continue;
    }

    if (index == 0 && canStop) {
      break;
    }

    if (index < 1 || static_cast<size_t>(index) > validSelection.size()) {
      std::cout << "Invalid index. Please try again.\n";
      continue;
    }

    auto posi = std::find(chosen.begin(), chosen.end(), index);
    if (posi != chosen.end()) {
      std::cout << "Deselecting card number " << index << std::endl;
      chosen.erase(posi);
    } else {
      std::cout << "Selecting card number " << index << std::endl;
      chosen.push_back(index);
    }
  }

  if (chosen.empty()) {
    std::cout << "There are no chosen cards to " << action << std::endl;
    return {};
  }
  std::vector<const Card *> res;
  for (auto index : chosen) {
    res.push_back(validSelection[index - 1]);
  }
  return res;
}

void Game::clearRevealedCards() { revealedCards.clear(); }

void Game::addGold(int amount) { currentPlayerMoney += amount; }

void Game::discardCardsInPlay() {
  for (const auto& player : players) {
    for (const Card *card : player->getCardsInPlay()) {
      player->addCardToDiscardStack(card);
    }
    player->clearCardsInPlay();
  }

  CardsInPlay.clear();
}

Player* Game::getWinner() const {
  Player* winner = players[0].get();
  int maxScore = std::numeric_limits<int>::min();

  for (const auto& player : players) {
    if (player->getVictoryPoints() > maxScore) {
      maxScore = player->getVictoryPoints();
      winner = player.get();
    }
  }

  return winner;
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
  for (auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if (card->getName() == cardName) {
      if (quantity > 0) {
        pair.second--;
        return card;
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

std::vector<Player *> Game::getOtherPlayers() {
  std::vector<Player *> res;
  for (const auto& player : players) {
    if (player.get() != activePlayer) {
      res.push_back(player.get());
    }
  }
  return res;
}

bool Game::canExecuteTurnAction(Player *p) const {
  if (p == nullptr || p != activePlayer) {
    return false;
  }

  if (currentPhase != GamePhase::ACTION && currentPhase != GamePhase::BUY) {
    return false;
  }

  return !hasPendingSelection() && !waitingForPlayers();
}

bool Game::canPlayAction(Player* p, const Card* card) const {
  if (!canExecuteTurnAction(p)) { return false; }
  if(currentPlayerActions <= 0) { return false; }
  if(currentPhase != GamePhase::ACTION) { return false; }
  if(!p->hasCardInHand(card)) { return false; }
  if(!card->isA(CardType::ACTION)) { return false; }
  return true;
}

bool Game::tryPlayAction(Player* p, const Card* card) {
  if(!canPlayAction(p, card)){
    // Signal "erreur"
    return false;
  }

  // Signal "ok"
  p->removeCardFromHand(card);
  p->addCardToPlay(card);
  addCardToPlay(card);
  const Action* action = dynamic_cast<const Action*>(card);
  action->play(*this, *p);
  currentPlayerActions--;
  return true;
}

bool Game::canPlayTreasure(Player* p, const Card* card) const {
  if (!canExecuteTurnAction(p)) { return false; }
  if(currentPhase != GamePhase::BUY && currentPhase != GamePhase::ACTION) { return false; }
  if(!p->hasCardInHand(card)) { return false; }
  if(!card->isA(CardType::TREASURE)) { return false; }
  return true;
}
bool Game::tryPlayTreasure(Player* p, const Card* card) {
  if(!canPlayTreasure(p, card)){
    // Signal "erreur"
    return false;
  }
  // Signal "ok"
  p->removeCardFromHand(card);
  p->addCardToPlay(card);
  addCardToPlay(card);
  const Treasure* treasure = dynamic_cast<const Treasure*>(card);
  addGold(treasure->getCoins());
  return true;
}
bool Game::tryPlayAllTreasures(Player* p) {
  if(!canExecuteTurnAction(p)) {
    return false;
  }

  std::vector<const Card*> treasures;
  for (const Card* card : p->getHand().getCards()) {
    if (card->isA(CardType::TREASURE)) {
      if (!canPlayTreasure(p, card)) {
        return false;
      }
      treasures.push_back(card);
    }
  }

  if (treasures.empty()) {
    return false;
  }

  for (const Card* card : treasures) {
    if (!tryPlayTreasure(p, card)) {
      return false;
    }
  }

  return true;
}

bool Game::canPlayAttack(Player* p, const Card* card) const {
  if(std::find(CardsInPlay.begin(), CardsInPlay.end(), card) == CardsInPlay.end()) { return false; } // Si la carte a pas été jouée juste avant
  if(p != activePlayer) { return false; }
  if(!card->isA(CardType::ATTACK)) { return false; }

  return true;
}

bool Game::tryPlayAttack(Player* p, const Card* card) {
  if(!canPlayAttack(p, card)) { return false; }
  const Attack* at = dynamic_cast<const Attack*>(card);
  at->attack(*this, *p, currentTargets);
  return true;
}

bool Game::canPlayReaction(Player* p, const Card* card, EventType t, Player* actor) {
  if(!isPlayerPending(p)) { return false;}
  if(!card->isA(CardType::REACTION)) { return false; }
  if(!p->hasCardInHand(card)) { return false; }

  const Reaction* reac = dynamic_cast<const Reaction*>(card);
  if(!reac->canReact(t, p)) { return false; }
  return true;
}

bool Game::tryPlayReaction(Player* p, const Card* card, EventType t, Player* actor) {
  if(!canPlayReaction(p, card, t, actor)) { return false ;}
  const Reaction* reac = dynamic_cast<const Reaction*>(card);
  reac->react(t, *this, p, actor);
  removePendingPlayers(p);
  return true;
}

bool Game::applyProtection(Player* p, Player* attacker, const Card* card) {
  if(attacker != activePlayer) { return false; }
  auto i = std::find(currentTargets.begin(), currentTargets.end(), p);
  if(i != currentTargets.end()) {
    currentTargets.erase(i);
    return true;
  }
  return false;
}

bool Game::canBuyCard(Player* p, const Card* card) const {
  if (p == nullptr || card == nullptr) { return false; }
  if (!canExecuteTurnAction(p)) { return false; }
  if(currentPlayerPurchaseLeft <= 0) { return false; }
  auto stack = currentCardDistribution.find(card);
  if(stack == currentCardDistribution.end() || stack->second <= 0) { return false; }
  if(card->getCost() > currentPlayerMoney) { return false; }
  if(!Factory::get(card->getName())){ return false ;}
  return true;
}
bool Game::tryBuyCard(Player* p, const Card* card) {
  if(!canBuyCard(p, card)) {
    // Signal erreur
    return false;
  }
  auto stack = currentCardDistribution.find(card);
  if (stack == currentCardDistribution.end()) { return false; }
  stack->second -= 1;
  currentPlayerMoney -= card->getCost();
  currentPlayerPurchaseLeft--;
  const Card* newCard = Factory::get(card->getName());
  p->addCardToDiscardStack(newCard);
  return true;
}

// Là pour le coup je sais pas ce que tu comptes faire ou en tout cas comment le faire
bool Game::tryNextPhase(Player* p) {
  if(!canExecuteTurnAction(p)) {
    return false;
  }
  switch(currentPhase) {
    case GamePhase::ACTION :
      currentPhase = GamePhase::BUY;
      return true;
    default:
      return false;
  }
}

// Pareil jsp, c'est dans le cadre d'une demande de sélection, d'une réponse ?
bool Game::trySendSelection(Player* p, const std::vector<const Card*>& selection) {
  if (!p || p != selectionPlayer) {
    return false;
  }

  const size_t count = selection.size();
  if ((selectionConstraint == SelectionConstraint::EXACTLY && count != selectionCount) ||
      (selectionConstraint == SelectionConstraint::AT_LEAST && count < selectionCount) ||
      (selectionConstraint == SelectionConstraint::AT_MOST && count > selectionCount)) {
    return false;
  }

  std::map<const Card*, size_t> availableCards;
  for (const Card *card : selectionCards) {
    availableCards[card] += 1;
  }
  std::map<const Card*, size_t> selectedCards;
  for (const Card *card : selection) {
    if (!card || availableCards.count(card) == 0 || availableCards[card] == 0) {
      return false;
    }
    selectedCards[card] += 1;
    availableCards[card] -= 1;
  }

  return true;
}

void Game::requestSelection(Player *p, const std::vector<const Card*> &cards,
                             size_t count, SelectionMode mode,
                             SelectionConstraint constraint,
                             SelectionContext context, int limitCost) {
  selectionPlayer = p;
  selectionCards = cards;
  selectionCount = count;
  selectionMode = mode;
  selectionConstraint = constraint;
  selectionContext = context;
  selectionLimitCost = limitCost;
}

void Game::clearSelection() {
  selectionPlayer = nullptr;
  selectionCards.clear();
  selectionCount = 0;
  selectionMode = SelectionMode::NONE;
  selectionConstraint = SelectionConstraint::NONE;
  selectionContext = SelectionContext::NONE;
  selectionLimitCost = 0;
}

bool Game::hasGameEnded() const {
  int emptyStacks = 0;
  for (const auto &pair : currentCardDistribution) {
    if (pair.second <= 0) {
      emptyStacks++;
    }
    if (pair.first->getName() == "Province" && pair.second <= 0) {
      return true;
    }
  }

  if (emptyStacks >= 3) {
    return true;
  }
  return false;
}

bool Game::performCleanup() {

  discardCardsInPlay();
  activePlayer->discardHand();
  activePlayer->drawCardsToHand(5);

  if(hasGameEnded()){
    //fin du jeu
    return true;
  }
  else {

    passToNextPlayer();
    resetCurrentPlayerStats();

    return false;
  }
}

void Game::resetCurrentPlayerStats() {
  currentPlayerActions = 1;
  currentPlayerPurchaseLeft = 1;
  currentPlayerMoney = 0;
  currentPhase = GamePhase::ACTION;
}

void Game::passToNextPlayer() {
  auto it = std::find_if(players.begin(), players.end(),
    [this](const std::unique_ptr<Player>& p) {
        return p.get() == activePlayer;
    });
  if(it == players.end()){
    // Pas normal
    return;
  }
  ++it;
  if(it == players.end()){
    it = players.begin();
  }
  activePlayer = it->get();
}
