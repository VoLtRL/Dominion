#include "../../include/gameplay/InputManager.hpp"

#include <algorithm>

std::vector<const Card *> InputManager::chooseCardsFromSelection(Player& p, GamePhase phase, SelectionParameters parameters)
{

    std::vector<const Card*> selection = parameters.selection;
    size_t numberOfCardsToChoose = parameters.nbToSelect;
    SelectionMode mode = parameters.mode;

    if (false /*Game.getActivePlayer.isBot()*/)
    {
        // Je le fait moi
    }
    else
    {
        if (getInputType())
        {
            // Selection via la console

            std::cout << parameters.presentation;

            if (selection.empty())
            {
                std::cout << "There are no valid cards in this selection." << std::endl;
                return {};
            }

            size_t allowedCount = std::min(numberOfCardsToChoose, selection.size());

            if ((mode == SelectionMode::EXACTLY && allowedCount == selection.size()) ||
                (mode == SelectionMode::AT_LEAST && allowedCount >= selection.size()))
            {
                std::cout << "You need to choose all cards" << std::endl;
                return selection;
            }

            std::cout << parameters.label;

            std::cout << (mode == SelectionMode::EXACTLY ? ":\n" : " (0 to finish):\n");

            displaySelection(selection);

            // Indices into `selection`, in order of choice
            std::vector<size_t> chosen;

            while (true)
            {
                const bool reachedN = chosen.size() >= allowedCount;

                // Automatic stop
                if (chosen.size() == allowedCount || (mode != SelectionMode::AT_LEAST && reachedN))
                {
                    break;
                }

                const bool canStop = (mode == SelectionMode::AT_MOST) || (mode == SelectionMode::AT_LEAST && reachedN);

                std::cout << "[" << chosen.size() << " chosen] Enter a card number"
                            << (canStop ? " (0 to finish)" : "") << ": ";

                std::string input = "";
                try
                {
                    if (!std::getline(std::cin, input))
                    {
                        input = -1;
                        break; // input stream closed
                    }
                }
                catch (const std::exception &)
                {
                    input = -1;
                }

                if (parameters.allowAll)
                {
                    if (input == "all")
                    {
                        if (selection.size() > allowedCount && mode != SelectionMode::AT_LEAST)
                        {
                            std::cout << "Too many cards!" << std::endl;
                            continue;
                        }
                        chosen.clear();
                        for (size_t i = 0; i < selection.size(); i++)
                        {
                            chosen.push_back(i + 1);
                        }
                        continue;
                    }
                    if (input == "none")
                    {
                        if (mode != SelectionMode::AT_MOST && allowedCount > 0)
                        {
                            std::cout << "Too few cards chosen!" << std::endl;
                            continue;
                        }
                    }
                }

                int index = -1;
                try
                {
                    size_t pos = 0;
                    index = std::stoi(input, &pos);
                    if (pos != input.size())
                    {
                        input = -1; // reject things like "12abc"
                    }
                }
                catch (const std::exception &)
                {
                    std::cout << "Invalid index." << std::endl;
                    continue;
                }

                if (index == 0 && canStop)
                {
                    break;
                }

                if (index < 1 || static_cast<size_t>(index) > selection.size())
                {
                    std::cout << "Invalid index. Please try again.\n";
                    continue;
                }

                auto posi = std::find(chosen.begin(), chosen.end(), index);
                if (posi != chosen.end())
                {
                    std::cout << "Deselecting card number " << index << std::endl;
                    chosen.erase(posi);
                }
                else
                {
                    std::cout << "Selecting card number " << index << std::endl;
                    chosen.push_back(index);
                }
            }

            if (chosen.empty())
            {
                std::cout << "No cards were chosen." << std::endl;
                return {};
            }
            std::vector<const Card *> res;
            for (auto index : chosen)
            {
                res.push_back(selection[index - 1]);
            }
            return res;
        }
        else
        {
            // Selection via l'interface, jte laisse faire
        }
    }
    return {};
}


std::vector<std::string> InputManager::parseLines(const std::string &str) {
  std::vector<std::string> lines;
  std::string line;
  for (const char c : str) {
    if (c == '\n') {
      lines.push_back(line);
      line.clear();
    } else {
      line += c;
    }
  }
  if (!line.empty()) {
    lines.push_back(line);
  }
  return lines;
}

void InputManager::displaySelection(const std::vector<const Card *> &selection) {
  const std::string gap = "  ";
  const size_t cardsPerRow = 5;

  for (size_t start = 0; start < selection.size(); start += cardsPerRow) {
    const size_t end = std::min(start + cardsPerRow, selection.size());

    std::vector<std::vector<std::string>> cards;
    std::vector<size_t> widths;
    size_t maxHeight = 0;

    // Découper chaque carte en lignes
    for (size_t i = start; i < end; ++i) {
      if (selection[i] == nullptr) {
        continue;
      }
      std::vector<std::string> lines = parseLines(selection[i]->getString());
      widths.push_back(lines.empty() ? 0 : lines[0].size());
      maxHeight = std::max(maxHeight, lines.size());
      cards.push_back(std::move(lines));
    }

    // Étirer les cartes trop courtes : lignes vides avant la bordure basse
    for (size_t c = 0; c < cards.size(); ++c) {
      auto &lines = cards[c];
      if (lines.size() >= maxHeight || lines.size() < 2)
        continue;

      const size_t w = widths[c];
      // ligne vide : "|" + espaces + "|"
      const std::string emptyRow = "|" + std::string(w - 2, ' ') + "|";

      const size_t missing = maxHeight - lines.size();
      lines.insert(lines.end() - 1, missing, emptyRow);
    }

    // Affichage ligne par ligne
    for (size_t row = 0; row < maxHeight; ++row) {
      for (size_t c = 0; c < cards.size(); ++c) {
        if (row < cards[c].size())
          std::cout << cards[c][row];
        else
          std::cout << std::string(widths[c], ' ');

        if (c + 1 < cards.size())
          std::cout << gap;
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
}