#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

namespace Request {
    enum Type {             // PAYLOADS:
        LOGIN = 0,			// nick
        LOGOUT,				// *empty*
        CREATE_ROOM,		// *empty*
        JOIN_ROOM,			// room_id|pin
        LEAVE_ROOM,			// *empty*
        START_GAME,			// *empty*
        GUESS,				// letter
        PONG,				// *empty*
        RECONNECT,			// nick|room_id
        ERROR,				// message
        NUM_OF_REQUESTS
    };
}

namespace Response {
    enum Type {             // PAYLOADS:
        ROOM_CREATED = 0,	// room_id|pin
        ROOM_UPDATE,		// player1:status:errors:points|player2:...
        GAME_STARTED,		// word_length|max_errors|max_limit
        GAME_STATE,			// masked_word|errors_left|time_left
        ROUND_FINISHED,		// result|points
        GAME_SUMMARY,		// player1:points|player2:...
        PING,				// *empty*
        REAUTH,				// pin
        LOGIN_OK,           // *empty*
        LOGIN_FAILED,       // error
        NUM_OF_RESPONSES
    };
}

#endif