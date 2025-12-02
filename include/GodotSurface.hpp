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

#ifndef GODOTSURFACE_HPP
#define GODOTSURFACE_HPP

#include "Helper/Convert.hpp"
#include "godot_cpp/variant/packed_color_array.hpp"
#include <VCore/VCore.hpp>
#include <cstdint>
#include <godot_cpp/variant/variant.hpp>

namespace VCoreGDExt
{
    class GodotSurface : public VCore::ISurface
    {
        public:
            godot::PackedVector3Array Positions;
            godot::PackedVector3Array Normals;
            godot::PackedColorArray Colors;
            godot::PackedInt32Array Indices;

            uint32_t AddVertex(const VCore::SVertex *p_Vertex) override
            {
                Positions.push_back(Convert::ToGDVec3(p_Vertex->Pos));
                Normals.push_back(Convert::ToGDVec3(p_Vertex->Normal));
                Colors.push_back(Convert::ToGDColor(VCore::CColor(p_Vertex->Color)));
                
                delete p_Vertex;
                return Positions.size() - 1;
            }

            void AddFace(uint32_t p_Idx1, uint32_t p_Idx2, uint32_t p_Idx3) override
            {
                Indices.push_back(p_Idx1);
                Indices.push_back(p_Idx2);
                Indices.push_back(p_Idx3);
            }

            uint64_t GetVertexCount() const override
            {
                return (uint64_t)Positions.size();
            }

            uint64_t GetFaceCount() const override
            {
                return (uint64_t)(Indices.size() / 3);
            }

            void UpdateVertex(uint64_t p_Idx, const VCore::SVertex &p_Vertex) override
            {
                if(p_Idx < Positions.size())
                {
                    Positions[p_Idx] = Convert::ToGDVec3(p_Vertex.Pos);
                    Normals[p_Idx] = Convert::ToGDVec3(p_Vertex.Normal);
                    Colors[p_Idx] = Convert::ToGDColor(VCore::CColor(p_Vertex.Color));
                }
            }

            VCore::SVertex GetVertex(uint64_t p_Idx) const override
            {
                return VCore::SVertex(
                    Convert::ToVVec3(Positions[p_Idx]),
                    Convert::ToVVec3(Normals[p_Idx]),
                    Convert::ToColor(Colors[p_Idx]).AsABGR(),
                    3
                );
            }

            uint32_t GetIndex(uint64_t p_Idx) const override
            {
                return Indices[p_Idx];
            }

            void ReserveVertices(uint64_t p_Size) override
            {
                Positions.resize(p_Size);
                Normals.resize(p_Size);
                Colors.resize(p_Size);
            }

            void ReserveFaces(uint64_t p_Size) override
            {
                Indices.resize(p_Size * 3);
            }

            const VCore::SVertex* GetRawVertexPointer() const override
            {
                return nullptr; //m_Vertices.data();
            }

            const void* GetRawIndexPointer() const override
            {
                return Indices.ptr();
            }

            bool IsFaceCountMaxReached() const override
            {
                return (UINT64_MAX - Positions.size()) > 3;
            }

            void MergeSurface(ISurface *p_Surface) override
            {
                auto otherSurface = dynamic_cast<GodotSurface*>(p_Surface);
                if(otherSurface)
                {
                    ReserveVertices(p_Surface->GetVertexCount() + GetVertexCount());
                    ReserveFaces(p_Surface->GetFaceCount() + GetFaceCount());

                    auto startIdx = Positions.size();
                    Positions.append_array(otherSurface->Positions);
                    Normals.append_array(otherSurface->Normals);
                    Colors.append_array(otherSurface->Colors);

                    for (int64_t i = 0; i < otherSurface->Indices.size(); i++) 
                        Indices.append(startIdx + otherSurface->Indices[i]);

                    p_Surface->ClaimVertices();
                }                
            }

            void ClaimVertices() override
            {
                Positions.clear();
                Normals.clear();
                Colors.clear();
                Indices.clear();
            }
        protected:
            const void *GetUnderlyingVertexReference() const override
            {
                return nullptr;
            }

            const void *GetUnderlyingIndexReference() const override
            {
                return nullptr;
            }

#ifdef VCORE_RTTI_ENABLED
            std::type_index GetUnderlyingVertexType() const override
            {
                return typeid(nullptr);
            }

            std::type_index GetUnderlyingIndexType() const override
            {
                return typeid(nullptr);
            }
#endif
    };
}

#endif