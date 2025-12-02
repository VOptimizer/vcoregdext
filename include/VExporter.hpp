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

#ifndef VEXPORTER_HPP
#define VEXPORTER_HPP

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <VCore/VCore.hpp>
#include "GodotFileStream.hpp"
#include "Helper/Convert.hpp"
#include "godot_cpp/classes/node3d.hpp"

namespace VCoreGDExt
{
    class VExporter : public godot::RefCounted
    {
        GDCLASS(VExporter, godot::RefCounted)
        public:
            VExporter() = default;

            /**
             * @return Returns the exporter type of a given file.
             */
            inline int GetType(const godot::String &_Filename) const
            {
                return (int)VCore::IExporter::GetType(_Filename.utf8().get_data());
            }

            /**
             * @brief Saves an array of meshes to file.
             */
            int SaveScene(const godot::String &p_Path, const godot::Node3D *p_Scene);

            template<class T>
            inline void Save(const godot::String &_Path, const T &_MeshData)
            {
                auto exporter = VCore::IExporter::Create((VCore::ExporterType)GetType(_Path));
                exporter->Save<CGodotIOHandler, T>(_Path.utf8().get_data(), _MeshData);
            }

            ~VExporter() = default;

        protected:
            VCore::Mesh ConverToVMesh(const godot::Ref<godot::ArrayMesh> &p_Mesh);
	        static void _bind_methods();

        private:
            void CopyProperties(VCore::CSceneNodeBase *p_VNode, const godot::Node3D *p_Node)
            {
                p_VNode->Name = p_Node->get_name().to_utf8_buffer().get_string_from_utf8().utf8().get_data();
                p_VNode->Visible = p_Node->is_visible();
                p_VNode->SetPosition(Convert::ToVVec3i(p_Node->get_position()));
                p_VNode->SetScale(Convert::ToVVec3(p_Node->get_scale()));
                p_VNode->SetRotation(Convert::ToVVec3(p_Node->get_rotation()));
            }

            void CreateNodeTree(VCore::RenderSceneTree &p_Tree, VCore::CSceneNodeBase *p_Parent, const godot::Node *p_Node);
    };
} // namespace VCoreGDExt

#endif