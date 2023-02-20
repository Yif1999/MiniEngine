#pragma once

#include "runtime/function/framework/object/object_id_allocator.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace MiniEngine
{
    class GObject;
    class ObjectInstanceRes;

    using SceneObjectsMap = std::unordered_map<GObjectID, std::shared_ptr<GObject>>;

    /// The main class to manage all game objects
    class Scene
    {
    public:
        virtual ~Scene(){};

        bool load(const std::string& scene_res_url);
        void unload();

        bool save();

        void tick(float delta_time);

        const std::string& getSceneResUrl() const { return m_scene_res_url; }

        const SceneObjectsMap& getAllGObjects() const { return m_gobjects; }

        std::weak_ptr<GObject>   getGObjectByID(GObjectID go_id) const;

        GObjectID createObject(const ObjectInstanceRes& object_instance_res);
        void      deleteGObjectByID(GObjectID go_id);

    protected:
        void clear();

        bool        m_is_loaded {false};
        std::string m_scene_res_url;

        // all game objects in this scene, key: object id, value: object instance
        SceneObjectsMap m_gobjects;

    };
} // namespace MiniEngine
