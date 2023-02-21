#pragma once

#include <memory>

namespace MiniEngine
{
    class EditorUI;
    class Engine;

    class Editor 
    {
        friend class EditorUI;

    public:
        Editor();
        virtual ~Editor();

        void initialize(Engine* engine_runtime);
        void clear();

        void run();

    protected:
        std::shared_ptr<EditorUI> m_editor_ui;
        Engine* m_engine_runtime{ nullptr };
    };
}
