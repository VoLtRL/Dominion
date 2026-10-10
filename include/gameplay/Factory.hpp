#pragma once

#include "../abstract/Card.hpp"
#include "../cards/Adventurer.hpp"
#include "../cards/Bureaucrat.hpp"
#include "../cards/Cellar.hpp"
#include "../cards/Chancellor.hpp"
#include "../cards/Chapel.hpp"
#include "../cards/Copper.hpp"
#include "../cards/CouncilRoom.hpp"
#include "../cards/Curse.hpp"
#include "../cards/Duchy.hpp"
#include "../cards/Estate.hpp"
#include "../cards/Feast.hpp"
#include "../cards/Festival.hpp"
#include "../cards/Gardens.hpp"
#include "../cards/Gold.hpp"
#include "../cards/Laboratory.hpp"
#include "../cards/Library.hpp"
#include "../cards/Market.hpp"
#include "../cards/Militia.hpp"
#include "../cards/Mine.hpp"
#include "../cards/Moat.hpp"
#include "../cards/Moneylender.hpp"
#include "../cards/Province.hpp"
#include "../cards/Remodel.hpp"
#include "../cards/Silver.hpp"
#include "../cards/Smithy.hpp"
#include "../cards/Spy.hpp"
#include "../cards/Thief.hpp"
#include "../cards/ThroneRoom.hpp"
#include "../cards/Village.hpp"
#include "../cards/Witch.hpp"
#include "../cards/Woodcutter.hpp"
#include "../cards/Workshop.hpp"
#include <memory>

class Factory{
    private:
        static std::map<std::string, std::unique_ptr<Card>> m_card_map;
        Factory() {
            m_card_map["Adventurer"] = std::make_unique<Adventurer>();
            m_card_map["Bureaucrat"] = std::make_unique<Bureaucrat>();
            m_card_map["Cellar"] = std::make_unique<Cellar>();
            m_card_map["Chancellor"] = std::make_unique<Chancellor>();
            m_card_map["Chapel"] = std::make_unique<Chapel>();
            m_card_map["Copper"] = std::make_unique<Copper>();
            m_card_map["CouncilRoom"] = std::make_unique<CouncilRoom>();
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
            m_card_map["Spy"] = std::make_unique<Spy>();
            m_card_map["Thief"] = std::make_unique<Thief>();
            m_card_map["ThroneRoom"] = std::make_unique<ThroneRoom>();
            m_card_map["Village"] = std::make_unique<Village>();
            m_card_map["Witch"] = std::make_unique<Witch>();
            m_card_map["Woodcutter"] = std::make_unique<Woodcutter>();
            m_card_map["Workshop"] = std::make_unique<Workshop>();
        }

    static const Factory& getInstance() {
        static Factory instance;
        return instance;
    }

    public:
        static const Card* get(const std::string& cardName);
};
