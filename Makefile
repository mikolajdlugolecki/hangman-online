SRC = src
BUILD = build
SERVER_FILES = \
	$(SRC)/server.cpp
CLIENT_FILES = \
	$(SRC)/client.cpp
FLAGS = -Wall -Wextra --std=c++20

default: clean init server client

init:
	@mkdir $(BUILD)

server: $(SERVER_FILES)
	@g++ $(FLAGS) $(SERVER_FILES) -o $(BUILD)/server

client: $(CLIENT_FILES)
	@g++ $(FLAGS) $(CLIENT_FILES) -o $(BUILD)/client

clean:
	@rm -Rf $(BUILD)
