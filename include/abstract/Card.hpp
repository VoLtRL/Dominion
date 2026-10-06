#pragma once
#include <string>
#include <set>
#include "CardType.hpp"

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

        std::string getName(){
            return name;
        }
        std::set<CardType> getTypes(){
            return types;
        }
        std::string getDescription(){
            return description;
        }
        int getCost(){
            return cost;
        }

        virtual void onPlay(Game& g, Player& p);
        virtual void onDiscard(Game& g, Player& p);
        virtual void onTrash(Game& g, Player& p);
        virtual void onGain(Game& g, Player& p);
        bool isA(CardType);
        virtual std::string getString();
        virtual void display();

};