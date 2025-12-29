#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

namespace ClientMessageTypes
{
enum Type
{                // PAYLOADS:
    LOGIN = 0,   // nick
    LOGOUT,      // *empty*
    CREATE_ROOM, // *empty*
    JOIN_ROOM,   // room_id|pin
    LEAVE_ROOM,  // *empty*
    START_GAME,  // *empty*
    GUESS,       // letter
    PONG,        // *empty*
    RECONNECT,   // nick|room_id
    ERROR        // message
};
}

namespace ServerMessageTypes
{
enum Type
{                            // PAYLOADS:
    LOGIN_OK = 0,            // *empty*
    LOGIN_FAILED,            // error
    ROOM_CREATED,            // room_id|pin
    ROOM_OK,                 // *empty*
    ROOM_FAILED,             // error
    ROOM_USERS_LIST,         // player1_nick|player2_nick...
    ROOM_OWNERSHIP_TRANSFER, // *empty*
    ROOM_UPDATE,             // player1:status:errors:points|player2:...
    GAME_STARTED,            // word_length|max_errors|max_time
    GUESS_OK,                // *empty*
    GUESS_WRONG,             //
    GAME_STATE,              // masked_word|errors_left|time_left
    ROUND_FINISHED,          // result|points
    GAME_SUMMARY,            // player1:points|player2:...
    PING,                    // *empty*
    REAUTH                   // pin
};
}

#endif
