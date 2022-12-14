#pragma once

#include <memory>

namespace MiniEngine
{
    class UI;
    class Engine;

    class Editor 
    {
        friend class UI;

    public:
        Editor();
        virtual ~Editor();

        void initialize(Engine* engine_runtime);
        void clear();

        void run();

    protected:
        std::shared_ptr<UI> m_editor_ui;
        Engine* m_engine_runtime{ nullptr };
    };
}
