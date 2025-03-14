#include "Server.hpp"
#include "iostream"

#include <string>

using namespace server;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " [PORT]" << std::endl;
        return 1;
    }

    unsigned short port = static_cast<unsigned short>(std::stoi(argv[1]));
    Server server(port);
    server.run();
}
