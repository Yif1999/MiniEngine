#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "runtime/engine/engine.h"
#include "editor/include/editor.h"
#include "runtime/core/base/macro.h"

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);

    std::filesystem::path config_file_path = executable_path.parent_path() / "Editor.ini";

    MiniEngine::Engine* engine = new MiniEngine::Engine();

    engine->startEngine(config_file_path.generic_string());
    engine->initialize();

    MiniEngine::Editor* editor = new MiniEngine::Editor();
    editor->initialize(engine);

    editor->run();

    editor->clear();

    engine->clear();
    engine->shutdownEngine();

    return 0;
}
