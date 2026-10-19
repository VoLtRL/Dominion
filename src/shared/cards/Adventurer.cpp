#include "shared/gameplay/Game.hpp"
#include "shared/cards/Adventurer.hpp"
#include "shared/player/Player.hpp"

void Adventurer::play(Game &g, Player &p) const {
    int treasures = 0;
    while (treasures < 2 && p.getDiscardStack().getSize() + p.getDrawStack().getSize() > 0) {
        auto drawnCard = p.getDrawStack().getStack().top();
        drawnCard->display();
        if (drawnCard->isA(CardType::TREASURE)) {
            p.drawCardToHand();
            treasures++;
        } else {
            g.addRevealedCard(p.drawCard());
        }
    }
    for (const Card* card : g.getRevealedCards()) {
        p.addCardToDiscardStack(card);
    }
    g.clearRevealedCards();
}