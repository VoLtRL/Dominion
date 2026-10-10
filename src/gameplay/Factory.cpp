#include "../../include/gameplay/Factory.hpp"

const Card* Factory::get(const std::string& cardName) {
        return getInstance().m_card_map.at(cardName).get();
    }