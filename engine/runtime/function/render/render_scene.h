#pragma once

#include "runtime/function/framework/object/object_id_allocator.h"

#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_guid_allocator.h"
#include "runtime/function/render/render_object.h"
#include "runtime/function/render/render_common.h"

#include <optional>
#include <vector>

namespace MiniEngine
{
    class RenderResource;
    class Camera;

    class RenderScene
    {
    public:
        // render entities
        std::vector<RenderEntity> m_render_entities;

        // axis, for editor
        std::optional<RenderEntity> m_render_axis;


        // clear
        void clear();

        // update visible objects in each frame
        void updateVisibleObjects(std::shared_ptr<RenderResource> render_resource,
                                  std::shared_ptr<Camera>   camera);

        // set visible nodes ptr in render pass
        void setVisibleNodesReference();

        GuidAllocator<GameObjectPartId>&   getInstanceIdAllocator();
        GuidAllocator<MeshSourceDesc>&     getMeshAssetIdAllocator();
        GuidAllocator<MaterialSourceDesc>& getMaterialAssetdAllocator();

        void      addInstanceIdToMap(uint32_t instance_id, GObjectID go_id);
        GObjectID getGObjectIDByMeshID(uint32_t mesh_id) const;
        void      deleteEntityByGObjectID(GObjectID go_id);

        void clearForSceneReloading();

    private:
        GuidAllocator<GameObjectPartId>   m_instance_id_allocator;
        GuidAllocator<MeshSourceDesc>     m_mesh_asset_id_allocator;
        GuidAllocator<MaterialSourceDesc> m_material_asset_id_allocator;

        std::unordered_map<uint32_t, GObjectID> m_mesh_object_id_map;

        void updateVisibleObjectsAxis(std::shared_ptr<RenderResource> render_resource);
    };
} // namespace MiniEngine
