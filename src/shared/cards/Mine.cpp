#include "shared/cards/Mine.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Mine::play(Game& g, Player& p) const {
    std::vector<const Card*> treasures;
    for (const Card *card : p.getHand().getCards()) {
        if (card->isA(CardType::TREASURE)) {
            treasures.push_back(card);
        }
    }
    if(treasures.empty()) {
        return;
    }
    g.requestSelection(&p, treasures, 1,
                       SelectionMode::TRASH, SelectionConstraint::EXACTLY,
                       Game::SelectionContext::MINE_TRASH);
}
