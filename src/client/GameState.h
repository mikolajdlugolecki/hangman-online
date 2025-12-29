#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QVector>
#include <QWidget>

class GameState : public QObject
{
    Q_OBJECT
public:
    static GameState &instance()
    {
        static GameState instance;
        return instance;
    }

    QString usersNickname;
    QString roomId;
    QString roomPin;
    bool isRoomOwner;
    QVector<QString> roomPlayers;
    QString wordLength;
    int currentErrors;
    QString maxErrors;
    std::vector<char> word;
    QString lastGuessedLetter;

private:
    GameState();
};

#endif // GAMESTATE_H
