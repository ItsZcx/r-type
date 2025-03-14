#ifndef FIRST_LEVEL_SCENE_HPP
#define FIRST_LEVEL_SCENE_HPP

#include "AScene.hpp"

namespace server
{

class FirstLevelScene : public AScene
{
  public:
    FirstLevelScene() = delete;
    FirstLevelScene(Manager &manager);
    ~FirstLevelScene();

    void exit() override;
    void enter() override;

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                SceneEvent &event) override;
};

}  // namespace server

#endif  // FIRST_LEVEL_SCENE_HPP
