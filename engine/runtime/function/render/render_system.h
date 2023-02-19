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

#include "runtime/function/render/render_swap_context.h"

namespace MiniEngine
{
    class WindowSystem;
    class WindowUI;
    class RenderScene;
    class RenderCamera;

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

    private:
        RenderSwapContext m_swap_context;

        GLFWwindow *m_window;
        std::shared_ptr<RenderCamera> m_render_camera;
        std::shared_ptr<RenderScene> m_render_scene;

        void processSwapData();
    };
}