#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Adventurer.hpp"
#include "../../include/player/Player.hpp"

void Adventurer::onPlay(Game &g, Player &p) const {
    (void)g;
    int treasures = 0;
    while (treasures < 2 && p.getDrawStack().getSize() > 0) {
        p.drawCard();
        const auto &cards = p.getHand().getCards();
        const Card *revealed = cards.back();
        if (revealed->isA(CardType::TREASURE)) {
            ++treasures;
        } else {
            p.discardCard(revealed);
        }
    }
}