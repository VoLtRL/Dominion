#pragma once
#include <string>
#include <set>
#include "CardType.hpp"
#include "../gameplay/Game.hpp"

class Player;

class Card {
    private:
        std::string name;
        int cost;
        std::string description;
        std::set<CardType> types;
    public:
        Card(std::string cardName, int goldCost, std::string cardDescription="", std::set<CardType> cardTypes)
        :name(cardName),cost(goldCost),description(cardDescription){
            types = cardTypes;
        } 
        std::string getName() const{
            return name;
        }
        std::set<CardType> getTypes() const{
            return types;
        }

        virtual void use(Game& g, Player& p) const;
        bool isA(CardType) const;

};