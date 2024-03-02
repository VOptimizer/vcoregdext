#ifndef STATICTYPE_HPP
#define STATICTYPE_HPP

#include <utility>

namespace VCoreGDExt
{
    template <typename T>
    class TStaticType
    {
        public:
            TStaticType() { m_Value.Ptr = nullptr; }
            TStaticType(const TStaticType &_Other) : TStaticType() { *this = _Other; }
            TStaticType(TStaticType &&_Other) : TStaticType() { *this = std::move(_Other); }

            TStaticType &operator=(const T &_Value)
            {
                if(!m_Value.Ptr)
                    m_Value.Type = new T(_Value);
                else
                {
                    m_Value.Type->~T();
                    m_Value.Type = new(m_Value.Ptr) T(_Value);
                }

                return *this;
            }

            TStaticType &operator=(const TStaticType &_Other)
            {
                this->operator=(_Other.m_Value.Type);
                return *this;
            }

            TStaticType &operator=(TStaticType &&_Other)
            {
                Delete();
                this->m_Value.Ptr = std::move(_Other.m_Value.Ptr);

                return *this;
            }

            operator T()
            {
                return *this->m_Value.Type;
            }

            operator bool()
            {
                return this->m_Value.Ptr != nullptr;
            }

            ~TStaticType()
            {
                Delete();
            }
        private:
            void Delete()
            {
                if(m_Value.Ptr)
                {
                    m_Value.Type->~T();
                    delete (char*)m_Value.Ptr;
                }
            }

            union
            {
                void *Ptr;
                T *Type;
            } m_Value;
    };
} // namespace VCoreGDExt

#endif