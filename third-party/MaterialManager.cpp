/*
 * MIT License
 *
 * Copyright (c) 2025 Christian Tost
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

#include <VCore/Meshing/Material.hpp>
#include <VCore/Meshing/MaterialManager.hpp>
#include <VCore/VConfig.hpp>
#include <climits>
#include <cstdint>

namespace VCore
{
    namespace MaterialManager
    {
        static constexpr uint8_t MaxSlots = (Config::MaxMaterialSlots == UCHAR_MAX) ? (Config::MaxMaterialSlots - 1) : Config::MaxMaterialSlots;

        static uint8_t g_SlotTable[32];

        // All allocated materials.
        static CMaterial g_Slots[MaxSlots];

        //////////////////////////////////////////////////
        // Internal functions
        //////////////////////////////////////////////////

        constexpr uint64_t HashMaterial(const CMaterial &p_Material)
        {
            auto rawBytes = (const char*)(&p_Material);

            constexpr uint64_t magicPrime = 0x00000100000001b3;
            uint64_t hash = 0xcbf29ce484222325;

            for (uint32_t i = offsetof(CMaterial, Metallic); i < sizeof(CMaterial); i++)
                hash = (hash ^ rawBytes[i]) * magicPrime;

            return hash;
        }

        uint8_t FindMaterialSlot(const CMaterial &p_Material, uint8_t &p_Hash)
        {
            p_Hash = HashMaterial(p_Material);            
            const auto slot = p_Hash % MaxSlots;

            int i = slot;
            while (true)
            {
                const auto tableIdx = i / 8;

                // Find slot
                if((g_SlotTable[tableIdx] & (1 << (i - tableIdx * 8))) && g_Slots[i] == p_Material)
                    return i;

                i++;
                if(i == slot)
                    break;
            }
            
            return UCHAR_MAX;
        }

        uint8_t CreateMaterial(const uint64_t p_Hash)
        {
            const auto slot = p_Hash % MaxSlots;

            uint64_t i = slot;
            while (true)
            {
                const auto tableIdx = i / 8;

                // Find empty slot
                if(!(g_SlotTable[tableIdx] & (1 << (i - tableIdx * 8))))
                {
                    g_SlotTable[tableIdx] |= (1 << (i - tableIdx * 8));
                    return i;
                }

                i++;
                if(i == slot)
                    break;
            }

            return UCHAR_MAX;
        }

        //////////////////////////////////////////////////
        // Public functions
        //////////////////////////////////////////////////

        uint8_t AddMaterial(const CMaterial &p_Material)
        {
            auto hash = HashMaterial(p_Material);
            auto slot = CreateMaterial(hash);
            if(slot != UCHAR_MAX) 
                g_Slots[slot] = p_Material;

            return slot;
        }

        uint8_t AddOrGetMaterial(const CMaterial &p_Material)
        {
            uint8_t hash;
            auto slot = FindMaterialSlot(p_Material, hash);
            if(slot == UCHAR_MAX)
            {
                slot = CreateMaterial(hash);
                if(slot != UCHAR_MAX) 
                    g_Slots[slot] = p_Material;
            }

            return slot;
        }

        CMaterial *GetMaterial(const uint8_t p_MaterialHandle)
        {
            const auto tableIdx = p_MaterialHandle / 8;
            if(p_MaterialHandle >= MaxSlots || !(g_SlotTable[tableIdx] & (1 << (p_MaterialHandle - tableIdx * 8))))
                return nullptr;

            auto &slot = g_Slots[p_MaterialHandle];
            return &slot;
        }

        uint8_t FindMaterialSlot(const CMaterial &p_Material)
        {
            uint8_t hash;
            return FindMaterialSlot(p_Material, hash);
        }

        void DeleteMaterial(const uint8_t p_MaterialHandle)
        {
            if(p_MaterialHandle == 0 || p_MaterialHandle >= MaxSlots)
                return;

            const auto tableIdx = p_MaterialHandle / 8;
            g_SlotTable[tableIdx] &= ~(1 << (p_MaterialHandle - tableIdx * 8));
        }

        class InitHack
        {
            public:
                InitHack() 
                { g_SlotTable[0] = 0x1; }
        };
        InitHack _;
    } // namespace MaterialManager
} // namespace VCore
