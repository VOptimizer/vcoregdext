#include <VMesher.hpp>

namespace VCoreGDExt
{
    void VMesher::_bind_methods()
    {
        godot::ClassDB::bind_method(godot::D_METHOD("generate_chunks"), &VMesher::GenerateChunks);
    }

    godot::Array VMesher::GenerateChunks(godot::Ref<VModel> _Model, bool _OnlyDirtyChunks)
    {
        godot::Array result;

        auto meshes = m_Mesher->GenerateChunks(_Model->GetModel(), _OnlyDirtyChunks);
        for (auto &&mesh : meshes)
            result.push_back(CreateMesh(mesh.MeshData));

        return result;
    }

    godot::Ref<godot::ArrayMesh> VMesher::CreateMesh(const VCore::Mesh &_Mesh)
    {
        godot::Ref<godot::ArrayMesh> result = memnew(godot::ArrayMesh());
        godot::Array meshArray;
        meshArray.resize(godot::ArrayMesh::ARRAY_MAX);

        int surfaceIdx = 0;
        for (auto &&surface : _Mesh->Surfaces)
        {
            // Copies all indices to the godot array.
            godot::PackedInt32Array indices;
            indices.resize(surface.Indices.size());
            memcpy(indices.ptrw(), surface.Indices.data(), indices.size() * sizeof(int));  

            // Since VCore is compiled using the VConfig.hpp for godot, we can simply copy over
            // the generated mesh data.
            meshArray[godot::ArrayMesh::ARRAY_VERTEX] = surface.Vertices;
            meshArray[godot::ArrayMesh::ARRAY_NORMAL] = surface.Normals;
            meshArray[godot::ArrayMesh::ARRAY_TEX_UV] = surface.UVs;
            meshArray[godot::ArrayMesh::ARRAY_INDEX] = indices;

            result->add_surface_from_arrays(godot::ArrayMesh::PRIMITIVE_TRIANGLES, meshArray);
            result->surface_set_material(surfaceIdx, memnew(VMaterial(surface.FaceMaterial)));
            surfaceIdx++;
        }

        return result;
    }
} // namespace VCoreGDExt
