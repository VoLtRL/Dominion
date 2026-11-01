#include "shared/cards/Library.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Library::play(Game& g, Player& p) const {
    std::cout << "You draw cards until your hand holds at least 7, and you may skip any action card drawn this way, discarding them afterwards." << std::endl;
    while(p.getHand().getSize() < 7){
        const Card* c = p.drawCard();
        if (!c) break;
        std::cout << "You drew this card : " << std::endl;
        c->display();
        if(c->isA(CardType::ACTION)){
            g.requestSelection(&p, {c}, 1, SelectionMode::DISCARD,
                               SelectionConstraint::AT_MOST,
                               Game::SelectionContext::LIBRARY_ACTION);
            return;
        }
        p.addCardToHand(c);
    }
    for (const Card *c : g.getPendingLibrarySkipped()) {
        p.addCardToDiscardStack(c);
    }
    g.clearPendingLibrarySkipped();
}
