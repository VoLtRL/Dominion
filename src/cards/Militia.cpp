#include "../../include/cards/Militia.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

#include <algorithm>

void Militia::play(Game& g, Player& p) const {
    std::cout << "You gain 2 coins this turn!" << std::endl;
    g.addGold(2);
}

void Militia::attack(Game& g, Player& p, std::set<Player*> targets) const{
  for(Player* player : targets){
      std::cout << "Player " << player->getName() << ", discard until you hold 3 cards!" << std::endl;
      Hand h = player->getHand();
      int nb = std::max(0,static_cast<int>(h.getSize())-3);
      if(nb > 0){
          SelectionParameters params = InputManager::createSelectionParameters(nb, "Discard until you have only 3 left", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::ALL},  h.getCards());
          std::vector<const Card *> cardsToDiscard = InputManager::chooseCardsFromSelection(*player, g.getCurrentPhase(), params);
          std::cout << "Discarding cards." << std::endl;
          for(const Card* c : cardsToDiscard){
              player->discardCard(c);
          }
      }
      else{
          std::cout << "You already have at most 3 cards!" << std::endl;
      }
  }
}
