#pragma once

#include <filesystem>
#include <vector>

namespace MiniEngine
{
    class FileSystem 
    {
    public:
        static std::vector<std::filesystem::path> getFiles(const std::filesystem::path& directory);
    };
} // namespace MiniEngine
