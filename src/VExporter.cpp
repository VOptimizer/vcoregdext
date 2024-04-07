/*
 * MIT License
 *
 * Copyright (c) 2024 Christian Tost
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <VMaterial.hpp>
#include <VExporter.hpp>
#include <Helper/Convert.hpp>
#include <godot_cpp/classes/material.hpp>

namespace VCoreGDExt
{
    void VExporter::_bind_methods()
    {
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "UNKNOWN", (int)VCore::ExporterType::UNKNOWN);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "OBJ", (int)VCore::ExporterType::OBJ);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "GLTF", (int)VCore::ExporterType::GLTF);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "GLB", (int)VCore::ExporterType::GLB);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "ESCN", (int)VCore::ExporterType::ESCN);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "PLY", (int)VCore::ExporterType::PLY);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "FBX", (int)VCore::ExporterType::FBX);

        godot::ClassDB::bind_method(godot::D_METHOD("save_mesh"), &VExporter::SaveMesh);
        godot::ClassDB::bind_method(godot::D_METHOD("save_meshes"), &VExporter::SaveMeshes);
        godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &VExporter::GetType);
    }

    VCore::Texture VExporter::ConvertGodotToTexture(const godot::Ref<godot::ImageTexture> &_Texture)
    {
        auto img = _Texture->get_image();
        if(img->get_format() != godot::Image::FORMAT_RGBA8)
            return nullptr;

        return std::make_shared<VCore::CTexture>(Convert::ToVVec2ui(img->get_size()), (uint32_t*)img->get_data().ptr());
    }

    VCore::Mesh VExporter::ConverToVMesh(const godot::Ref<godot::ArrayMesh> &_Mesh)
    {
        auto mesh = std::make_shared<VCore::SMesh>();
        for (size_t i = 0; i < _Mesh->get_surface_count(); i++)
        {
            auto material = godot::Object::cast_to<VMaterial>(*_Mesh->surface_get_material(i));
            if(material)
            {
                if(mesh->Textures.empty())
                {
                    auto texture = material->GetAlbedoTexture();
                    if(texture.is_valid())
                        mesh->Textures[VCore::TextureType::DIFFIUSE] = ConvertGodotToTexture(texture);

                    texture = material->GetEmissionTexture();
                    if(texture.is_valid())
                        mesh->Textures[VCore::TextureType::EMISSION] = ConvertGodotToTexture(texture);
                }
            }
            else
                return nullptr;

            VCore::SSurface surface;
            surface.FaceMaterial = material->GetMaterial();

            auto data = _Mesh->surface_get_arrays(i);
            surface.Vertices = data[godot::ArrayMesh::ARRAY_VERTEX];
            surface.Normals = data[godot::ArrayMesh::ARRAY_NORMAL];
            surface.UVs = data[godot::ArrayMesh::ARRAY_TEX_UV];
            godot::PackedInt32Array indices = data[godot::ArrayMesh::ARRAY_INDEX];

            surface.Indices.resize(indices.size());
            memcpy(&surface.Indices[0], indices.ptr(), indices.size() * sizeof(int));

            mesh->Surfaces.push_back(std::move(surface));
        }

        return mesh;
    }

    int VExporter::SaveMesh(const godot::String &_Path, const godot::Ref<godot::ArrayMesh> &_Mesh)
    {
        auto type = (VCore::ExporterType)GetType(_Path);
        if(type != VCore::ExporterType::UNKNOWN)
        {
            auto mesh = ConverToVMesh(_Mesh);
            if(!mesh)
                return godot::Error::ERR_INVALID_DATA;

            try
            {
                Save(_Path, mesh);
                return godot::Error::OK;
            }
            catch(const std::exception& e)
            {
                ERR_PRINT(e.what());
                return godot::Error::ERR_FILE_CANT_WRITE;
            }
        }

        return godot::Error::ERR_FILE_UNRECOGNIZED;
    }

    int VExporter::SaveMeshes(const godot::String &_Path, const godot::Array &_Mesh)
    {
        std::vector<VCore::Mesh> meshes;
        for (size_t i = 0; i < _Mesh.size(); i++)
        {
            auto v = _Mesh[i];
            if(v.get_type() == godot::Variant::DICTIONARY)
            {
                auto dict = (godot::Dictionary)v;
                if(dict.has("mesh"))
                {
                    v = dict["mesh"];
                    if(v.get_type() == godot::Variant::OBJECT)
                    {
                        auto arrayMesh = godot::Object::cast_to<godot::ArrayMesh>(((godot::Object*)v));
                        if(arrayMesh)
                        {
                            auto mesh = ConverToVMesh(arrayMesh);
                            if(!mesh)
                                return godot::Error::ERR_INVALID_DATA;

                            if(dict.has("name") && dict["name"].get_type() == godot::Variant::STRING)
                                mesh->Name = ((godot::String)dict["name"]).utf8().get_data();

                            if(dict.has("frameTime") && dict["frameTime"].get_type() == godot::Variant::INT)
                                mesh->FrameTime = (int)dict["frameTime"];

                            if(dict.has("transform") && dict["transform"].get_type() == godot::Variant::TRANSFORM3D)
                            {
                                // Math is my passion :D
                                auto transform = (godot::Transform3D)dict["transform"];
                                mesh->ModelMatrix.x.x = transform.basis.rows[0][0];
                                mesh->ModelMatrix.x.y = transform.basis.rows[1][0];
                                mesh->ModelMatrix.x.z = transform.basis.rows[2][0];

                                mesh->ModelMatrix.y.x = transform.basis.rows[0][1];
                                mesh->ModelMatrix.y.y = transform.basis.rows[1][1];
                                mesh->ModelMatrix.y.z = transform.basis.rows[2][1];

                                mesh->ModelMatrix.z.x = transform.basis.rows[0][2];
                                mesh->ModelMatrix.z.y = transform.basis.rows[1][2];
                                mesh->ModelMatrix.z.z = transform.basis.rows[2][2];

                                mesh->ModelMatrix.x.w = transform.origin.x;
                                mesh->ModelMatrix.y.w = transform.origin.y;
                                mesh->ModelMatrix.z.w = transform.origin.z;
                            }

                            meshes.push_back(mesh);
                        }
                    }
                }
            }
            else if(v.get_type() == godot::Variant::OBJECT)
            {
                auto arrayMesh = godot::Object::cast_to<godot::ArrayMesh>(((godot::Object*)v));
                if(arrayMesh)
                {
                    auto mesh = ConverToVMesh(arrayMesh);
                    if(!mesh)
                        return godot::Error::ERR_INVALID_DATA;

                    meshes.push_back(mesh);
                }
            }
        }

        try
        {
            Save(_Path, meshes);
            return godot::Error::OK;
        }
        catch(const std::exception& e)
        {
            ERR_PRINT(e.what());
            return godot::Error::ERR_FILE_CANT_WRITE;
        }

        return godot::Error::ERR_FILE_UNRECOGNIZED;
    }
} // namespace VCoreGDExt
