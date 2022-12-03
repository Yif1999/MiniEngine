#include "runtime/function/render/render_system.h"

#include "runtime/core/base/macro.h"

// #include "runtime/resource/asset_manager/asset_manager.h"
// #include "runtime/resource/config_manager/config_manager.h"

// #include "runtime/function/render/render_camera.h"
// #include "runtime/function/render/render_pass.h"
// #include "runtime/function/render/render_resource.h"
// #include "runtime/function/render/render_scene.h"
#include "runtime/function/render/window_system.h"
#include "runtime/function/global/global_contex.h"
#include "runtime/resource/component/shader.h"
#include "runtime/resource/common/model.h"
#include "runtime/resource/component/camera.h"
// #include "runtime/function/render/passes/main_camera_pass.h"

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
        // std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
        // ASSERT(config_manager);
        // std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
        // ASSERT(asset_manager);

        // glad load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_FATAL(__FUNCTION__, "failed to create window");
            return;
        }

        // configure global opengl state
        glEnable(GL_MULTISAMPLE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // global rendering resource
        
        // GlobalRenderingRes global_rendering_res;
        // const std::string& global_rendering_res_url = config_manager->getGlobalRenderingResUrl();
        // asset_manager->loadAsset(global_rendering_res_url, global_rendering_res);

        // upload ibl, color grading textures
        // LevelResourceDesc level_resource_desc;
        // level_resource_desc.m_ibl_resource_desc.m_skybox_irradiance_map = global_rendering_res.m_skybox_irradiance_map;
        // level_resource_desc.m_ibl_resource_desc.m_skybox_specular_map   = global_rendering_res.m_skybox_specular_map;
        // level_resource_desc.m_ibl_resource_desc.m_brdf_map              = global_rendering_res.m_brdf_map;
        // level_resource_desc.m_color_grading_resource_desc.m_color_grading_map =
        //     global_rendering_res.m_color_grading_map;

        // m_render_resource = std::make_shared<RenderResource>();
        // m_render_resource->uploadGlobalRenderResource(m_rhi, level_resource_desc);

        // setup render camera
        // const CameraPose& camera_pose = global_rendering_res.m_camera_config.m_pose;
        // m_render_camera               = std::make_shared<RenderCamera>();
        // m_render_camera->lookAt(camera_pose.m_position, camera_pose.m_target, camera_pose.m_up);
        // m_render_camera->m_zfar  = global_rendering_res.m_camera_config.m_z_far;
        // m_render_camera->m_znear = global_rendering_res.m_camera_config.m_z_near;
        // m_render_camera->setAspect(global_rendering_res.m_camera_config.m_aspect.x /
        //                            global_rendering_res.m_camera_config.m_aspect.y);

        // setup render scene
        // m_render_scene                  = std::make_shared<RenderScene>();
        // m_render_scene->m_ambient_light = {global_rendering_res.m_ambient_light.toVector3()};
        // m_render_scene->m_directional_light.m_direction =
        //     global_rendering_res.m_directional_light.m_direction.normalisedCopy();
        // m_render_scene->m_directional_light.m_color = global_rendering_res.m_directional_light.m_color.toVector3();
        // m_render_scene->setVisibleNodesReference();
    }

    void RenderSystem::tick(float delta_time)
    {
        // // process swap data between logic and render contexts
        // processSwapData();

        // // prepare render command context
        // m_rhi->prepareContext();

        // // update per-frame buffer
        // m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);

        // // update per-frame visible objects
        // m_render_scene->updateVisibleObjects(std::static_pointer_cast<RenderResource>(m_render_resource),
        //                                      m_render_camera);

        // // prepare pipeline's render passes data
        // m_render_pipeline->preparePassData(m_render_resource);

        // g_runtime_global_context.m_debugdraw_manager->tick(delta_time);

        // // render one frame
        // m_render_pipeline->forwardRender(m_rhi, m_render_resource);



        // input
        // -----
        // processInput(m_window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        Shader ourShader("shader/glsl/unlit.vert", "shader/glsl/unlit.frag");
        Model ourModel("../scene/staircase/stairscase.obj");
        ourShader.use();

        // view/projection transformations
        Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
        float lastX = 1280 / 2.0f;
        float lastY = 720 / 2.0f;
        bool firstMouse = true;

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)1280 / (float)720, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        // glfwSwapBuffers();
        glfwPollEvents();

    }

    void RenderSystem::clear()
    {
        // if (m_rhi)
        // {
        //     m_rhi->clear();
        // }
        // m_rhi.reset();

        // if (m_render_scene)
        // {
        //     m_render_scene->clear();
        // }
        // m_render_scene.reset();

        // if (m_render_resource)
        // {
        //     m_render_resource->clear();
        // }
        // m_render_resource.reset();
        
        // if (m_render_pipeline)
        // {
        //     m_render_pipeline->clear();
        // }
        // m_render_pipeline.reset();
    }

} // namespace MiniEngine
