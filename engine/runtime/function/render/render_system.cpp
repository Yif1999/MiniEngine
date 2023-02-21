#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/window_system.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/render_scene.h"
#include "runtime/function/render/render_swap_context.h"
#include "runtime/function/render/render_resource.h"
#include "editor/include/editor_ui.h"

// #include "runtime/function/render/pathtracing/path_tracer.h"

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
        glEnable(GL_FRAMEBUFFER_SRGB);
        m_window = init_info.window_system->getWindow();

        // global rendering resource
        GlobalRenderingRes global_rendering_res;
        const std::string &global_rendering_res_url = config_manager->getGlobalRenderingResUrl();
        asset_manager->loadAsset(global_rendering_res_url, global_rendering_res);
        m_render_resource = std::make_shared<RenderResource>();

        // setup render camera
        const CameraPose &camera_pose = global_rendering_res.m_camera_config.m_pose;
        m_render_camera = std::make_shared<RenderCamera>();
        m_render_camera->lookAt(camera_pose.m_position, camera_pose.m_target, camera_pose.m_up);
        m_render_camera->m_zfar = global_rendering_res.m_camera_config.m_z_far;
        m_render_camera->m_znear = global_rendering_res.m_camera_config.m_z_near;
        m_render_camera->setAspect(global_rendering_res.m_camera_config.m_aspect.x /
                                   global_rendering_res.m_camera_config.m_aspect.y);

        // setup render scene
        m_render_scene = std::make_shared<RenderScene>();
        m_render_scene->setVisibleNodesReference();

        // //setup imgui
        // IMGUI_CHECKVERSION();
        // ImGui::CreateContext();
        // ImGuiIO& io = ImGui::GetIO(); (void)io;
        // ImGui_ImplGlfw_InitForOpenGL(m_window,true);
        // ImGui_ImplOpenGL3_Init("#version 330");

        // // create render texture
        // unsigned int texture1;
        // glGenTextures(1, &texture1);
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // // set the texture wrapping parameters
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // // set texture filtering parameters
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size , window_size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // // set viewport
        // m_shader->use();
        // glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
        // glm::mat4 view = m_virtualcamera->GetViewMatrix();
        // glm::mat4 model = glm::mat4(1.0f);
        // m_shader->setMat4("projection", projection);
        // m_shader->setMat4("view", view);
        // m_shader->setMat4("model", model);

        // // pathtracer initialize
        // PathTracing::PathTracer* tracer = new PathTracing::PathTracer();
        // tracer->transferModelData(m_model);
        // std::thread pt(&PathTracing::PathTracer::startRender,tracer,pixels);
        // pt.detach();
    }

    void RenderSystem::tick(float delta_time)
    {
        // clean frame buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // process swap data between logic and render contexts
        processSwapData();

        // update per-frame buffer
        m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);

        // update per-frame visible objects
        m_render_scene->updateVisibleObjects(std::static_pointer_cast<RenderResource>(m_render_resource),
                                             m_render_camera);
        // clean RT
        // memset(pixels,0,sizeof(char)*window_size*window_size*3);

        // swap RT
        // m_shader->use();
        // m_shader->setInt("texture1", 0);
        // m_display->Draw(m_shader);

        // update RT
        // glTexSubImage2D(GL_TEXTURE_2D,0,0,0,window_size,window_size,GL_RGB,GL_UNSIGNED_BYTE,pixels);

        // update camera
        // m_camera->ProcessMouseMovement(3.f,0.f,true);
        // m_camera->ProcessKeyboard(LEFT,0.024f);

        // // draw UI
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        // ImGui::Begin("scene manager");
        // ImGui::RadioButton("scene 1 (3k faces)",&scene_id,0);
        // ImGui::SameLine();
        // ImGui::RadioButton("scene 2 (25k faces)",&scene_id,1);
        // ImGui::SameLine();
        // ImGui::RadioButton("scene 3 (68k faces)",&scene_id,2);
        // ImGui::SameLine();
        // ImGui::End();
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // // update scene
        // if (scene_id!=last_scene)
        // {
        //     m_model.reset();
        //     m_model = std::make_shared<Model>("asset/scene/"+std::to_string(scene_id+1)+".obj");
        // }

        if (m_window_ui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_window_ui->preRender();

            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
        }

        // swap buffers and poll IO events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    void RenderSystem::clear()
    {
        if (m_render_scene)
        {
            m_render_scene->clear();
        }
        m_render_scene.reset();

        if (m_render_resource)
        {
            m_render_resource->clear();
        }
        m_render_resource.reset();
    }

    void RenderSystem::processSwapData()
    {
        RenderSwapData &swap_data = m_swap_context.getRenderSwapData();

        std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
        ASSERT(asset_manager);

        if (swap_data.m_scene_resource_desc.has_value())
        {
            // reset scene resource swap data to a clean state
            m_swap_context.resetSceneRsourceSwapData();
        }

        // update game object if needed
        if (swap_data.m_game_object_resource_desc.has_value())
        {
            while (!swap_data.m_game_object_resource_desc->isEmpty())
            {
                GameObjectDesc gobject = swap_data.m_game_object_resource_desc->getNextProcessObject();

                for (size_t part_index = 0; part_index < gobject.getObjectParts().size(); part_index++)
                {
                    const auto &game_object_part = gobject.getObjectParts()[part_index];
                    GameObjectPartId part_id = {gobject.getId(), part_index};

                    bool is_entity_in_scene = m_render_scene->getInstanceIdAllocator().hasElement(part_id);

                    RenderEntity render_entity;
                    render_entity.m_instance_id =
                        static_cast<uint32_t>(m_render_scene->getInstanceIdAllocator().allocGuid(part_id));
                    render_entity.m_model_matrix = game_object_part.m_transform_desc.m_transform_matrix;

                    m_render_scene->addInstanceIdToMap(render_entity.m_instance_id, gobject.getId());

                    // mesh properties
                    MeshSourceDesc mesh_source = {game_object_part.m_mesh_desc.m_mesh_file};
                    bool is_mesh_loaded = m_render_scene->getMeshAssetIdAllocator().hasElement(mesh_source);

                    RenderMeshData mesh_data;
                    if (!is_mesh_loaded)
                    {
                        mesh_data = m_render_resource->loadMeshData(mesh_source, render_entity.m_bounding_box);
                    }
                    else
                    {
                        render_entity.m_bounding_box = m_render_resource->getCachedBoudingBox(mesh_source);
                    }

                    render_entity.m_mesh_asset_id = m_render_scene->getMeshAssetIdAllocator().allocGuid(mesh_source);
                    render_entity.m_enable_vertex_blending =
                        game_object_part.m_skeleton_animation_result.m_transforms.size() > 1; // take care
                    render_entity.m_joint_matrices.resize(
                        game_object_part.m_skeleton_animation_result.m_transforms.size());
                    for (size_t i = 0; i < game_object_part.m_skeleton_animation_result.m_transforms.size(); ++i)
                    {
                        render_entity.m_joint_matrices[i] =
                            game_object_part.m_skeleton_animation_result.m_transforms[i].m_matrix;
                    }

                    // material properties
                    MaterialSourceDesc material_source;
                    if (game_object_part.m_material_desc.m_with_texture)
                    {
                        material_source = {game_object_part.m_material_desc.m_base_color_texture_file,
                                           game_object_part.m_material_desc.m_metallic_roughness_texture_file,
                                           game_object_part.m_material_desc.m_normal_texture_file,
                                           game_object_part.m_material_desc.m_occlusion_texture_file,
                                           game_object_part.m_material_desc.m_emissive_texture_file};
                    }
                    else
                    {
                        // TODO: move to default material definition json file
                        material_source = {
                            asset_manager->getFullPath("asset/texture/default/albedo.jpg").generic_string(),
                            asset_manager->getFullPath("asset/texture/default/mr.jpg").generic_string(),
                            asset_manager->getFullPath("asset/texture/default/normal.jpg").generic_string(),
                            "",
                            ""};
                    }
                    bool is_material_loaded = m_render_scene->getMaterialAssetdAllocator().hasElement(material_source);

                    RenderMaterialData material_data;
                    if (!is_material_loaded)
                    {
                        material_data = m_render_resource->loadMaterialData(material_source);
                    }

                    render_entity.m_material_asset_id =
                        m_render_scene->getMaterialAssetdAllocator().allocGuid(material_source);

                    // create game object on the graphics api side
                    if (!is_mesh_loaded)
                    {
                        // m_render_resource->uploadGameObjectRenderResource(m_rhi, render_entity, mesh_data);
                    }

                    if (!is_material_loaded)
                    {
                        // m_render_resource->uploadGameObjectRenderResource(m_rhi, render_entity, material_data);
                    }

                    // add object to render scene if needed
                    if (!is_entity_in_scene)
                    {
                        m_render_scene->m_render_entities.push_back(render_entity);
                    }
                    else
                    {
                        for (auto &entity : m_render_scene->m_render_entities)
                        {
                            if (entity.m_instance_id == render_entity.m_instance_id)
                            {
                                entity = render_entity;
                                break;
                            }
                        }
                    }
                }
                // after finished processing, pop this game object
                swap_data.m_game_object_resource_desc->pop();
            }

            // reset game object swap data to a clean state
            m_swap_context.resetGameObjectResourceSwapData();
        }

        // remove deleted objects
        if (swap_data.m_game_object_to_delete.has_value())
        {
            while (!swap_data.m_game_object_to_delete->isEmpty())
            {
                GameObjectDesc gobject = swap_data.m_game_object_to_delete->getNextProcessObject();
                m_render_scene->deleteEntityByGObjectID(gobject.getId());
                swap_data.m_game_object_to_delete->pop();
            }

            m_swap_context.resetGameObjectToDelete();
        }

        // process camera swap data
        if (swap_data.m_camera_swap_data.has_value())
        {
            if (swap_data.m_camera_swap_data->m_fov_x.has_value())
            {
                m_render_camera->setFOVx(*swap_data.m_camera_swap_data->m_fov_x);
            }

            if (swap_data.m_camera_swap_data->m_view_matrix.has_value())
            {
                m_render_camera->setMainViewMatrix(*swap_data.m_camera_swap_data->m_view_matrix);
            }

            if (swap_data.m_camera_swap_data->m_camera_type.has_value())
            {
                m_render_camera->setCurrentCameraType(*swap_data.m_camera_swap_data->m_camera_type);
            }

            m_swap_context.resetCameraSwapData();
        }
    }

    void RenderSystem::swapLogicRenderData() { m_swap_context.swapLogicRenderData(); }

    RenderSwapContext &RenderSystem::getSwapContext() { return m_swap_context; }

    std::shared_ptr<RenderCamera> RenderSystem::getRenderCamera() const { return m_render_camera; }

    void RenderSystem::updateEngineContentViewport(float offset_x, float offset_y, float width, float height)
    {
        m_viewport->x = offset_x;
        m_viewport->y = offset_y;
        m_viewport->width = width;
        m_viewport->height = height;
        m_viewport->minDepth = 0.0f;
        m_viewport->maxDepth = 1.0f;

        m_render_camera->setAspect(width / height);
    }

    EngineContentViewport RenderSystem::getEngineContentViewport() const
    {
        return {m_viewport->x, m_viewport->y, m_viewport->width, m_viewport->height};
    }

    uint32_t RenderSystem::getGuidOfPickedMesh(const Vector2 &picked_uv)
    {
        return 0;
    }

    GObjectID RenderSystem::getGObjectIDByMeshID(uint32_t mesh_id) const
    {
        return m_render_scene->getGObjectIDByMeshID(mesh_id);
    }

    void RenderSystem::createAxis(std::array<RenderEntity, 3> axis_entities, std::array<RenderMeshData, 3> mesh_datas)
    {
        for (int i = 0; i < axis_entities.size(); i++)
        {
            // m_render_resource->uploadGameObjectRenderResource(m_rhi, axis_entities[i], mesh_datas[i]);
        }
    }

    void RenderSystem::setVisibleAxis(std::optional<RenderEntity> axis)
    {
        m_render_scene->m_render_axis = axis;

        if (axis.has_value())
        {
            // std::static_pointer_cast<RenderPipeline>(m_render_pipeline)->setAxisVisibleState(true);
        }
        else
        {
            // std::static_pointer_cast<RenderPipeline>(m_render_pipeline)->setAxisVisibleState(false);
        }
    }

    void RenderSystem::setSelectedAxis(size_t selected_axis)
    {
        // std::static_pointer_cast<RenderPipeline>(m_render_pipeline)->setSelectedAxis(selected_axis);
    }

    GuidAllocator<GameObjectPartId> &RenderSystem::getGOInstanceIdAllocator()
    {
        return m_render_scene->getInstanceIdAllocator();
    }

    GuidAllocator<MeshSourceDesc> &RenderSystem::getMeshAssetIdAllocator()
    {
        return m_render_scene->getMeshAssetIdAllocator();
    }

    void RenderSystem::clearForSceneReloading()
    {
        m_render_scene->clearForSceneReloading();
    }

    void RenderSystem::initializeUIRenderBackend(WindowUI* window_ui)
    {
        m_window_ui = window_ui;

        ImGui_ImplGlfw_InitForOpenGL(m_window,true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

} // namespace MiniEngine
