#include <vector>
#include "shared/gameplay/GameMode.hpp"
#include "shared/gameplay/Factory.hpp"

namespace PredefinedGameModes{
    inline const std::vector<GameMode> &getGameModes(){
        static const std::vector<GameMode> gameModes = {
            GameMode("Les premières parties",
                     "Configuration recommandée pour découvrir le jeu",
                     {{Factory::get("Workshop"), 10},
                      {Factory::get("Woodcutter"), 10},
                      {Factory::get("Cellar"), 10},
                      {Factory::get("Moat"), 10},
                      {Factory::get("Smithy"), 10},
                      {Factory::get("Market"), 10},
                      {Factory::get("Militia"), 10},
                      {Factory::get("Mine"), 10},
                      {Factory::get("Remodel"), 10},
                      {Factory::get("Village"), 10}}),

            GameMode("Richesses et trésors",
                     "Aventurier, Bureaucrate, Chancelier, Chapelle, Festin, "
                     "Laboratoire, Marché, Mine, Prêteur sur gages, Salle du Trône",
                     {{Factory::get("Adventurer"), 10},
                      {Factory::get("Bureaucrat"), 10},
                      {Factory::get("Chancellor"), 10},
                      {Factory::get("Chapel"), 10},
                      {Factory::get("Feast"), 10},
                      {Factory::get("Laboratory"), 10},
                      {Factory::get("Market"), 10},
                      {Factory::get("Mine"), 10},
                      {Factory::get("Moneylender"), 10},
                      {Factory::get("Throne Room"), 10}}),

            GameMode("Interaction",
                     "Bibliothèque, Bureaucrate, Chambre du conseil, Chancelier, "
                     "Douves, Espion, Festival, Milice, Village, Voleur",
                     {{Factory::get("Library"), 10},
                      {Factory::get("Bureaucrat"), 10},
                      {Factory::get("Council Room"), 10},
                      {Factory::get("Chancellor"), 10},
                      {Factory::get("Moat"), 10},
                      {Factory::get("Spy"), 10},
                      {Factory::get("Festival"), 10},
                      {Factory::get("Militia"), 10},
                      {Factory::get("Village"), 10},
                      {Factory::get("Thief"), 10}}),

            GameMode(
                "Changement de taille",
                "Atelier, Bûcheron, Cave, Chapelle, Festin, Jardins, Laboratoire, "
                "Sorcière, Village, Voleur",
                {{Factory::get("Workshop"), 10},
                 {Factory::get("Woodcutter"), 10},
                 {Factory::get("Cellar"), 10},
                 {Factory::get("Chapel"), 10},
                 {Factory::get("Feast"), 10},
                 {Factory::get("Gardens"), 12},
                 {Factory::get("Laboratory"), 10},
                 {Factory::get("Witch"), 10},
                 {Factory::get("Village"), 10},
                 {Factory::get("Thief"), 10}}),

            GameMode("Place du Village",
                     "Bibliothèque, Bûcheron, Bureaucrate, Cave, Festival, Forgeron, "
                     "Marché, Rénovation, Salle du Trône, Village",
                     {{Factory::get("Library"), 10},
                      {Factory::get("Woodcutter"), 10},
                      {Factory::get("Bureaucrat"), 10},
                      {Factory::get("Cellar"), 10},
                      {Factory::get("Festival"), 10},
                      {Factory::get("Smithy"), 10},
                      {Factory::get("Market"), 10},
                      {Factory::get("Remodel"), 10},
                      {Factory::get("Throne Room"), 10},
                      {Factory::get("Village"), 10}})};

        return gameModes;
    }

    inline std::optional<GameMode> getGameModeByName(const std::string& name) {
        const auto& gameModes = getGameModes();
        auto it = std::find_if(gameModes.begin(), gameModes.end(),
                               [&name](const GameMode& gm) { return gm.getName() == name; });
        if (it != gameModes.end()) {
            return *it;
        }
        return std::nullopt;
    }

};