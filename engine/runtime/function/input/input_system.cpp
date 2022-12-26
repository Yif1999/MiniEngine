#include "runtime/function/input/input_system.h"

#include "runtime/core/base/macro.h"

#include "runtime/engine/engine.h"
#include "runtime/function/global/global_contex.h"
#include "runtime/resource/res_type/component/camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace MiniEngine
{
    unsigned int k_complement_control_command = 0xFFFFFFFF;

    void InputSystem::onKey(int key, int scancode, int action, int mods)
    {
        onKeyInEditMode(key, scancode, action, mods);
    }

    void InputSystem::onKeyInEditMode(int key, int scancode, int action, int mods)
    {
        m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::jump);

        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    // close();
                    break;
                case GLFW_KEY_R:
                    break;
                case GLFW_KEY_A:
                    m_game_command |= (unsigned int)GameCommand::left;
                    break;
                case GLFW_KEY_S:
                    m_game_command |= (unsigned int)GameCommand::backward;
                    break;
                case GLFW_KEY_W:
                    m_game_command |= (unsigned int)GameCommand::forward;
                    break;
                case GLFW_KEY_D:
                    m_game_command |= (unsigned int)GameCommand::right;
                    break;
                case GLFW_KEY_SPACE:
                    m_game_command |= (unsigned int)GameCommand::jump;
                    break;
                case GLFW_KEY_LEFT_CONTROL:
                    m_game_command |= (unsigned int)GameCommand::squat;
                    break;
                case GLFW_KEY_LEFT_ALT: {
                    std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
                    window_system->setFocusMode(!window_system->getFocusMode());
                }
                break;
                case GLFW_KEY_LEFT_SHIFT:
                    m_game_command |= (unsigned int)GameCommand::sprint;
                    break;
                case GLFW_KEY_F:
                    m_game_command ^= (unsigned int)GameCommand::free_carema;
                    break;
                default:
                    break;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    // close();
                    break;
                case GLFW_KEY_R:
                    break;
                case GLFW_KEY_W:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::forward);
                    break;
                case GLFW_KEY_S:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::backward);
                    break;
                case GLFW_KEY_A:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::left);
                    break;
                case GLFW_KEY_D:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::right);
                    break;
                case GLFW_KEY_LEFT_CONTROL:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::squat);
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::sprint);
                    break;
                default:
                    break;
            }
        }
    }

    void InputSystem::onCursorPos(double current_cursor_x, double current_cursor_y)
    {
        if (g_runtime_global_context.m_window_system->getFocusMode())
        {
            m_cursor_delta_x = m_last_cursor_x - current_cursor_x;
            m_cursor_delta_y = m_last_cursor_y - current_cursor_y;
        }
        m_last_cursor_x = current_cursor_x;
        m_last_cursor_y = current_cursor_y;
    }

    void InputSystem::clear()
    {
        m_cursor_delta_x = 0;
        m_cursor_delta_y = 0;
    }

    void InputSystem::initialize()
    {
        std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
        ASSERT(window_system);

        window_system->registerOnKeyFunc(std::bind(&InputSystem::onKey,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3,
                                                   std::placeholders::_4));
        window_system->registerOnCursorPosFunc(
            std::bind(&InputSystem::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
    }

    void InputSystem::tick()
    {
        clear();

        std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
        if (window_system->getFocusMode())
        {
            m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::invalid);
        }
        else
        {
            m_game_command |= (unsigned int)GameCommand::invalid;
        }
    }
} // namespace MiniEngine
