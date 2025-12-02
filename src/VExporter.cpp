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

#include "GodotSurface.hpp"
#include "VModel.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/object.hpp"
#include <VMaterial.hpp>
#include <VExporter.hpp>
#include <Helper/Convert.hpp>
#include <godot_cpp/classes/material.hpp>
#include <memory>

namespace VCoreGDExt
{
    void VExporter::_bind_methods()
    {
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "UNKNOWN", (int)VCore::ExporterType::UNKNOWN);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "OBJ", (int)VCore::ExporterType::OBJ);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "GLTF", (int)VCore::ExporterType::GLTF);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "GLB", (int)VCore::ExporterType::GLB);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "ESCN2", (int)VCore::ExporterType::ESCN2);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "ESCN3", (int)VCore::ExporterType::ESCN3);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "PLY", (int)VCore::ExporterType::PLY);
        godot::ClassDB::bind_integer_constant(get_class_static(), "ExporterTypes", "FBX", (int)VCore::ExporterType::FBX);

        godot::ClassDB::bind_method(godot::D_METHOD("save_scene"), &VExporter::SaveScene);
        godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &VExporter::GetType);
    }

    VCore::Mesh VExporter::ConverToVMesh(const godot::Ref<godot::ArrayMesh> &p_Mesh)
    {
        auto mesh = std::make_shared<VCore::SMesh>();
        for (size_t i = 0; i < p_Mesh->get_surface_count(); i++)
        {
            auto material = godot::Object::cast_to<VMaterial>(*p_Mesh->surface_get_material(i));

            auto data = p_Mesh->surface_get_arrays(i);

            auto surface = new GodotSurface();
            surface->MaterialHandle = material->GetMaterialIdx();
            surface->Positions = data[godot::Mesh::ARRAY_VERTEX];
            surface->Normals = data[godot::Mesh::ARRAY_NORMAL];
            surface->Colors = data[godot::Mesh::ARRAY_COLOR];
            surface->Indices = data[godot::Mesh::ARRAY_INDEX];

            mesh->Surfaces.push_back(std::move(surface));
        }

        return mesh;
    }

    int VExporter::SaveScene(const godot::String &p_Path, const godot::Node3D *p_Scene)
    {
        auto renderTree = std::make_shared<VCore::RenderSceneTree_t>();
        CopyProperties(renderTree.get(), p_Scene);
        CreateNodeTree(renderTree, renderTree.get(), p_Scene);

        try
        {
            Save(p_Path, renderTree);
            return godot::Error::OK;
        }
        catch(const std::exception& e)
        {
            ERR_PRINT(e.what());
            return godot::Error::ERR_FILE_CANT_WRITE;
        }

        return godot::Error::ERR_FILE_UNRECOGNIZED;
    }

    void VExporter::CreateNodeTree(VCore::RenderSceneTree &p_Tree, VCore::CSceneNodeBase *p_Parent, const godot::Node *p_Node)
    {
        for (int i = 0; i < p_Node->get_child_count(); i++) 
        {
            auto n = p_Node->get_child(i);
            auto c = godot::Object::cast_to<godot::Node3D>(n);
            if(c)
            {
                VCore::CSceneNode *node = nullptr;

                auto vmodel = godot::Object::cast_to<VModel>(c);
                if(vmodel)
                {
                    p_Tree->AddModel(ConverToVMesh(vmodel->get_mesh()));
                    node = new VCore::CSceneModelNode(p_Parent, p_Tree->GetModels().size() - 1);
                }
                else
                    node = new VCore::CSceneNode(p_Parent);
                
                CopyProperties(node, c);
                p_Tree->AddChild(node);
                

                CreateNodeTree(p_Tree, node, c);
            }
            else
                CreateNodeTree(p_Tree, p_Parent, n);
        }
    }
} // namespace VCoreGDExt
