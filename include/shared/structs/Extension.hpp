#pragma once
#include <string>


enum class Extension {
    ADVENTURES,
    ALCHEMY,
    BASE_CARDS,
    DOMINION,
    GUILDS,
    INTRIGUE,
    PROMOTIONAL_CARDS,
    PROSPERITY,
    CORNUCOPIA,
    HINTERLANDS,
    SEASIDE,
    DARK_AGES,
};

namespace ExtensionUtils {
    inline std::string toString(Extension extension) {
        switch (extension) {
            case Extension::ADVENTURES: return "adventures";
            case Extension::ALCHEMY: return "alchemy";
            case Extension::BASE_CARDS: return "base_cards";
            case Extension::DOMINION: return "dominion";
            case Extension::GUILDS: return "guilds";
            case Extension::INTRIGUE: return "intrigue";
            case Extension::PROMOTIONAL_CARDS: return "promotional_cards";
            case Extension::PROSPERITY: return "prosperity";
            case Extension::SEASIDE: return "seaside";
            case Extension::CORNUCOPIA: return "cornucopia";
            case Extension::HINTERLANDS: return "hinterlands";
            case Extension::DARK_AGES: return "dark_ages";
            default: return "unknown_extension";
        }
    }
}

    