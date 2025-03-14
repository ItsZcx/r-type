#include "SceneManager.hpp"

using namespace server;

SceneManager::SceneManager(Manager &manager) : _manager(manager), _event(Event::NONE), _currSceneIdx(0) {}

SceneManager::~SceneManager() {}

void SceneManager::addScene(std::unique_ptr<AScene> scene)
{
    _scenes.push_back(std::move(scene));
}

void SceneManager::removeScene(size_t idx)
{
    if (idx < _scenes.size())
    {
        _scenes.erase(_scenes.begin() + idx);
    }
}

void SceneManager::nextScene()
{
    // Exit current scene
    _scenes[_currSceneIdx]->exit();

    _currSceneIdx = _currSceneIdx + 1;

    if (_currSceneIdx >= _scenes.size())
        return;
    _scenes[_currSceneIdx]->enter();
}

void SceneManager::prevScene()
{
    if (_currSceneIdx == 0)
        return;

    // Exit current scene
    _scenes[_currSceneIdx]->exit();

    _currSceneIdx = _currSceneIdx - 1;

    _scenes[_currSceneIdx]->enter();
}

void SceneManager::atScene(size_t idx)
{
    if (idx >= _scenes.size())
        return;

    _scenes[_currSceneIdx]->exit();
    _currSceneIdx = idx;
    _scenes[_currSceneIdx]->enter();
}

void SceneManager::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime)
{
    _scenes[_currSceneIdx]->update(sceneStartTime, _event);

    switch (_event.event)
    {
        case Event::NEXT: nextScene(); break;
        case Event::PREV: prevScene(); break;
        case Event::AT: atScene(_event.idx); break;
        default: break;
    }

    _event.event = Event::NONE;
    _event.idx = (size_t)-1;  // To max value
}

size_t SceneManager::size() const
{
    return _scenes.size();
}

size_t SceneManager::currentSceneIdx() const
{
    return _currSceneIdx;
}

Registry &SceneManager::getRegistry()
{
    return _manager.getRegistry();
}
