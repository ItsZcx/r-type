#ifndef A_SCENE_HPP
#define A_SCENE_HPP

#include "Manager.hpp"

#define WORLD_MAX_WIDTH 1920.0f
#define WORLD_MIN_WIDTH 0.0f

#define WORLD_MAX_HEIGHT 1080.0f
#define WORLD_MIN_HEIGHT 0.0f

namespace server
{

struct SceneEvent;

class AScene
{
  public:
    AScene() = delete;
    AScene(Manager &manager) : _manager(&manager) {};
    virtual ~AScene() = default;

    virtual void exit() = 0;
    virtual void enter() = 0;

    virtual void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                        SceneEvent &action) = 0;

    void setManager(Manager &manager) { _manager = &manager; }

  protected:
    Manager *_manager;
};

}  // namespace server

#endif  // A_SCENE_HPP
