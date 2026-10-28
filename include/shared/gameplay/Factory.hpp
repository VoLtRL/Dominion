#pragma once

#include "shared/abstract/Card.hpp"
#include "shared/cards/Adventurer.hpp"
#include "shared/cards/Bureaucrat.hpp"
#include "shared/cards/Cellar.hpp"
#include "shared/cards/Chancellor.hpp"
#include "shared/cards/Chapel.hpp"
#include "shared/cards/Copper.hpp"
#include "shared/cards/CouncilRoom.hpp"
#include "shared/cards/Curse.hpp"
#include "shared/cards/Duchy.hpp"
#include "shared/cards/Estate.hpp"
#include "shared/cards/Feast.hpp"
#include "shared/cards/Festival.hpp"
#include "shared/cards/Gardens.hpp"
#include "shared/cards/Gold.hpp"
#include "shared/cards/Laboratory.hpp"
#include "shared/cards/Library.hpp"
#include "shared/cards/Market.hpp"
#include "shared/cards/Militia.hpp"
#include "shared/cards/Mine.hpp"
#include "shared/cards/Moat.hpp"
#include "shared/cards/Moneylender.hpp"
#include "shared/cards/Province.hpp"
#include "shared/cards/Remodel.hpp"
#include "shared/cards/Silver.hpp"
#include "shared/cards/Smithy.hpp"
#include "shared/cards/Spy.hpp"
#include "shared/cards/Thief.hpp"
#include "shared/cards/ThroneRoom.hpp"
#include "shared/cards/Village.hpp"
#include "shared/cards/Witch.hpp"
#include "shared/cards/Woodcutter.hpp"
#include "shared/cards/Workshop.hpp"
#include <memory>
#include <map>
#include <string>


/**
 * @brief Factory class for creating and managing card instances.
 * This class provides a centralized way to access card instances by name.
 */
class Factory{
    private:
        inline static std::map<std::string, std::unique_ptr<Card>> m_card_map;

        Factory() {
            m_card_map["Adventurer"] = std::make_unique<Adventurer>();
            m_card_map["Bureaucrat"] = std::make_unique<Bureaucrat>();
            m_card_map["Cellar"] = std::make_unique<Cellar>();
            m_card_map["Chancellor"] = std::make_unique<Chancellor>();
            m_card_map["Chapel"] = std::make_unique<Chapel>();
            m_card_map["Copper"] = std::make_unique<Copper>();
            m_card_map["Council Room"] = std::make_unique<CouncilRoom>();
            m_card_map["Curse"] = std::make_unique<Curse>();
            m_card_map["Duchy"] = std::make_unique<Duchy>();
            m_card_map["Estate"] = std::make_unique<Estate>();
            m_card_map["Feast"] = std::make_unique<Feast>();
            m_card_map["Festival"] = std::make_unique<Festival>();
            m_card_map["Gardens"] = std::make_unique<Gardens>();
            m_card_map["Gold"] = std::make_unique<Gold>();
            m_card_map["Laboratory"] = std::make_unique<Laboratory>();
            m_card_map["Library"] = std::make_unique<Library>();
            m_card_map["Market"] = std::make_unique<Market>();
            m_card_map["Militia"] = std::make_unique<Militia>();
            m_card_map["Mine"] = std::make_unique<Mine>();
            m_card_map["Moat"] = std::make_unique<Moat>();
            m_card_map["Moneylender"] = std::make_unique<Moneylender>();
            m_card_map["Province"] = std::make_unique<Province>();
            m_card_map["Remodel"] = std::make_unique<Remodel>();
            m_card_map["Silver"] = std::make_unique<Silver>();
            m_card_map["Smithy"] = std::make_unique<Smithy>();
            m_card_map["Spy"] = std::make_unique<Spy>();
            m_card_map["Thief"] = std::make_unique<Thief>();
            m_card_map["Throne Room"] = std::make_unique<ThroneRoom>();
            m_card_map["Village"] = std::make_unique<Village>();
            m_card_map["Witch"] = std::make_unique<Witch>();
            m_card_map["Woodcutter"] = std::make_unique<Woodcutter>();
            m_card_map["Workshop"] = std::make_unique<Workshop>();
        }
        

        /**
         * @brief Gets the singleton instance of the Factory.
         * @return The singleton instance of the Factory.
         */
    static const Factory& getInstance() {
        static Factory instance;
        return instance;
    }

    public:
        /**
         * @brief Gets a card instance by its name.
         * @param cardName The name of the card to retrieve.
         * @return A pointer to the card instance, or nullptr if not found.
         */
        static const Card* get(const std::string& cardName);

        
};
