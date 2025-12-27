#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QVector>

class GameState : public QObject
{
    Q_OBJECT
public:
    static GameState& instance() {
        static GameState instance;
        return instance;
    }

    QString usersNickname;

    QString roomId;
    QString roomPin;
    bool isRoomOwner;
    QVector<QString> roomPlayers;
    QString word_length;
    int current_errors;
    QString max_errors;
    QString word;

private:
    GameState();
};

#endif // GAMESTATE_H
