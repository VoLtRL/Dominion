#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "../abstract/Card.hpp"
#include "../abstract/Treasure.hpp"
#include "../gameplay/Game.hpp"

class Player;


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
        Hand hand;
        int turnCount;
        int victoryPoints;
        int purchases;
        int actions;
        int coins;

    public:

        GameView(Game& g, Player& p) {
            for(auto data : g.getCurrentCardDistrib()){
                supplies[data.first] = data.second;
            }

            for(Player* p : g.getOtherPlayers()){
                PlayerView view = {p->getHand().getSize(), p->getDrawStack().getSize(), p->getDiscardStack().getSize(), p->getVictoryPoints()};
                otherPlayersStats[p] = view;
            }

            for(const Card* c : p.getHand().getCards()){
                cardCount[c]++;
            }

            for(auto data : p.getDrawStack().getCardCount()){
                cardCount[data.first] += data.second;
            }

            for(auto data : p.getDiscardStack().getCardCount()){
                cardCount[data.first] += data.second;
            }

            hand = p.getHand();
            turnCount = p.getTurnsPlayed();
            victoryPoints = p.getVictoryPoints();
            purchases = g.getPurchaseCount();
            actions = g.getActionCount();
            coins = g.getCoins();
        }

        bool isOwned(const Card* card) const {
            return cardCount.find(card) != cardCount.end();
        }

        int inHand(const Card* card) const {
            std::vector<const Card*> cards = hand.getCards();
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

        int getPurchases() const {
            return purchases;
        }

        int getActions() const {
            return actions;
        }
};