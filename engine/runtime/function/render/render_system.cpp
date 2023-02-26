#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/window_system.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/render_scene.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_model.h"
#include "runtime/function/render/render_swap_context.h"
#include "runtime/function/render/render_resource.h"

#include "runtime/function/render/pathtracing/path_tracer.h"

namespace MiniEngine
{
    RenderSystem::~RenderSystem()
    {
        clear();
    }

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
        ASSERT(config_manager);
        std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
        ASSERT(asset_manager);

        // configure global opengl state
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_FRAMEBUFFER_SRGB);

        // setup window & viewport
        m_window = init_info.window_system->getWindow();
        std::array<int, 2> window_size = init_info.window_system->getWindowSize();

        // load rendering resource
        GlobalRenderingRes global_rendering_res;
        const std::string &global_rendering_res_url = config_manager->getGlobalRenderingResUrl();
        asset_manager->loadAsset(global_rendering_res_url, global_rendering_res);

        // setup render camera
        const CameraConfig &camera_config = global_rendering_res.m_camera_config;
        const CameraPose &camera_pose = camera_config.m_pose;

        m_render_camera = std::make_shared<Camera>(camera_pose.m_position.to_glm(),
                                                   glm::vec3(0.f, 1.f, 0.f), 
                                                   camera_pose.m_rotation.z, camera_pose.m_rotation.y,
                                                   camera_config.m_aspect.x / camera_config.m_aspect.y, 
                                                   camera_config.m_z_near, camera_config.m_z_far);

        // create and setup shader
        m_render_shader = std::make_shared<Shader>((config_manager->getShaderFolder() / "lit.vert").c_str(),
                                                   (config_manager->getShaderFolder() / "lit.frag").c_str());
        m_tracer_shader = std::make_shared<Shader>((config_manager->getShaderFolder() / "unlit.vert").c_str(),
                                                   (config_manager->getShaderFolder() / "unlit.frag").c_str());
    }

    void RenderSystem::tick(float delta_time)
    {
        // refresh render target frame buffer
        refreshFrameBuffer();

        // draw models in the scene
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, m_viewport.width, m_viewport.height);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_render_model)
        {
            m_render_shader->use();
            glm::mat4 projection = m_render_camera->getPersProjMatrix();
            glm::mat4 view = m_render_camera->getViewMatrix();
            glm::mat4 model = glm::mat4(1.0f);
            m_render_shader->setMat4("projection", projection);
            m_render_shader->setMat4("view", view);
            m_render_shader->setMat4("model", model);
            m_render_shader->setVec3("viewPos", m_render_camera->Position);
            m_render_model->Draw(m_render_shader);
        }

        // draw editor ui
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (m_ui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_ui->preRender();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        // swap buffers
        glfwSwapBuffers(m_window);
    }

    void RenderSystem::loadScene(char *filepath)
    {
        clear();
        m_render_model = std::make_shared<Model>(filepath);
    }

    void RenderSystem::unloadScene()
    {
        clear();
    }

    void RenderSystem::refreshFrameBuffer()
    {
        if (framebuffer)
        {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &texColorBuffer);
            glDeleteRenderbuffers(1, &texDepthBuffer);
        }

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_viewport.width, m_viewport.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        glGenRenderbuffers(1, &texDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, texDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.width, m_viewport.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, texDepthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LOG_WARN("framebuffer is not complete");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderSystem::clear()
    {
        if (m_render_model)
        {
            m_render_model.reset();
        }
    }

    std::shared_ptr<Camera> RenderSystem::getRenderCamera() const
    {
        return m_render_camera;
    }

    void RenderSystem::updateEngineContentViewport(float offset_x, float offset_y, float width, float height)
    {
        m_viewport.x = offset_x;
        m_viewport.y = offset_y;
        m_viewport.width = width;
        m_viewport.height = height;

        m_render_camera->setAspect(width / height);
    }

    EngineContentViewport RenderSystem::getEngineContentViewport() const
    {
        return {m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height};
    }

    void RenderSystem::initializeUIRenderBackend(WindowUI *window_ui)
    {
        m_ui = window_ui;

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

} // namespace MiniEngine
