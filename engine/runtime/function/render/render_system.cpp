#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/render/window_system.h"
#include "runtime/function/global/global_contex.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_model.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/zbuffer/transform.h"
#include "runtime/function/render/zbuffer/hierarchy_zbuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define window_size 512

namespace MiniEngine
{
    unsigned char *pixels;
    unsigned char *texture;
    int width, height, nChannels;

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
        // setup render model
        m_model = std::make_shared<Model>("asset/mesh/monkey.obj");
        m_display = std::make_shared<Model>("asset/mesh/plane.obj");
        // setup render shader
        m_shader = std::make_shared<Shader>("shader/glsl/unlit.vert", "shader/glsl/unlit.frag");
        // setup render camera
        m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 3.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        // setup virtual camera
        m_virtualcamera = std::make_shared<Camera>(glm::vec3(0.0f, 1.0f, 0.0f));
        // setup material texture
        stbi_set_flip_vertically_on_load(true);
        texture=stbi_load(m_model->mats[0].map_Kd.c_str(), &width, &height, &nChannels, 3);

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

        // zbuffer initialize
        hierarchy_zbuffer_initialize(window_size);
    }

    void RenderSystem::tick(float delta_time)
    {
        // clean canvas
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // clean RT
        memset(pixels,0,sizeof(char)*window_size*window_size*3);

        // render RT
        m_shader->use();
        m_shader->setInt("texture1", 0);
        m_display->Draw(m_shader);

        // update RT
        glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)window_size / (float)window_size, 0.1f, 1000.0f);
        glm::mat4 view = m_camera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        Model model_copy = *m_model.get();
        hierarchy_zbuffer_rasterize(&model_copy,model,view,projection,pixels,texture,width,height);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,window_size,window_size,GL_RGB,GL_UNSIGNED_BYTE,pixels);

        // update camera
        // m_camera->ProcessMouseMovement(1.0f,0.f,true);
        // m_camera->ProcessKeyboard(FORWARD,0.001f);

        // draw UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello, world!"); 
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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