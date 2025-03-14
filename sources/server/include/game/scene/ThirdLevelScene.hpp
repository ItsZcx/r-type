#ifndef THIRD_LEVEL_SCENE_HPP
#define THIRD_LEVEL_SCENE_HPP

#include "AScene.hpp"
#include "Entity.hpp"

#include <vector>

namespace server
{

class ThirdLevelScene : public AScene
{
  public:
    ThirdLevelScene() = delete;
    ThirdLevelScene(Manager &manager);
    ~ThirdLevelScene();

    void exit() override;
    void enter() override;

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                SceneEvent &event) override;

  private:
    std::vector<Entity> _mobs;
};

}  // namespace server

#endif  // THIRD_LEVEL_SCENE_HPP
