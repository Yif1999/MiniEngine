#pragma once

#include <array>
#include <memory>
#include <optional>

namespace MiniEngine
{
    class WindowSystem;
    class RHI;
    class RenderResourceBase;
    class RenderPipelineBase;
    class RenderScene;
    class RenderCamera;
    class WindowUI;
    class DebugDrawManager;

    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    struct EngineContentViewport
    {
        float x { 0.f};
        float y { 0.f};
        float width { 0.f};
        float height { 0.f};
    };

    class RenderSystem
    {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        void initialize(RenderSystemInitInfo init_info);
        void tick(float delta_time);
        void clear();

    };
} 