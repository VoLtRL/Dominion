#include "shared/cards/Feast.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Feast::play(Game& g, Player& p) const {

    std::cout << "This card is trashed." << std::endl;
    p.removeCardFromHand(this);
    g.trashCard(this);

    std::cout << "You now gain a card costing up to 5 coins!" << std::endl;
    const std::vector<const Card*> buyable = g.getBuyableCardsBelowCost(5);
    if (!buyable.empty()) {
        g.requestSelection(&p, buyable, 1, SelectionMode::GAIN,
                           SelectionConstraint::EXACTLY,
                           Game::SelectionContext::FEAST_GAIN);
    }
}