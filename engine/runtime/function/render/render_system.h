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

        std::shared_ptr<RenderCamera> getRenderCamera() const;

        void initializeUIRenderBackend(WindowUI* window_ui);
        void updateEngineContentViewport(float offset_x, float offset_y, float width, float height);
        EngineContentViewport getEngineContentViewport() const;
        
        unsigned int getFrameBuffer() {return framebuffer;}
        unsigned int getTexColorBuffer() {return texColorBuffer;}
        unsigned int getTexDepthBuffer() {return texDepthBuffer;}

    private:
        void refreshFrameBuffer();

        GLFWwindow *m_window;
        WindowUI *m_ui;
        EngineContentViewport m_viewport;
        std::shared_ptr<Model> m_render_model;
        std::shared_ptr<RenderCamera> m_render_camera;
        std::shared_ptr<RenderShader> m_render_shader;

        unsigned int texColorBuffer, texDepthBuffer, framebuffer= 0;
    };

    
}