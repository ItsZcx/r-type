#ifndef LOBBY_SCENE_HPP
#define LOBBY_SCENE_HPP

#include "AScene.hpp"

namespace server
{

class LobbyScene : public AScene
{
  public:
    LobbyScene() = delete;
    LobbyScene(Manager &manager);
    ~LobbyScene();

    void exit() override;
    void enter() override;

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                SceneEvent &action) override;

  private:
    Manager &_manager;
};

}  // namespace server

#endif  // LOBBY_SCENE_HPP
