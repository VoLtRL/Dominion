#include "shared/cards/Militia.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

#include <algorithm>
#include <iterator>

void Militia::play(Game& g, Player& p) const {
    std::cout << "You gain 2 coins this turn!" << std::endl;
    g.addGold(2);
}

void Militia::attack(Game& g, Player& p, std::set<Player*> targets) const{
  for(auto it = targets.begin(); it != targets.end(); ++it){
      Player* player = *it;
      const int nb = std::max(0, static_cast<int>(player->getHand().getSize()) - 3);
      if(nb > 0){
          std::vector<Player*> remaining;
          for(auto next = std::next(it); next != targets.end(); ++next) remaining.push_back(*next);
          g.setPendingPlayers(remaining);
          g.requestSelection(player, player->getHand().getCards(), nb,
                             SelectionMode::DISCARD, SelectionConstraint::EXACTLY,
                             Game::SelectionContext::MILITIA_DISCARD);
          return;
      }
  }
}
