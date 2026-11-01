#include "shared/cards/Spy.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Spy::play(Game& g, Player& p) const {
    std::cout << "You draw 1 card and get 1 action this turn!" << std::endl;
    p.drawCardsToHand(1);
    g.addPlayerActions(1);


}

void Spy::attack(Game& g, Player& p, std::set<Player*> targets) const {
    std::cout << "Everyone (including you) reveals the top card of their draw pile and YOU choose if it goes in the draw pile or the discard pile." << std::endl;
    
    if(targets.find(&p) == targets.end()){
        targets.insert(&p);
    }

  if (targets.empty()) return;
  Player *player = *targets.begin();
  targets.erase(targets.begin());
  const Card *card = player->drawCard();
  if (!card) {
    attack(g, p, targets);
    return;
  }
  g.setPendingPlayers(std::vector<Player*>(targets.begin(), targets.end()));
  g.setPendingSelectionTarget(player);
  g.requestSelection(&p, {card}, 1, SelectionMode::DISCARD,
                     SelectionConstraint::AT_MOST,
                     Game::SelectionContext::SPY_CARD);
}
