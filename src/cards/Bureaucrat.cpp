#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Bureaucrat.hpp"
#include "../../include/player/Player.hpp"

void Bureaucrat::onPlay(Game &g, Player &p) const
{
    (void)g;
    p.addCardToDiscardStack(Factory::get("Silver"));
}
