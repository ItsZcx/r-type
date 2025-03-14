#ifndef SECOND_LEVEL_SCENE_HPP
#define SECOND_LEVEL_SCENE_HPP

#include "AScene.hpp"

namespace server
{

class SecondLevelScene : public AScene
{
  public:
    SecondLevelScene() = delete;
    SecondLevelScene(Manager &manager);
    ~SecondLevelScene();

    void exit() override;
    void enter() override;

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                SceneEvent &event) override;
};

}  // namespace server

#endif  // SECOND_LEVEL_SCENE_HPP
