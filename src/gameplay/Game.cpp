#include "../../include/gameplay/Game.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <limits>

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
                               const std::string &action, std::string mode, const std::set<CardType> &allowedTypes) const {
  if (mode != "exact" && mode != "most" && mode != "least") {
    mode = "exact";
  }
  
  std::vector<const Card*> validSelection;
  if(allowedTypes.find(CardType::ALL) != allowedTypes.end()){
    validSelection = selection;
  }
  else{
    for(const Card* card : selection){
      std::set<CardType> types = card->getTypes();
      for(CardType t : types){
        if(allowedTypes.find(t) != allowedTypes.end()){
          validSelection.push_back(card);
        }
      }
    }
  }

  std::cout << HUDFormat();

  if(validSelection.empty()){
    std::cout << "There are no valid cards in this selection." << std::endl;
    return {};
  }

  size_t allowedCount = std::min(numberOfCardsToChoose, validSelection.size());

  if((mode == "exact" && allowedCount==validSelection.size()) || (mode == "least" && allowedCount >= validSelection.size())){
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

    if (allowedTypes.find(CardType::TREASURE) != allowedTypes.end()){
      if (input == "all"){
        if (validSelection.size() > allowedCount && mode!="least"){
          std::cout << "Too many cards!" << std::endl;
          continue;
        }
        for(size_t i = 0; i < validSelection.size() ; i++){
          chosen[i] = i+1;
        }
        continue;
      }
      if (input == "none"){
        if (mode != "most" && allowedCount>0){
          std::cout << "Too few cards chosen!" << std::endl;
          continue;
        }
      }
    }

    int index = -1;
    try{
      size_t pos = 0;
      index = std::stoi(input, &pos);
      if (pos != input.size()) {
        input = -1; // reject things like "12abc"
      }
    } catch(const std::exception &){
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
    if(posi != chosen.end()){
      std::cout << "Deselecting card number " << index << std::endl;
      chosen.erase(posi);
    }
    else{
      std::cout << "Selecting card number " << index << std::endl;
      chosen.push_back(index);
    }
  }

  if(chosen.empty()){
    std::cout << "There are no chosen cards to " << action << std::endl;
    return {};
  }
  std::vector<const Card*> res;
  for(auto index : chosen){
    res.push_back(validSelection[index-1]);
  }
  return res;
}

void Game::clearRevealedCards() { revealedCards.clear(); }

void Game::ActionPhase() {
  while (currentPlayerActions > 0) {
    std::cout << "You have " << currentPlayerActions << " actions left."
              << std::endl;
    auto card = chooseCardsFromSelection(activePlayer->getHand().getCards(), 1, "play", "exact", {CardType::ACTION});
    if(card.empty()){
      break;
    }
    std::cout << "You played the " << card[0]->getName() << std::endl;
    playCard(*activePlayer, card[0]);
    if(card[0]->isA(CardType::ATTACK)){
      playAttackCard(card[0]);
    }
    currentPlayerActions--;
  }
  useTreasureCards();
  std::cout << "Action phase ended." << std::endl;
}

void Game::useTreasureCards() {

  std::vector<const Card *> treasureCards = chooseCardsFromSelection(activePlayer->getHand().getCards(), activePlayer->getHand().getSize(), "play", "most", {CardType::TREASURE});
  for (const Card *card : treasureCards) {
    playCard(*activePlayer, card);
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
  std::cout << "It is " << activePlayer->getName() << "'s turn." << std::endl;
  ActionPhase();
  BuyPhase();
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

const std::vector<TableStack> &Game::getKingdomStacks(){
  return KingdomStacks;
}

const std::vector<TableStack> &Game::getBasicStacks(){
  return BasicStacks;
}

std::vector<const Card*> Game::getBuyableCardsBelowCost(int cost) const{
  std::vector<const Card*> validCards;
  for (const auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if(card->getCost() <= cost && quantity > 0){
      validCards.push_back(card);
    }
  }
  return validCards;
}


const Card* Game::getCardFromCardDistrib(std::string cardName){
  for (const auto &pair : currentCardDistribution) {
    const Card *card = pair.first;
    int quantity = pair.second;
    if(card->getName() == cardName){
      if (quantity>0){
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
    if(card->getName() == cardName){return quantity==0;}
  }
  return true;
}

std::string Game::HUDFormat() const {
  std::string hud = "[" + activePlayer->getString() + std::to_string(currentPlayerMoney) + " coins, " + std::to_string(currentPlayerActions) + " actions, " + std::to_string(currentPlayerPurchaseLeft) + " purchases left] ";
  return hud;
}

void Game::activateReactions(GameEvent* event){
  for (Player player : players){
    auto possibleReactions = player.getAvailableReactions();
    
    for (const Reaction* reactionCard : possibleReactions){
      if ( reactionCard->canReact(event, this, &player)) {
        reactionCard->react(event,this,&player);
        }
    }
  }
}

std::vector<Player*> Game::getOtherPlayers(Player& p) const{
  std::vector<Player*> res;
  for(Player player : players){
    if(p != player){
      res.push_back(&player);
    }
  }
  return res;
}

void Game::playAttackCard(const Card* card){
  std::vector<Player*> others = getOtherPlayers(*activePlayer);
  AttackEvent event(card, *activePlayer, std::set(others.begin(), others.end()));
  activateReactions(&event);

  std::set<Player*> newTargets;

  for(Player* p : event.getTargets()){
    if(!event.isIgnoredFor(*p)){
      newTargets.insert(p);
    }
  }

  card->onAttack(*this,*activePlayer, newTargets);

}

void Game::playCard(Player& p, const Card* card){
  p.removeCardFromHand(card);
  addCardToPlay(card);
  card->onPlay(*this, p);
}