#include "shared/cards/Library.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Library::play(Game&, Player& p) const {
    std::cout << "You draw cards until your hand holds at least 7, and you may skip any action card drawn this way, discarding them afterwards." << std::endl;
    std::vector<const Card*> actionCardsDrawn;
    while(p.getHand().getSize() < 7){
        const Card* c = p.drawCard();
        bool skipped = false;
        std::cout << "You drew this card : " << std::endl;
        c->display();
        if(c->isA(CardType::ACTION)){

      std::string input;

      while (true) {
        std::cout << "Do you want to skip this card ? (Y/N)" << std::endl;
        std::cin >> input;
        std::cin.ignore();
        if (input == "Y" || input == "y") {
          std::cout << "Skipping this card." << std::endl;
          actionCardsDrawn.push_back(c);
          skipped = true;
          break;
        }
        if (input == "N" || input == "n") {
          std::cout << "Card added to your hand." << std::endl;
          break;
        }
        std::cout << "Please enter a correct response (Y/N)" << std::endl;
      }
    }
    if (!skipped) {
      p.addCardToHand(c);
    }
  }
  if (!actionCardsDrawn.empty()) {
    std::cout << "The other cards will be discarded." << std::endl;
    for (const Card *c : actionCardsDrawn) {
      p.addCardToDiscardStack(c);
    }
  }
}
