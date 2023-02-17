// #pragma once

// #include "runtime/resource/res_type/common/world.h"

// #include <filesystem>
// #include <string>

// namespace MiniEngine
// {
//     class Scene;

//     /// Manage all game worlds, it should be support multiple worlds, including game world and editor world.
//     /// Currently, the implement just supports one active world and one active scene
//     class WorldManager
//     {
//     public:
//         virtual ~WorldManager();

//         void initialize();
//         void clear();

//         void reloadCurrentScene();
//         void saveCurrentScene();

//         void                 tick(float delta_time);
//         std::weak_ptr<Scene> getCurrentActiveScene() const { return m_current_active_scene; }


//     private:
//         bool loadWorld(const std::string& world_url);
//         bool loadScene(const std::string& scene_url);

//         bool                      m_is_world_loaded {false};
//         std::string               m_current_world_url;
//         std::shared_ptr<WorldRes> m_current_world_resource;

//         // all loaded scenes, key: scene url, vaule: scene instance
//         std::unordered_map<std::string, std::shared_ptr<Scene>> m_loaded_scenes;
//         // active scene, currently we just support one active scene
//         std::weak_ptr<Scene> m_current_active_scene;
//     };
// } // namespace MiniEngine
