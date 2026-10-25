#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "../abstract/Card.hpp"
#include "../abstract/Treasure.hpp"

class Player;


struct PlayerView {
    int HandCount=0,DrawCount=0,DiscardCount = 0;
    int victoryPoints = 0;
};

// Just a container of useful informations for bots
class GameView {
    private:
        std::map<const Card*, int> supplies;
        std::map<Player&, PlayerView> otherPlayersStats;
        std::map<const Card*, int> cardCount;
        int turnCount;
        int victoryPoints;

    public:


        bool isOwned(const Card* card) const {
            return cardCount.find(card) != cardCount.end();
        }

        int getSupplyCount(const Card* card) const {
            if(isOwned(card)){
                return cardCount.at(card);
            }
            return 0;
        }

        int getTurn() const {
            return turnCount;
        }

        int getCoins() const{
            int c = 0;
            for (auto data : cardCount){
                const Card* card = data.first;
                int count = data.second;
                if(card->isA(CardType::TREASURE)){
                    const Treasure* t = dynamic_cast<const Treasure*>(card);
                    c += t->getCoins();
                }
            }
            return c;
        }

        int getDeckSize() const {
            int c = 0;
            for (auto data : cardCount){
                const Card* card = data.first;
                int count = data.second;
                c += count;
            }
            return c;
        }

        double getMoneyDensity() const {
            double coins = getCoins(), nb = getDeckSize();
            return coins/nb;
        }

        int getEmptyPilesCount() const {
            int c = 0;
            for(auto data : supplies){
                const Card* card = data.first;
                int count = data.second;
                if (count == 0) { c+= 1; }
            }
            return c;
        }

        bool isInKingdom(const Card* card) const {
            return supplies.find(card) != supplies.end();
        }

        int getScoreDiff() const {
            int max = 0;
            for(auto data : otherPlayersStats){
                max = std::max(max, data.second.victoryPoints);
            }
            return victoryPoints-max;
        }
};