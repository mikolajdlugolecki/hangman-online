#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QVector>

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
    QString wordWithHiddenChars;
    QString lastGuessedLetter;
    int remainingTime;
    bool inProgress;

private:
    GameState();
};

#endif // GAMESTATE_H
