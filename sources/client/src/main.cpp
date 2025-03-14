#include "game/Game.hpp"

using namespace client;

int main()
{
    Game game;
    game.init();
    std::cout << "Exiting game..." << std::endl;
    return 0;
}