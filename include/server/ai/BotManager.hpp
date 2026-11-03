#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include "server/Room.hpp"
#include "server/ai/Bot.hpp"
#include "shared/structs/ActionType.hpp"
#include "shared/structs/Payloads.hpp"

class BotManager : public QObject {
    Q_OBJECT
public:
    BotManager(int targetRoomId, const QString &botName, Room *room,
              const Strategy &strategy, const DifficultyProfile &profile, QObject *parent = nullptr)
        : QObject(parent),
          m_socket(this),
          m_roomId(targetRoomId),
          m_name(botName),
          m_room(room),
          m_bot(botName.toStdString(), strategy, profile) {

        connect(&m_socket, &QTcpSocket::connected, this, &BotManager::onConnected);
        connect(&m_socket, &QTcpSocket::readyRead, this, &BotManager::onReadyRead);
    }

    BotManager(int targetRoomId, const QString &botName, Room *room,
               const DifficultyProfile &profile, QObject *parent = nullptr)
        : QObject(parent),
          m_socket(this),
          m_roomId(targetRoomId),
          m_name(botName),
          m_room(room),
          m_bot(botName.toStdString(), profile) {

        connect(&m_socket, &QTcpSocket::connected, this, &BotManager::onConnected);
        connect(&m_socket, &QTcpSocket::readyRead, this, &BotManager::onReadyRead);
    }

    void start(quint16 port = 4040) {
        m_socket.connectToHost("127.0.0.1", port);
    }

private slots:
    void onConnected() {
        // Dès qu'il est connecté au GameServer, le bot rejoint la Room
        sendAction(ActionType::JOIN_ROOM, Network::JoinRoomPayload(m_roomId, m_name));
    }

    void onReadyRead() {
        while (m_socket.canReadLine()) {
            QByteArray line = m_socket.readLine().trimmed();
            if (line.isEmpty()) continue;

            QJsonDocument doc = QJsonDocument::fromJson(line);
            if (!doc.isObject()) continue;

            QJsonObject obj = doc.object();
            ActionType action = stringToActionType(obj["action"].toString().toStdString());
            QJsonObject data = obj["data"].toObject();

            QTimer::singleShot(1000, this, [this, action, data]() {
                handleServerMessage(action, data);
            });
        }
    }

private:
    void sendAction(ActionType action, const Network::Payload &payload) {
        if (m_socket.state() != QAbstractSocket::ConnectedState) return;
        QJsonObject msg;
        msg["action"] = QString::fromStdString(actionTypeToString(action));
        msg["data"] = payload.toJson();
        m_socket.write(QJsonDocument(msg).toJson(QJsonDocument::Compact) + '\n');
    }

    void handleServerMessage(ActionType action, const QJsonObject &data) {
        if (!m_room || !m_room->getGame()) return;
        Game *game = m_room->getGame();
        Player *me = game->getPlayerByName(m_name.toStdString());
        if (!me) return;

        // 1. Demande de RÉACTION suite à une carte Attaque
        if (action == ActionType::REACTION && data.contains("reactions")) {
            auto payload = Network::ReactionPayload::fromJson(data);
            if (!payload) return;
            GameView view(lastState, *me);
            SelectionParameters params = {
                1,
                "",
                "",
                SelectionConstraint::AT_MOST,
                payload->reactions
            };
            std::vector<const Card*> chosen = m_bot.chooseFromList(view, SelectionMode::REACTION, params);
            const Card* card = chosen.empty() ? nullptr : chosen[0];
            sendAction(ActionType::REACTION,
                       Network::ReactionResponsePayload(m_name, card != nullptr, card));
            return;
        }

        // 2. Demande de SÉLECTION de cartes (Chapel, Militia, Remodel, Workshop...)
        if (action == ActionType::SELECTION_REQUEST) {
            auto payload = Network::SelectionRequestPayload::fromJson(data);
            if (!payload) return;

            GameView view(lastState, *me);
            SelectionParameters params{
                (size_t)payload->numberOfCards,
                "",
                "",
                payload->constraint,
                payload->cards
            };
            std::vector<const Card*> chosen = m_bot.chooseFromList(
                view,
                payload->mode,
                params
            );
            m_bot.showDecisionTrace();
            sendAction(ActionType::SELECTION_REQUEST, Network::SelectionResponsePayload(chosen));
            return;
        }

        // 3. Mise à jour du GAME_STATE : le bot vérifie si c'est à lui de jouer
        if (action == ActionType::GAME_STATE) {
            auto payload = Network::GameStatePayload::fromJson(data);
            if (!payload) return;

            lastState = payload;

            if(!m_bot.hasStrategy()) {
                m_bot.setStrategy(Strategy::bigMoney(lastState->cardsInShop));
            }

            // Si ce n'est pas le tour du bot, ou si une sélection/réaction est en attente, on attend !
            if (payload->activePlayerName != m_name ||
                payload->pendingInteractionPhase != PendingInteractionPhase::NONE ||
                game->hasPendingSelection() ||
                game->waitingForPlayers()) {
                return;
            }

            GameView view(lastState, *me);

            if (payload->currentPhase == GamePhase::ACTION) {
                if (payload->actions > 0) {
                    const Card *cardToPlay = m_bot.chooseAction(view);
                    m_bot.showDecisionTrace();
                    if(cardToPlay && Bot::isPlayValid(view, cardToPlay)) {
                        sendAction(ActionType::PLAY_CARD, Network::CardPlayed(CardType::ACTION, cardToPlay));
                    }
                    if(payload->actions == 1 || !cardToPlay) {
                        sendAction(ActionType::NEXT_PHASE, Network::EmptyPayload());
                    }
                }
                return;
            }

            if (payload->currentPhase == GamePhase::BUY) {
                // Jouer d'abord tous les trésors en main
                if (!payload->hand.getSpecificCards(CardType::TREASURE).empty()) {
                    sendAction(ActionType::PLAY_ALL_TREASURES, Network::EmptyPayload());
                    return;
                }
                // Tenter un achat
                if (payload->buys > 0) {
                    const Card *cardToBuy = m_bot.chooseBuy(view);
                    m_bot.showDecisionTrace();
                    if(cardToBuy && Bot::isPurchaseValid(view, cardToBuy)) {
                        sendAction(ActionType::BUY_CARD, Network::BuyCardPayload(cardToBuy));
                        return;
                    }
                }
                // Finir le tour
                sendAction(ActionType::END_TURN, Network::EmptyPayload());
                return;
            }
        }
    }

    QTcpSocket m_socket;
    int m_roomId;
    QString m_name;
    Room *m_room;
    Bot m_bot;
    std::optional<Network::GameStatePayload> lastState;
};