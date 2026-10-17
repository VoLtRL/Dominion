#include "../../include/cards/Spy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

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

  for (Player *player : targets) {
    std::cout << "Player " << player->getName()
              << ", here is your card : " << std::endl;
    const Card *c = player->drawCard();
    c->display();

    std::string input;
    while (true) {
      std::cout << "Player " << p.getName()
                << ", do you want to send it to the draw pile or the discard "
                   "pile ? (draw/discard)"
                << std::endl;
      std::cin >> input;
      std::cin.ignore();
      if (input == "discard" || input == "Discard") {
        std::cout << "Sent to the discard pile!" << std::endl;
        player->addCardToDiscardStack(c);
        break;
      }
      if (input == "draw" || input == "Draw") {
        std::cout << "Sent to the draw pile!" << std::endl;
        player->addCardToDrawStack(c);
        break;
      }
      std::cout << "Please enter a valid response (draw/discard)" << std::endl;
    }
  }
}
