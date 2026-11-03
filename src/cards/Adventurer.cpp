#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Adventurer.hpp"
#include "../../include/player/Player.hpp"

void Adventurer::play(Game &g, Player &p) const {
    int treasures = 0;
    while (treasures < 2 && p.getDiscardStack().getSize() + p.getDrawStack().getSize() > 0) {
        const Card* drawnCard = p.drawCard();
        if (!drawnCard) { break; }
        drawnCard->display();
        if (drawnCard->isA(CardType::TREASURE)) {
            p.addCardToHand(drawnCard);
            treasures++;
        } else {
            g.addRevealedCard(drawnCard);
        }
    }
    for (const Card* card : g.getRevealedCards()) {
        p.addCardToDiscardStack(card);
    }
    g.clearRevealedCards();
}