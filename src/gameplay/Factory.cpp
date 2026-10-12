#include "../../include/gameplay/Factory.hpp"
#include <iostream>

const Card* Factory::get(const std::string& cardName) {
    if (m_card_map.empty()) {
        Factory::getInstance();
    }
    if (m_card_map.find(cardName) == m_card_map.end()) {
        std::cerr << "Error: Card with name '" << cardName << "' not found in the factory." << std::endl;
        return nullptr;
    }
        else {
            return m_card_map[cardName].get();

    }
}