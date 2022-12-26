#pragma once

#include <filesystem>

namespace MiniEngine
{
    struct EngineInitParams;

    class ConfigManager
    {
    public:
        void initialize(const std::filesystem::path& config_file_path);

        const std::filesystem::path& getRootFolder() const;
        const std::filesystem::path& getAssetFolder() const;
        const std::filesystem::path& getShaderFolder() const;
        const std::filesystem::path& getEditorBigIconPath() const;
        const std::filesystem::path& getEditorSmallIconPath() const;
        const std::filesystem::path& getEditorFontPath() const;
        
        const std::string& getDefaultWorldUrl() const;
        const std::string& getGlobalRenderingResUrl() const;
        const std::string& getGlobalParticleResUrl() const;

    private:
        std::filesystem::path m_root_folder;
        std::filesystem::path m_asset_folder;
        std::filesystem::path m_shader_folder;
        std::filesystem::path m_editor_big_icon_path;
        std::filesystem::path m_editor_small_icon_path;
        std::filesystem::path m_editor_font_path;

        std::string m_default_world_url;
        std::string m_global_rendering_res_url;
        std::string m_global_particle_res_url;
    };
} // namespace MiniEngine
