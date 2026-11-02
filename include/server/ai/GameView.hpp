#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "../../shared/abstract/Treasure.hpp"
#include "../../shared/gameplay/Factory.hpp"

struct PlayerView {
    int HandCount=0,DrawCount=0,DiscardCount = 0;
    int victoryPoints = 0;
};

// Just a container of useful informations for bots
class GameView {
    private:
        std::map<const Card*, int> supplies ;
        std::map<Player*, PlayerView> otherPlayersStats;
        std::map<const Card*, int> cardCount;
        int purchases;
        int actions;
        int coins;
        Player* currentPlayer;

    public:

        GameView(const Game& g, Player& p);

        bool isOwned(const Card* card) const {
            return cardCount.find(card) != cardCount.end();
        }

        int inHand(const Card* card) const {
            std::vector<const Card*> cards = currentPlayer->getHand().getCards();
            std::string name = card->getName();
            int count = 0;
            for(const Card* c : cards){
                if(c->getName() == name){
                    count++;
                }
            }
            return count;
        }

        int getSupplyCount(const Card* card) const {
            return supplies.at(card);
            return 0;
        }

        int getOwnedCount(const Card* card) const {
            if(cardCount.find(card) != cardCount.end()){
                return cardCount.at(card);
            }
            return 0;
        }

        int getCoinsInDeck() const{
            int c = 0;
            for (auto data : cardCount){
                const Card* card = data.first;
                int count = data.second;
                if(card->isA(CardType::TREASURE)){
                    const Treasure* t = dynamic_cast<const Treasure*>(card);
                    c += count * t->getCoins();
                }
            }
            return c;
        }

        int getCoins() const {
            return coins;
        }

        int getDeckSize() const {
            int c = 0;
            for (auto data : cardCount){
                int count = data.second;
                c += count;
            }
            return c;
        }

        double getMoneyDensity() const {
            double coins = getCoinsInDeck(), nb = getDeckSize();
            return coins/nb;
        }

        int getEmptyPilesCount() const {
            int c = 0;
            for(auto data : supplies){
                int count = data.second;
                if (count == 0) { c+= 1; }
            }
            return c;
        }

        bool isInKingdom(const Card* card) const {
            return supplies.find(card) != supplies.end();
        }

        int getScoreDiff(const Player& p) const {
            int max = 0;
            for(auto data : otherPlayersStats){
                max = std::max(max, data.second.victoryPoints);
            }
            return p.getVictoryPoints() - max;
        }

        int getPurchases() const {
            return purchases;
        }

        int getActions() const {
            return actions;
        }

        int getTurn() const {
            return currentPlayer->getTurnsPlayed();
        }

        int getScoreDiff() const {
            int max = 0;
            for(auto data : otherPlayersStats){
                max = std::max(max, data.second.victoryPoints);
            }
            return currentPlayer->getVictoryPoints() - max;
        }

        Player* getCurrentPlayer() const {
            return currentPlayer;
        }

        std::map<const Card*, int> getSupply() const {
            return supplies;
        }
};