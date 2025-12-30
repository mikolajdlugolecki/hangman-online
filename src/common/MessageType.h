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
    REMAINING_TIME,          // seconds_left
    GAME_STATE,              // masked_word|errors_left|time_left
    ROUND_TIMEOUT, // 1st_nick:1st_points:1st_errors|2nd_nick:2nd_points:2nd_errors[|3rd_nick:3rd_points:3rd_errors]
    ROUND_SINGLE_FINISHED, // points|errors
    ROUND_ALL_FINISHED, // 1st_nick:1st_points:1st_errors|2nd_nick:2nd_points:2nd_errors[|3rd_nick:3rd_points:3rd_errors]
    GAME_SUMMARY,       // player1:points|player2:...
    PING,               // *empty*
    REAUTH              // pin
};
}

#endif
