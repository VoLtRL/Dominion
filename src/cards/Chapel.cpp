#include "../../include/cards/Chapel.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"
#include <algorithm>

void Chapel::onPlay(Game& g, Player& p) const {
    const size_t cardsToTrash = std::min<size_t>(4, p.getHand().getSize());
    for (size_t i = 0; i < cardsToTrash; ++i) {
        const Card *card = p.getHand().getCards().front();
        p.removeCardFromHand(card);
        g.trashCard(card);
    }
}