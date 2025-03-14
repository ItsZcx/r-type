#ifndef BOSS_LEVEL_SCENE_HPP
#define BOSS_LEVEL_SCENE_HPP

#include "AScene.hpp"

namespace server
{

class BossLevelScene : public AScene
{
  public:
    BossLevelScene() = delete;
    BossLevelScene(Manager &manager);
    ~BossLevelScene();

    void exit() override;
    void enter() override;

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                SceneEvent &event) override;
};

}  // namespace server

#endif  // BOSS_LEVEL_SCENE_HPP
