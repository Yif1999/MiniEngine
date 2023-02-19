#include "runtime/function/render/render_scene.h"
#include "runtime/function/render/render_resource.h"

namespace MiniEngine
{
    void RenderScene::clear()
    {
    }

    void RenderScene::updateVisibleObjects(std::shared_ptr<RenderResource> render_resource,
                                           std::shared_ptr<RenderCamera>   camera)
    {
        updateVisibleObjectsAxis(render_resource);
    }

    void RenderScene::setVisibleNodesReference()
    {

    }

    GuidAllocator<GameObjectPartId>& RenderScene::getInstanceIdAllocator() { return m_instance_id_allocator; }

    GuidAllocator<MeshSourceDesc>& RenderScene::getMeshAssetIdAllocator() { return m_mesh_asset_id_allocator; }

    GuidAllocator<MaterialSourceDesc>& RenderScene::getMaterialAssetdAllocator()
    {
        return m_material_asset_id_allocator;
    }

    void RenderScene::addInstanceIdToMap(uint32_t instance_id, GObjectID go_id)
    {
        m_mesh_object_id_map[instance_id] = go_id;
    }

    GObjectID RenderScene::getGObjectIDByMeshID(uint32_t mesh_id) const
    {
        auto find_it = m_mesh_object_id_map.find(mesh_id);
        if (find_it != m_mesh_object_id_map.end())
        {
            return find_it->second;
        }
        return GObjectID();
    }

    void RenderScene::deleteEntityByGObjectID(GObjectID go_id)
    {
        for (auto it = m_mesh_object_id_map.begin(); it != m_mesh_object_id_map.end(); it++)
        {
            if (it->second == go_id)
            {
                m_mesh_object_id_map.erase(it);
                break;
            }
        }

        GameObjectPartId part_id = {go_id, 0};
        size_t           find_guid;
        if (m_instance_id_allocator.getElementGuid(part_id, find_guid))
        {
            for (auto it = m_render_entities.begin(); it != m_render_entities.end(); it++)
            {
                if (it->m_instance_id == find_guid)
                {
                    m_render_entities.erase(it);
                    break;
                }
            }
        }
    }

    void RenderScene::clearForLevelReloading()
    {
        m_instance_id_allocator.clear();
        m_mesh_object_id_map.clear();
        m_render_entities.clear();
    }


    void RenderScene::updateVisibleObjectsAxis(std::shared_ptr<RenderResource> render_resource)
    {

    }

} // namespace MiniEngine
