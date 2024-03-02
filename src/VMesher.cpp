#include <VMesher.hpp>
#include <godot_cpp/classes/ref.hpp>

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

            godot::Ref<VMaterial> material = memnew(VMaterial(surface.FaceMaterial));

            auto it = _Mesh->Textures.find(VCore::TextureType::DIFFIUSE);
            if(it != _Mesh->Textures.end())
                material->SetAlbedoTexture(ConvertTextureToGodot(it->second));

            it = _Mesh->Textures.find(VCore::TextureType::EMISSION);
            if(it != _Mesh->Textures.end())
                material->SetEmissionTexture(ConvertTextureToGodot(it->second));
            
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
