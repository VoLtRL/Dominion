#include "shared/gameplay/GameMode.hpp"

GameMode::GameMode(const std::string& modeName,
                   const std::string& modeDescription,
                   const std::map<const Card*, int>& distribution)
    : name(modeName), description(modeDescription), cardDistribution(distribution) {}

const std::string& GameMode::getName() const {
    return name;
}

const std::string& GameMode::getDescription() const {
    return description;
}

const std::map<const Card*, int>& GameMode::getCardDistribution() const {
    return cardDistribution;
}

QJsonObject GameMode::toJson() const {
    QJsonObject json;
    json["name"] = QString::fromStdString(name);
    json["description"] = QString::fromStdString(description);
    QJsonObject distributionJson;
    for (const auto& pair : cardDistribution) {
        distributionJson[QString::fromStdString(pair.first->getName())] = pair.second;
    }
    json["cardDistribution"] = distributionJson;
    return json;
}

std::optional<GameMode> GameMode::fromJson(const QJsonObject& json) {
    if (!json.contains("name") || !json.contains("description") || !json.contains("cardDistribution")) {
        return std::nullopt;
    }
    std::map<const Card*, int> cardDistribution;
    QJsonObject distributionJson = json["cardDistribution"].toObject();
    for (const auto& key : distributionJson.keys()) {
        cardDistribution[Factory::get(key.toStdString())] = distributionJson[key].toInt();
    }
    return GameMode(json["name"].toString().toStdString(), json["description"].toString().toStdString(), cardDistribution);
}