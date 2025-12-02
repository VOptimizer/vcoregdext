#ifndef GODOTFILESTREAM_HPP
#define GODOTFILESTREAM_HPP

#include <godot_cpp/classes/dir_access.hpp>
#include <VCore/VCore.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>

class CGodotFileStream : public VCore::IFileStream
{
    public:
        CGodotFileStream(const std::string &_File, const char *_OpenMode);

        size_t Read(char *_Buffer, size_t _Size) override;
        size_t Write(const char *_Buffer, size_t _Size) override;
        void Seek(size_t _Offset, VCore::SeekOrigin _Origin = VCore::SeekOrigin::CUR) override;
        size_t Tell() override;
        size_t Size() override;
        void Close() override;
    private:
        godot::Ref<godot::FileAccess> m_File;
};

class CGodotIOHandler : public VCore::IIOHandler
{
    public:
        VCore::IFileStream *Open(const std::string &_File, const char *_OpenMode)
        {
            return new CGodotFileStream(_File, _OpenMode);
        }

        void Delete(const std::string &p_File)
        {
            godot::DirAccess::remove_absolute(p_File.c_str());
        }

        void Close(VCore::IFileStream *_Stream)
        {
            if(_Stream)
                delete _Stream;
        }
};

#endif