#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "AScene.hpp"
#include "Manager.hpp"

#include <cstddef>
#include <vector>

namespace server
{

enum class Event
{
    NONE,
    NEXT,
    PREV,
    AT
};

struct SceneEvent
{
    Event event;
    size_t idx;
};

class SceneManager
{
  public:
    SceneManager();
    SceneManager(Manager &manager);
    ~SceneManager();

    void addScene(std::unique_ptr<AScene> scene);
    void removeScene(size_t idx);

    void nextScene();
    void prevScene();
    void atScene(size_t idx);

    void update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime);

    size_t size() const;
    size_t currentSceneIdx() const;

    Registry &getRegistry();

  private:
    Manager &_manager;
    SceneEvent _event;
    size_t _currSceneIdx;
    std::vector<std::unique_ptr<AScene>> _scenes;
};

}  // namespace server

#endif  // SCENE_MANAGER_HPP