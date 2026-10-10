#include "../../include/gameplay/Factory.hpp"

std::map<std::string, std::unique_ptr<Card>> Factory::m_card_map;

const Card* Factory::get(const std::string& cardName) {
        return getInstance().m_card_map.at(cardName).get();
    }