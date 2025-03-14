#ifndef SERVER_HPP
#define SERVER_HPP

namespace server
{

class Server
{
  public:
    Server(unsigned short port);
    ~Server();

    void run();

  private:
    unsigned short _port;
};

}  // namespace server

#endif  // SERVER_HPP
