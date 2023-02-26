#pragma once

#include "runtime/function/render/render_scene.h"
#include "runtime/function/render/render_swap_context.h"
#include "runtime/function/render/render_type.h"
#include "runtime/function/render/render_common.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace MiniEngine
{
    class RHI;
    class RenderScene;
    class Camera;

    class RenderResource
    {
    public:
        virtual ~RenderResource() {}

        void clear();

        void updatePerFrameBuffer(std::shared_ptr<RenderScene>  render_scene, std::shared_ptr<Camera> camera);

        std::shared_ptr<TextureData> loadTextureHDR(std::string file, int desired_channels = 4);
        std::shared_ptr<TextureData> loadTexture(std::string file, bool is_srgb = false);
        RenderMeshData               loadMeshData(const MeshSourceDesc& source, AxisAlignedBox& bounding_box);
        RenderMaterialData           loadMaterialData(const MaterialSourceDesc& source);
        AxisAlignedBox               getCachedBoudingBox(const MeshSourceDesc& source) const;

        // storage buffer objects
        MeshPerframeStorageBufferObject                 m_mesh_perframe_storage_buffer_object;
        MeshInefficientPickPerframeStorageBufferObject m_mesh_inefficient_pick_perframe_storage_buffer_object;

    private:
        StaticMeshData loadStaticMesh(std::string mesh_file, AxisAlignedBox& bounding_box);

        std::unordered_map<MeshSourceDesc, AxisAlignedBox> m_bounding_box_cache_map;
    };
} // namespace MiniEngine
