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
             * @brief Saves a mesh to file.
             */
            int SaveMesh(const godot::String &_Path, const godot::Ref<godot::ArrayMesh> &_Mesh);

            /**
             * @brief Saves an array of meshes to file.
             */
            int SaveMeshes(const godot::String &_Path, const godot::Array &_Mesh);

            template<class T>
            inline void Save(const godot::String &_Path, const T &_MeshData)
            {
                auto exporter = VCore::IExporter::Create((VCore::ExporterType)GetType(_Path));
                exporter->Save<CGodotIOHandler, T>(_Path.utf8().get_data(), _MeshData);
            }

            ~VExporter() = default;

        protected:
            VCore::Texture ConvertGodotToTexture(const godot::Ref<godot::ImageTexture> &_Texture);
            VCore::Mesh ConverToVMesh(const godot::Ref<godot::ArrayMesh> &_Mesh);
	        static void _bind_methods();
    };
} // namespace VCoreGDExt

#endif