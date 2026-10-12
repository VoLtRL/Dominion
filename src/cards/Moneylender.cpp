#include "../../include/cards/Moneylender.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Moneylender::onPlay(Game& g, Player& p) const {
    const auto cards = p.getHand().getCards();
    for (const Card *card : cards) {
        if (card->getName() == "Copper") {
            p.removeCardFromHand(card);
            g.trashCard(card);
            g.addGold(3);
            break;
        }
    }
}