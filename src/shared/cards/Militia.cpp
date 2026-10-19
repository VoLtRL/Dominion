#include "shared/cards/Militia.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

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
          std::vector<const Card*> cardsToDiscard = g.chooseCardsFromSelection(h.getCards(), nb, "discard", "exact", {CardType::ALL});
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
