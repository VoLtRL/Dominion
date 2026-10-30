#include "shared/gameplay/Game.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>
#include <random>

bool Game::initialize(const GameMode& gameMode,const std::vector<QString> &playerNames) {
  setCurrentCardDistribution(gameMode.getCardDistribution());
  for (const QString &name : playerNames) {
    addPlayer(Player(name.toStdString()));
  }

  std::cout << "Game initialized with " << players.size() << " players." << std::endl;

  auto seed = std::mt19937(std::random_device{}());

  std::shuffle(players.begin(), players.end(), seed);

  activePlayer = &players[0];
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

  for (Player &player : players) {
    CardStack drawStack = CardStack(
        {Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Copper"), Factory::get("Copper"), Factory::get("Copper"),
         Factory::get("Estate"), Factory::get("Estate"),
         Factory::get("Estate")});
    drawStack.shuffle();
    Hand hand;
    CardStack discardStack = CardStack();
    hand.refillHandFromDrawStack(drawStack, discardStack);
    player.setHand(hand);
    player.setDrawStack(drawStack);
    player.setDiscardStack(discardStack);
  }

  return true;
}

Player* Game::getPlayerByName(const std::string& name) {
  for (auto& player : players) {
    if (player.getString() == name) {
      return &player;
    }
  }
  return nullptr;
}

const std::vector<Player> &Game::getPlayers() const { return players; }

const Player &Game::getActivePlayer() const { return *activePlayer; }

void Game::addPlayer(Player player) { players.push_back(player); }

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
/*
void Game::ActionPhase() {
  while (currentPlayerActions > 0) {
    std::cout << "You have " << currentPlayerActions << " actions left."
              << std::endl;
    auto card = chooseCardsFromSelection(activePlayer->getHand().getCards(), 1,
                                         "play", "most", {CardType::ACTION});
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
    displaySelection(activePlayer->getHand().getCards());
  }
  useTreasureCards();
  std::cout << "Action phase ended." << std::endl;
}

void Game::useTreasureCards() {

  std::vector<const Card *> treasureCards = chooseCardsFromSelection(
      activePlayer->getHand().getCards(), activePlayer->getHand().getSize(),
      "play", "most", {CardType::TREASURE});
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
      displaySelection(pair.second);
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

/*
void Game::playTurn(Player &player) {
  setActivePlayer(&player);
  player.addTurnPlayed();
  displaySelection(activePlayer->getHand().getCards());
  std::cout << "It is " << activePlayer->getName() << "'s turn." << std::endl;
  ActionPhase();
  BuyPhase();
  discardCardsInPlay();
  player.discardHand();
  player.drawCardsToHand(5);
  hasGameEnded();
}
*/

void Game::addGold(int amount) { currentPlayerMoney += amount; }

void Game::discardCardsInPlay() {
  for (auto card : CardsInPlay) {
    activePlayer->addCardToDiscardStack(card);
  }
  CardsInPlay.clear();
}

Player Game::getWinner() const {
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
/*
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
    }
  }

  std::cout << "Game is over!" << std::endl;
  std::cout << "The winner is:" << getWinner().getString() << std::endl;
}
*/

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

/*
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
*/

std::vector<Player *> Game::getOtherPlayers() {
  std::vector<Player *> res;
  for (Player &player : players) {
    if (player != *activePlayer) {
      res.push_back(&player);
    }
  }
  return res;
}

/*
void Game::playAttackCard(const Card *card) {
  std::vector<Player *> others = getOtherPlayers();

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
*/

void Game::playAction(Player &p, const Card *card) {

}

void Game::playTreasure(Player&p, const Card* card){

}


bool Game::canPlayAction(Player* p, const Card* card) const {
  if(currentPlayerActions <= 0) { return false; }
  if(!p->hasCardInHand(card)) { return false; }
  if(p != activePlayer) { return false; }
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
  return true;
}

bool Game::canPlayTreasure(Player* p, const Card* card) const {
  if(!p->hasCardInHand(card)) { return false; }
  if(p != activePlayer) { return false; }
  if(!card->isA(CardType::TREASURE)) { return false; }
  const Treasure* treasure = dynamic_cast<const Treasure*>(card);
  const Treasure* secondTreasure = dynamic_cast<const Treasure*>(Factory::get(card->getName()));
  if(treasure->getCoins() != secondTreasure->getCoins()) { return false ;}  // Si d'une manière ou d'une autre les coins de la carte ont changés, jsp comment tu gères l'envoi des cartes
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
  if(p != activePlayer) {
    // Signal "erreur"
    return false;
  }
  for(const Card* card : p->getHand().getCards()){
    if(!canPlayTreasure(p, card)){
      return false;
    }
  }
  for(const Card* card : p->getHand().getCards()){
    tryPlayTreasure(p, card);
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
  if(card->getName() != pendingCard->getName()) { return false; }
  if(attacker != activePlayer) { return false; }
  auto i = std::find(currentTargets.begin(), currentTargets.end(), p);
  if(i != currentTargets.end()) {
    currentTargets.erase(i);
    return true;
  }
  return false;
}

bool Game::canBuyCard(Player* p, const Card* card) const {
  if(currentPlayerPurchaseLeft <= 0) { return false; }
  if(p != activePlayer) { return false; }
  if(!currentCardDistribution.at(card)) { return false ;}
  if(card->getCost() > currentPlayerMoney) { return false; }
  if(!Factory::get(card->getName())){ return false ;}
  return true;
}
bool Game::tryBuyCard(Player* p, const Card* card) {
  if(!canBuyCard(p, card)) {
    // Signal erreur
    return false;
  }
  currentCardDistribution.at(card) -= 1;
  currentPlayerMoney -= card->getCost();
  const Card* newCard = Factory::get(card->getName());
  p->addCardToDiscardStack(newCard);
  return true;
}

// Là pour le coup je sais pas ce que tu comptes faire ou en tout cas comment le faire
bool Game::tryNextPhase(Player* p) {
  if(p != activePlayer) {
    // Signal erreur
    return false;
  }
  switch(currentPhase) {
    case GamePhase::ACTION :
      
      break;

    case GamePhase::BUY :
      
      break;

    case GamePhase::REACTION :
      
      break;
    
    case GamePhase::SELECTION :
      
      break;
  }
  return false;
}

// Pareil jsp, c'est dans le cadre d'une demande de sélection, d'une réponse ?
bool Game::trySendSelection(Player* p, const std::vector<const Card*>& selection) {
  return false;
}

bool Game::hasGameEnded() const {
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
      // Message de fin
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
    return false;
  }
}

void Game::passToNextPlayer() {
  auto index = std::find(players.begin(), players.end(), *activePlayer);
  if(index == players.end()){
    // Pas normal
    return;
  }
  index += 1;
  if(index == players.end()){
    index = players.begin();
  }
  activePlayer = &*index;
}