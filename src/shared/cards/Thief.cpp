#include "shared/cards/Thief.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

#include <algorithm>
#include <iterator>

void Thief::play(Game& g, Player& p) const {
    
}

void Thief::attack(Game& g, Player& p, std::set<Player*> targets) const {
    for(auto it = targets.begin(); it != targets.end(); ++it) {
        Player *player = *it;
        if(player == &p) continue;
        std::vector<const Card*> revealed = {player->drawCard(), player->drawCard()};
        std::vector<const Card*> localTreasures;
        for(const Card *card : revealed) {
            if(card && card->isA(CardType::TREASURE)) localTreasures.push_back(card);
        }
        std::vector<const Card*> nonTreasures;
        for(const Card *card : revealed) {
            if(card && !card->isA(CardType::TREASURE)) nonTreasures.push_back(card);
        }
        for(const Card *card : nonTreasures) player->addCardToDiscardStack(card);
        std::vector<Player*> remaining;
        for(auto next = std::next(it); next != targets.end(); ++next) remaining.push_back(*next);
        g.setPendingPlayers(remaining);
        if(localTreasures.size() > 1) {
            auto treasures = g.getPendingThiefTreasures();
            treasures.insert(treasures.end(), localTreasures.begin(), localTreasures.end());
            g.setPendingThiefTreasures(treasures);
            g.requestSelection(&p, localTreasures, 1, SelectionMode::TRASH,
                               SelectionConstraint::EXACTLY,
                               Game::SelectionContext::THIEF_TRASH);
            return;
        }
        if(localTreasures.size() == 1) {
            auto treasures = g.getPendingThiefTreasures();
            treasures.push_back(localTreasures.front());
            g.setPendingThiefTreasures(treasures);
        }
        if(!remaining.empty()) {
            attack(g, p, std::set<Player*>(remaining.begin(), remaining.end()));
            return;
        }
    }
    const auto &treasures = g.getPendingThiefTreasures();
    if(!treasures.empty()) {
        g.requestSelection(&p, treasures, treasures.size(), SelectionMode::GAIN,
                           SelectionConstraint::AT_MOST, Game::SelectionContext::THIEF_GAIN);
    }
}