#include "shared/gameplay/Game.hpp"
#include "shared/cards/Chancellor.hpp"
#include "shared/player/Player.hpp"

void Chancellor::play(Game &g, Player &p) const
{
    std::cout << "You gain 2 coins for this turn and you may immediately discard your entire draw pile." << std::endl;
    g.addGold(2);
    std::vector<const Card*> cards;
    auto drawStack = p.getDrawStack().getStack();
    while (!drawStack.empty()) {
        cards.push_back(drawStack.top());
        drawStack.pop();
    }
    g.requestSelection(&p, cards, cards.size(), SelectionMode::DISCARD,
                       SelectionConstraint::AT_MOST,
                       Game::SelectionContext::CHANCELLOR_DISCARD);
}
