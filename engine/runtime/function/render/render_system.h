#pragma once

#include <array>
#include <memory>
#include <optional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MiniEngine
{
    class WindowSystem;
    class WindowUI;
    class Model;
    class Shader;
    class Camera;

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
        GLFWwindow* m_window {nullptr};
        std::shared_ptr<Model> m_model;
        std::shared_ptr<Model> m_display;
        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<Shader> m_shader;
    };
} 