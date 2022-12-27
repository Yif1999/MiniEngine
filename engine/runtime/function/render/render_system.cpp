#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/render/window_system.h"
#include "runtime/function/global/global_contex.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_model.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        glEnable(GL_MULTISAMPLE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        m_window = init_info.window_system->getWindow();

        // setup render model
        m_model = std::make_shared<Model>("../scene/staircase/stairscase.obj");

        // setup render shader
        m_shader = std::make_shared<Shader>("shader/glsl/unlit.vert", "shader/glsl/unlit.frag");

        // setup render camera
        m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    }

    void RenderSystem::tick(float delta_time)
    {
        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader->use();

        glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)1280 / (float)720, 0.01f, 1000.0f);
        glm::mat4 view = m_camera->GetViewMatrix();
        m_shader->setMat4("projection", projection);
        m_shader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        m_shader->setMat4("model", model);
        m_model->Draw(m_shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();

    }

    void RenderSystem::clear()
    {
        m_model.reset();
        m_shader.reset();
        m_camera.reset();
    }

} // namespace MiniEngine
