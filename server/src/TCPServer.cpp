#include "TCPServer.hpp"


void Server::close_client_connection(int user_id) {
    std::lock_guard<std::mutex> guard(mtx);
    auto it = std::remove_if(clients.begin(), clients.end(),
        [user_id](const auto& client) { return client.user_id == user_id; });
	
    // If "it" is not equal to clients.end() then at least one element has been deleted
    if (it != clients.end()) {
        it->thread.detach();
        close(it->socket);
        clients.erase(it, clients.end());
    }
}

void Server::message_broadcaster(const std::string &message, int sender_id) {
    for (const auto& client : clients) {
        if (client.user_id != sender_id) {
            if ((send(client.socket, message.c_str(), message.size(), 0)) == -1) { 
                std::cerr << "Failed to send message for user-" << client.user_id << std::endl;
            }
        }
    }
}

void Server::client_handler(int client_socket, int user_id) {
    std::array<char, BUFSIZE> buffer;
    std::string message;

    // display welcome message
    message = "user-" + std::to_string(user_id) + " has joined.";
    message_broadcaster(message, user_id);

    // Start to receiving and sending messages
    while (const auto bytes_received = recv(client_socket, buffer.data(), sizeof(buffer), 0)) {
        if (bytes_received == -1) {
            throw std::runtime_error("Failed to received message");
        }
        
        if (std::string (buffer.data()) == "!exit") {
            message = "user-" + std::to_string(user_id) + " has left.";
            message_broadcaster(message, user_id);

            close_client_connection(user_id);

            return;
        }

        buffer[bytes_received] = '\0';

        message = "user-" + std::to_string(user_id) + ": " + std::string(buffer.data());
        message_broadcaster(message, user_id);

        buffer.fill(0);
    }
}

void Session::start() {
    // create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        throw std::runtime_error("Failed to create server socket");
    }

    // configure server address and port
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
 
    // bind server socket
    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Failed to bind server socket to address");
    }

    // start to listening new connections
    if (listen(server_socket, 25) == -1) {
        throw std::runtime_error("Failed to listen incoming connections");
    }

    struct sockaddr_in client_addr;
	unsigned int len = sizeof(sockaddr_in);
    int client_socket;

    Server server;
    int id = 0;    // needed for user identification

    // accept new connections and create a thread
    while (true) {
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &len);
        if (client_socket == -1) {
            throw std::runtime_error("Failed to accept client");
        }

        id++;

        std::thread thread(&Server::client_handler, &server, client_socket, id);
        std::lock_guard<std::mutex> guard(server.mtx);
		server.clients.push_back({id, client_socket, "User", (move(thread))});
    }

    // detach all threads
    for (auto& client : server.clients) {
        if (client.thread.joinable()) {
            client.thread.detach();
        }
    }
}

int Session::get_server_socket() {
    return server_socket;
}
