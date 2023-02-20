#include "runtime/function/framework/scene/scene.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/res_type/common/scene.h"

#include "runtime/engine/engine.h"
#include "runtime/function/framework/object/object.h"

#include <limits>

namespace MiniEngine
{
    void Scene::clear()
    {
        m_gobjects.clear();
    }

    GObjectID Scene::createObject(const ObjectInstanceRes& object_instance_res)
    {
        GObjectID object_id = ObjectIDAllocator::alloc();
        ASSERT(object_id != k_invalid_gobject_id);

        std::shared_ptr<GObject> gobject;
        try
        {
            gobject = std::make_shared<GObject>(object_id);
        }
        catch (const std::bad_alloc&)
        {
            LOG_FATAL("cannot allocate memory for new gobject");
        }

        bool is_loaded = gobject->load(object_instance_res);
        if (is_loaded)
        {
            m_gobjects.emplace(object_id, gobject);
        }
        else
        {
            LOG_ERROR("loading object " + object_instance_res.m_name + " failed");
            return k_invalid_gobject_id;
        }
        return object_id;
    }

    bool Scene::load(const std::string& scene_res_url)
    {
        LOG_INFO("loading scene: {}", scene_res_url);

        m_scene_res_url = scene_res_url;

        SceneRes   scene_res;
        const bool is_load_success = g_runtime_global_context.m_asset_manager->loadAsset(scene_res_url, scene_res);
        if (is_load_success == false)
        {
            return false;
        }

        for (const ObjectInstanceRes& object_instance_res : scene_res.m_objects)
        {
            createObject(object_instance_res);
        }

        // create active character
        for (const auto& object_pair : m_gobjects)
        {
            std::shared_ptr<GObject> object = object_pair.second;
            if (object == nullptr)
                continue;

        }

        m_is_loaded = true;

        LOG_INFO("scene load succeed");

        return true;
    }

    void Scene::unload()
    {
        clear();
        LOG_INFO("unload scene: {}", m_scene_res_url);
    }

    bool Scene::save()
    {
        LOG_INFO("saving scene: {}", m_scene_res_url);
        SceneRes output_scene_res;

        const size_t                    object_cout    = m_gobjects.size();
        std::vector<ObjectInstanceRes>& output_objects = output_scene_res.m_objects;
        output_objects.resize(object_cout);

        size_t object_index = 0;
        for (const auto& id_object_pair : m_gobjects)
        {
            if (id_object_pair.second)
            {
                id_object_pair.second->save(output_objects[object_index]);
                ++object_index;
            }
        }

        const bool is_save_success =
            g_runtime_global_context.m_asset_manager->saveAsset(output_scene_res, m_scene_res_url);

        if (is_save_success == false)
        {
            LOG_ERROR("failed to save {}", m_scene_res_url);
        }
        else
        {
            LOG_INFO("scene save succeed");
        }

        return is_save_success;
    }

    void Scene::tick(float delta_time)
    {
        if (!m_is_loaded)
        {
            return;
        }

        for (const auto& id_object_pair : m_gobjects)
        {
            assert(id_object_pair.second);
            if (id_object_pair.second)
            {
                id_object_pair.second->tick(delta_time);
            }
        }

    }

    std::weak_ptr<GObject> Scene::getGObjectByID(GObjectID go_id) const
    {
        auto iter = m_gobjects.find(go_id);
        if (iter != m_gobjects.end())
        {
            return iter->second;
        }

        return std::weak_ptr<GObject>();
    }

    void Scene::deleteGObjectByID(GObjectID go_id)
    {
        auto iter = m_gobjects.find(go_id);
        if (iter != m_gobjects.end())
        {
            std::shared_ptr<GObject> object = iter->second;
        }

        m_gobjects.erase(go_id);
    }

} // namespace MiniEngine
