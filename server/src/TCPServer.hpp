#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <stdexcept>
#include <array>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <algorithm>
#include <mutex>

constexpr int PORT = 8080;
constexpr int BUFSIZE = 128;


struct Client {
    int user_id;
    int socket;
    std::string name;
    std::thread thread;
};

class Server {
public:
    std::vector<Client> clients;
    std::mutex mtx;
    
    void close_client_connection(int user_id);
    void message_broadcaster(const std::string &message, int sender_id);
    void client_handler(int client_socket, int user_id);
};

class Session {
public:
    Session() {
        start();
    }

    int get_server_socket();

private:
    int server_socket;
    void start();
};

#endif