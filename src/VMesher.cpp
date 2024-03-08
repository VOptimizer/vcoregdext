#include <VMesher.hpp>
#include <godot_cpp/classes/ref.hpp>

namespace VCoreGDExt
{
    void VMesher::_bind_methods()
    {
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "SIMPLE", (int)VCore::MesherTypes::SIMPLE);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY", (int)VCore::MesherTypes::GREEDY);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "MARCHING_CUBES", (int)VCore::MesherTypes::MARCHING_CUBES);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY_CHUNKED", (int)VCore::MesherTypes::GREEDY_CHUNKED);
        godot::ClassDB::bind_integer_constant(get_class_static(), "MesherTypes", "GREEDY_TEXTURED", (int)VCore::MesherTypes::GREEDY_TEXTURED);

        godot::ClassDB::bind_method(godot::D_METHOD("generate_chunks"), &VMesher::GenerateChunks);
        godot::ClassDB::bind_method(godot::D_METHOD("generate_mesh"), &VMesher::GenerateMesh);
        godot::ClassDB::bind_method(godot::D_METHOD("generate_scene"), &VMesher::GenerateScene);

        godot::ClassDB::bind_method(godot::D_METHOD("set_mesher_type"), &VMesher::SetMesherType);
        godot::ClassDB::bind_method(godot::D_METHOD("get_mesher_type"), &VMesher::GetMesherType);
        godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::INT, "mesher_type", godot::PROPERTY_HINT_ENUM, "SIMPLE,GREEDY,MARCHING_CUBES,GREEDY_CHUNKED,GREEDY_TEXTURED"), "set_mesher_type", "get_mesher_type");
    }

    godot::Array VMesher::GenerateScene(const godot::Ref<VSceneNode> _SceneTree, bool _MergeChildren)
    {
        godot::Array result;
        auto meshes = m_Mesher->GenerateScene(_SceneTree->GetSceneNode());
        for (auto &&mesh : meshes)
        {
            godot::Dictionary entry;
            entry["name"] = mesh->Name.c_str();
            entry["mesh"] = CreateMesh(mesh);
            entry["frameTime"] = mesh->FrameTime;
            entry["transform"] = godot::Transform3D(
                mesh->ModelMatrix.x.x, mesh->ModelMatrix.y.x, mesh->ModelMatrix.z.x,
                mesh->ModelMatrix.x.y, mesh->ModelMatrix.y.y, mesh->ModelMatrix.z.y,
                mesh->ModelMatrix.x.z, mesh->ModelMatrix.y.z, mesh->ModelMatrix.z.z,
                mesh->ModelMatrix.x.w, mesh->ModelMatrix.y.w, mesh->ModelMatrix.z.w
            );
            result.push_back(entry);
        }
        return result;
    }

    godot::Ref<godot::ArrayMesh> VMesher::GenerateMesh(godot::Ref<VModel> _Model)
    {
        auto mesh = m_Mesher->GenerateMesh(_Model->GetModel());
        if(mesh)
            return CreateMesh(mesh);

        return nullptr;
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

        // Converts all textures to godot ones.
        godot::Dictionary textures;
        auto it = _Mesh->Textures.find(VCore::TextureType::DIFFIUSE);
        if(it != _Mesh->Textures.end())
            textures[(int)VCore::TextureType::DIFFIUSE] = ConvertTextureToGodot(it->second);

        it = _Mesh->Textures.find(VCore::TextureType::EMISSION);
        if(it != _Mesh->Textures.end())
            textures[(int)VCore::TextureType::EMISSION] = ConvertTextureToGodot(it->second);

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

            godot::Ref<VMaterial> material = memnew(VMaterial(surface.FaceMaterial));

            if(textures.has((int)VCore::TextureType::DIFFIUSE))
                material->SetAlbedoTexture(textures[(int)VCore::TextureType::DIFFIUSE]);

            if(textures.has((int)VCore::TextureType::EMISSION))
                material->SetEmissionTexture(textures[(int)VCore::TextureType::EMISSION]);
            
            result->surface_set_material(surfaceIdx, material);
            surfaceIdx++;
        }

        return result;
    }

    godot::Ref<godot::ImageTexture> VMesher::ConvertTextureToGodot(const VCore::Texture &_Texture)
    {
        // Converts the colorpalette to a texture for godot.
        godot::Ref<godot::Image> img = godot::Image::create(_Texture->GetSize().x, _Texture->GetSize().y, false, godot::Image::Format::FORMAT_RGBA8);
        for (size_t x = 0; x < _Texture->GetSize().x; x++)
        {
            for (size_t y = 0; y < _Texture->GetSize().y; y++)
            {
                VCore::CColor c;
                c.FromRGBA(_Texture->GetPixel(VCore::Math::Vec2ui(x, y)));

                img->set_pixel(x, y, godot::Color(c.R / 255.f, c.G / 255.f, c.B / 255.f, c.A / 255.f));
            }
        }

        return godot::ImageTexture::create_from_image(img);
    }
} // namespace VCoreGDExt
