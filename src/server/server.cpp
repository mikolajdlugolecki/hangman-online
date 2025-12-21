#include <csignal>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include "../common.h"

#define MSG_SIZE 4096
#define MAX_CLIENTS 256

namespace ReadState {
	enum ReadStates {
		READ_TYPE = 0,
		READ_LENGTH,
		READ_PAYLOAD,
		NUM_OF_READ_STATES
	};
}

struct Client{
	int socket;
	sockaddr_in address;
	int closed;
	int nextReadState = ReadState::READ_TYPE;
	uint32_t incomingMessageLength;
	std::vector<char> buffer;
};

int init_server(const char *port){
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_socket == -1){
		fprintf(stderr, "Error creating server socket\n");
		exit(EXIT_FAILURE);
	}
	const int one = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) == -1){
		fprintf(stderr, "Error changing socket options\n");
		exit(EXIT_FAILURE);
	}
	sockaddr_in server_address = {};
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(port));
	if(bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1){
		fprintf(stderr, "Error binding address to a socket\n");
		exit(EXIT_FAILURE);
	}
	if(listen(server_socket, SOMAXCONN) == -1){
		fprintf(stderr, "Error marking server socket as a listening socket\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Server listening on port %s...\n", port);
	return server_socket;
}

int main(const int argc, char *argv[])
{
	if(argc != 2){
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	const int server_socket = init_server(argv[1]);
	Client clients[MAX_CLIENTS] = {};
	int num_of_clients = 0;
	pollfd pfds[MAX_CLIENTS+2] = {};
	pfds[0].fd = STDOUT_FILENO;
	pfds[1].fd = server_socket;
	pfds[0].events = pfds[1].events = POLLIN;
	char buffer[MSG_SIZE];
	while(true){
		poll(pfds, MAX_CLIENTS+2, -1);
		if(pfds[0].revents & POLLIN){
			memset(&buffer, 0, MSG_SIZE);
			read(STDIN_FILENO, buffer, MSG_SIZE);
			for(int i=0; i<num_of_clients; i++)
				if(clients[i].closed == 0)
					construct_response(clients[i].socket, Response::GAME_STARTED, buffer);
		}
		if(pfds[1].revents & POLLIN){
			memset(&buffer, 0, MSG_SIZE);
			sockaddr_in client_address = {};
			socklen_t s = sizeof(client_address);
			const int client_socket = accept(server_socket, (sockaddr*)&client_address, &s);
			if(client_socket == -1){
				fprintf(stderr, "Error accepting new client\n");
				continue;
			}
			clients[num_of_clients].socket = pfds[num_of_clients+2].fd = client_socket;
			clients[num_of_clients].address = client_address;
			pfds[num_of_clients+2].events = POLLIN;
			fprintf(stdout, "Accepted new client - %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
			num_of_clients++;
		}
		for(int i=2; i<num_of_clients+2; i++){
			if(clients[i-2].closed == 0 && pfds[i].revents & POLLIN){
				memset(&buffer, 0, MSG_SIZE);
				const int bytes = read(clients[i-2].socket, buffer, MSG_SIZE);
				clients[i-2].buffer.insert(clients[i-2].buffer.end(), buffer, buffer + bytes);
				if(bytes == 0){
					pfds[i].events = 0x0;
					clients[i-2].closed = 1;
					continue;
				}
				while (true) {
					if (clients[i-2].nextReadState == ReadState::READ_TYPE) {
						if (clients[i-2].buffer.size() < 1) break;
						auto type = static_cast<uint8_t>(clients[i-2].buffer[0]);
						clients[i-2].buffer.erase(clients[i-2].buffer.begin());
						clients[i-2].nextReadState = ReadState::READ_LENGTH;
					}
					if (clients[i-2].nextReadState == ReadState::READ_LENGTH) {
						if (clients[i-2].buffer.size() < 4) break;
						uint32_t length;
						memcpy(&length, clients[i-2].buffer.data(), 4);
						length = ntohl(length);
						clients[i-2].incomingMessageLength = length;
						clients[i-2].buffer.erase(clients[i-2].buffer.begin(), clients[i-2].buffer.begin() + 4);
						clients[i-2].nextReadState = ReadState::READ_PAYLOAD;
					}
					if (clients[i-2].nextReadState == ReadState::READ_PAYLOAD) {
						if (clients[i-2].buffer.size() < clients[i-2].incomingMessageLength) break;
						std::vector<char> payload(
						 clients[i-2].buffer.begin(),
						 clients[i-2].buffer.begin() + clients[i-2].incomingMessageLength
						);
						clients[i-2].buffer.erase(clients[i-2].buffer.begin(),
						clients[i-2].buffer.begin() + clients[i-2].incomingMessageLength);
						std::string text(payload.begin(), payload.end());
						fprintf(stdout, "Message from %s:%d - %s\n", inet_ntoa(clients[i-2].address.sin_addr), ntohs(clients[i-2].address.sin_port), text.c_str());
						clients[i-2].nextReadState = ReadState::READ_TYPE;
						clients[i-2].incomingMessageLength = 0;
					}
				}
			}
		}
	}
	close(server_socket);
    return EXIT_SUCCESS;
}
