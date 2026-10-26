#include "../../include/ai/Strategy.hpp"

size_t Strategy::resolvePhase(const GameView& view, int i, size_t current){
    while(current+1 < phases.size() && phases[current+1].canEnter(view,i)){
        current++;
    }
    return current;
}

size_t Strategy::resolvePhase(const GameView& view, int i){
    return resolvePhase(view, i, phaseIndex);
}



double Strategy::getViability(std::map<const Card*, int> distrib) const {
    return viability ? viability(distrib) : 1.0;
}