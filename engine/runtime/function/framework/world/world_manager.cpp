#include "runtime/function/framework/world/world_manager.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/framework/scene/scene.h"
#include "runtime/function/global/global_contex.h"

namespace MiniEngine
{
    WorldManager::~WorldManager() { clear(); }

    void WorldManager::initialize()
    {
        m_is_world_loaded   = false;
        m_current_world_url = g_runtime_global_context.m_config_manager->getDefaultWorldUrl();
    }

    void WorldManager::clear()
    {
        // unload all loaded scenes
        for (auto scene_pair : m_loaded_scenes)
        {
            scene_pair.second->unload();
        }
        m_loaded_scenes.clear();

        m_current_active_scene.reset();

        // clear world
        m_current_world_resource.reset();
        m_current_world_url.clear();
        m_is_world_loaded = false;
    }

    void WorldManager::tick(float delta_time)
    {
        if (!m_is_world_loaded)
        {
            loadWorld(m_current_world_url);
        }

        // tick the active scene
        std::shared_ptr<Scene> active_scene = m_current_active_scene.lock();
        if (active_scene)
        {
            active_scene->tick(delta_time);
        }
    }

    bool WorldManager::loadWorld(const std::string& world_url)
    {
        LOG_INFO("loading world: {}", world_url);
        WorldRes   world_res;
        const bool is_world_load_success = g_runtime_global_context.m_asset_manager->loadAsset(world_url, world_res);
        if (!is_world_load_success)
        {
            return false;
        }

        m_current_world_resource = std::make_shared<WorldRes>(world_res);

        const bool is_scene_load_success = loadScene(world_res.m_default_scene_url);
        if (!is_scene_load_success)
        {
            return false;
        }

        // set the default scene to be active scene
        auto iter = m_loaded_scenes.find(world_res.m_default_scene_url);
        ASSERT(iter != m_loaded_scenes.end());

        m_current_active_scene = iter->second;

        m_is_world_loaded = true;

        LOG_INFO("world load succeed!");
        return true;
    }

    bool WorldManager::loadScene(const std::string& scene_url)
    {
        std::shared_ptr<Scene> scene = std::make_shared<Scene>();
        // set current scene temporary
        m_current_active_scene       = scene;

        const bool is_scene_load_success = scene->load(scene_url);
        if (is_scene_load_success == false)
        {
            return false;
        }

        m_loaded_scenes.emplace(scene_url, scene);

        return true;
    }

    void WorldManager::reloadCurrentScene()
    {
        auto active_scene = m_current_active_scene.lock();
        if (active_scene == nullptr)
        {
            LOG_WARN("current scene is nil");
            return;
        }

        const std::string scene_url = active_scene->getSceneResUrl();
        active_scene->unload();
        m_loaded_scenes.erase(scene_url);

        const bool is_load_success = loadScene(scene_url);
        if (!is_load_success)
        {
            LOG_ERROR("load scene failed {}", scene_url);
            return;
        }

        // update the active scene instance
        auto iter = m_loaded_scenes.find(scene_url);
        ASSERT(iter != m_loaded_scenes.end());

        m_current_active_scene = iter->second;

        LOG_INFO("reload current evel succeed");
    }

    void WorldManager::saveCurrentScene()
    {
        auto active_scene = m_current_active_scene.lock();

        if (active_scene == nullptr)
        {
            LOG_ERROR("save scene failed, no active scene");
            return;
        }

        active_scene->save();
    }
} // namespace MiniEngine
