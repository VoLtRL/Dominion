#pragma once
#include "../player/Player.hpp"
#include "EventType.hpp"
#include <set>

class GameEvent{
    private:
        EventType type;
        Player& source;
        std::set<Player*> ignored;
    
    public:
        GameEvent(EventType t, Player& s):type(t),source(s){};
        virtual ~GameEvent() = default;

        bool isIgnoredFor(Player& p) const{
            return ignored.find(&p) == ignored.end();
        }
        
        void ignorePlayer(Player& p){
            if(!isIgnoredFor(p)){
                ignored.insert(&p);
            }
        }

        EventType getType() const{
            return type;
        };
};