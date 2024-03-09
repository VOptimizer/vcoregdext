# Class: VCoreGDExt::VMaterial

> Represents a voxel material. This class can be use like a StandardMaterial3D, however if you plan to use this library only at development time you need to convert this material into an actual StandardMaterial3D.

### godot::String godot::String VCoreGDExt::VMaterial::GetName() const

### void void VCoreGDExt::VMaterial::SetName(const godot::String &_Name)

### float float VCoreGDExt::VMaterial::GetMetallic() const

### void void VCoreGDExt::VMaterial::SetMetallic(float _Metallic)

### float float VCoreGDExt::VMaterial::GetSpecular() const

### void void VCoreGDExt::VMaterial::SetSpecular(float _Specular)

### float float VCoreGDExt::VMaterial::GetRoughness() const

### void void VCoreGDExt::VMaterial::SetRoughness(float _Roughness)

### float float VCoreGDExt::VMaterial::GetIOR() const

### void void VCoreGDExt::VMaterial::SetIOR(float _Ior)

### float float VCoreGDExt::VMaterial::GetPower() const

### void void VCoreGDExt::VMaterial::SetPower(float _Power)

### float float VCoreGDExt::VMaterial::GetTransparency() const

### void void VCoreGDExt::VMaterial::SetTransparency(float _Transparency)

### void void VCoreGDExt::VMaterial::SetAlbedoTexture(const godot::Ref< godot::Texture > &_Texture)

### godot::Ref< godot::Texture > godot::Ref< godot::Texture > VCoreGDExt::VMaterial::GetAlbedoTexture() const

### void void VCoreGDExt::VMaterial::SetEmissionTexture(const godot::Ref< godot::Texture > &_Texture)

### godot::Ref< godot::Texture > godot::Ref< godot::Texture > VCoreGDExt::VMaterial::GetEmissionTexture() const

### godot::Ref< godot::StandardMaterial3D > godot::Ref< godot::StandardMaterial3D > VCoreGDExt::VMaterial::ToStandardMaterial3D() const

#### Return:

> Converts this material to a standard godot material. This is only needed if don't want to deploy this library with your project, and just want to import assets at development time.

### VCore::Material VCore::Material VCoreGDExt::VMaterial::GetMaterial()

> For internal use only!

### godot::RID godot::RID VCoreGDExt::VMaterial::_get_shader_rid() const override

### bool bool VCoreGDExt::VMaterial::_can_do_next_pass() const override

### bool bool VCoreGDExt::VMaterial::_can_use_render_priority() const override

### godot::Shader::Mode godot::Shader::Mode VCoreGDExt::VMaterial::_get_shader_mode() const override

