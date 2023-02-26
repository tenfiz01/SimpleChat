#include "TCPServer.hpp"

#include <signal.h>

Session *session_ptr;


void signal_handler(int signum) {
    if (session_ptr) {
        close(session_ptr->get_server_socket());
    }
    exit(signum);
}

int main() {
    // create signal handler for SIGINT
    signal(0, signal_handler);

    try {
        Session session;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}