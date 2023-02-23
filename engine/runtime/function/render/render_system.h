#pragma once

#include <array>
#include <memory>
#include <optional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_guid_allocator.h"
#include "runtime/function/render/render_swap_context.h"
#include "runtime/function/render/render_type.h"
#include "editor/include/editor_ui.h"

namespace MiniEngine
{
    class WindowSystem;
    class RenderScene;
    class RenderResource;
    class RenderCamera;
    class RenderShader;
    class WindowUI;
    class Model;

    struct EngineContentViewport
    {
        float x{0.f};
        float y{0.f};
        float width{0.f};
        float height{0.f};
    };

    struct RHIViewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    class RenderSystem
    {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        void initialize(RenderSystemInitInfo init_info);
        void tick(float delta_time);
        void clear();

        void swapLogicRenderData();
        RenderSwapContext &getSwapContext();
        std::shared_ptr<RenderCamera> getRenderCamera() const;

        void initializeUIRenderBackend(WindowUI* window_ui);
        void updateEngineContentViewport(float offset_x, float offset_y, float width, float height);
        uint32_t getGuidOfPickedMesh(const Vector2 &picked_uv);
        GObjectID getGObjectIDByMeshID(uint32_t mesh_id) const;

        EngineContentViewport getEngineContentViewport() const;

        void createAxis(std::array<RenderEntity, 3> axis_entities, std::array<RenderMeshData, 3> mesh_datas);
        void setVisibleAxis(std::optional<RenderEntity> axis);
        void setSelectedAxis(size_t selected_axis);
        GuidAllocator<GameObjectPartId> &getGOInstanceIdAllocator();
        GuidAllocator<MeshSourceDesc> &getMeshAssetIdAllocator();

        void clearForSceneReloading();

        unsigned int framebuffer, texColorBuffer, rbo;

    private:
        RenderSwapContext m_swap_context;

        GLFWwindow *m_window;
        WindowUI *m_window_ui;
        RHIViewport m_viewport;
        std::shared_ptr<RenderCamera> m_render_camera;
        std::shared_ptr<RenderScene> m_render_scene;
        std::shared_ptr<RenderResource> m_render_resource;
        std::shared_ptr<RenderShader> m_render_shader;
        std::shared_ptr<Model> m_render_model;

        void processSwapData();
    };

    
}