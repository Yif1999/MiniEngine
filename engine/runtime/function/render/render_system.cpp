#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/render/window_system.h"
#include "runtime/function/global/global_contex.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_model.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"

#include "runtime/function/render/pathtracing/path_tracer.h"

#define window_size 512

namespace MiniEngine
{
    RenderSystem::~RenderSystem()
    {
        clear();
    }

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        // configure opengl state
        glEnable(GL_MULTISAMPLE);

        // initialization
        pixels = new unsigned char[3*window_size*window_size];
        m_window = init_info.window_system->getWindow();
        // load display model
        m_display = std::make_shared<Model>("asset/object/basic/plane.obj");
        // load display shader
        m_shader = std::make_shared<Shader>("shader/glsl/unlit.vert", "shader/glsl/unlit.frag");
        // setup virtual camera
        m_virtualcamera = std::make_shared<Camera>(glm::vec3(0.0f, 1.0f, 0.0f));
        
        // setup render camera
        m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 1.0f, -12.0f),glm::vec3(0.0f, 1.0f, 0.0f),90.0f,-2.0f);
        // load render model
        m_model = std::make_shared<Model>("/Volumes/T7/Dev/MiniEngine/scene/staircase/stairscase.obj");

        //setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplGlfw_InitForOpenGL(m_window,true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // create render texture
        unsigned int texture1;
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size , window_size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // set viewport
        m_shader->use();
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
        glm::mat4 view = m_virtualcamera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        m_shader->setMat4("projection", projection);
        m_shader->setMat4("view", view);
        m_shader->setMat4("model", model);

        // pathtracer initialize
        PathTracing::PathTracer* tracer = new PathTracing::PathTracer();
        tracer->transferModelData(m_model);
        std::thread pt(&PathTracing::PathTracer::startRender,tracer,pixels);
        pt.detach();

    }

    void RenderSystem::tick(float delta_time)
    {
        // clean canvas
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // clean RT
        // memset(pixels,0,sizeof(char)*window_size*window_size*3);

        // swap RT
        m_shader->use();
        m_shader->setInt("texture1", 0);
        m_display->Draw(m_shader);

        // update RT
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,window_size,window_size,GL_RGB,GL_UNSIGNED_BYTE,pixels);

        // update camera
        m_camera->ProcessMouseMovement(3.f,0.f,true);
        m_camera->ProcessKeyboard(LEFT,0.024f);

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

        // swap buffers and poll IO events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    void RenderSystem::clear()
    {
        m_display.reset();
        m_model.reset();
        m_shader.reset();
        m_camera.reset();
    }

} // namespace MiniEngine
