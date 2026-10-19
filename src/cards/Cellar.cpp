#include "../../include/cards/Cellar.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Cellar::play(Game &g, Player &p) const {
  std::cout << "You get one additional action and you may discard any number of cards from your hand and draw one for every card discarded this way." << std::endl;
  g.addPlayerActions(1);

  Hand hand = p.getHand();
  std::vector<const Card*> selection = g.chooseCardsFromSelection(hand.getCards(), hand.getSize(), "discard", "most", {CardType::ALL});
  if(!selection.empty()){
    for(const Card* c : selection){
      p.discardCard(c);
    }
    std::cout << "Drawing " << selection.size() << " cards." << std::endl;
    p.drawCardsToHand(selection.size());
  }

}
