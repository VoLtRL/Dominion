#include "shared/gameplay/Game.hpp"
#include "shared/cards/Bureaucrat.hpp"
#include "shared/player/Player.hpp"
#include <iterator>

void Bureaucrat::play(Game &g, Player &p) const
{
    std::cout << "A silver is added to your draw pile !" << std::endl;
    const Card* silver = g.getCardFromCardDistrib("Silver");
    if (silver == nullptr){
        std::cout << "There aren't any silver left..." << std::endl;
    }
    else{
        p.addCardToDrawStack(silver);
    }
    
}

void Bureaucrat::attack(Game& g, Player& p, std::set<Player*> targets) const{
    std::cout << "Every opponents must show and put on their draw pile a victory card from their hand." << std::endl;
    for(auto it = targets.begin(); it != targets.end(); ++it){
        Player* player = *it;
        std::vector<const Card*> victories = player->getCardsInHandOfType(CardType::VICTORY);
        if(!victories.empty()){
            std::vector<Player*> remaining;
            for(auto next = std::next(it); next != targets.end(); ++next) remaining.push_back(*next);
            g.setPendingPlayers(remaining);
            g.requestSelection(player, victories, 1, SelectionMode::DISCARD,
                               SelectionConstraint::EXACTLY,
                               Game::SelectionContext::BUREAUCRAT_TOPDECK);
            return;
        }
    }
}
