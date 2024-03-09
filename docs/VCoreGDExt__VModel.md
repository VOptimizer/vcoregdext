# Class: VCoreGDExt::VModel

> A voxel model represents a collection of voxels, textures and materials.

### godot::String godot::String VCoreGDExt::VModel::GetName() const

### void void VCoreGDExt::VModel::SetName(const godot::String &_Name)

### godot::AABB godot::AABB VCoreGDExt::VModel::GetBBox() const

#### Return:

> Returns the bounding box of a voxel model inside the voxel grid.

### int int VCoreGDExt::VModel::AddMaterial(const godot::Ref< VMaterial > &_Material)

> Must be called for each new material to be used in the model.

### godot::Ref<  godot::Ref< VMaterial > VCoreGDExt::VModel::GetMaterial(int _Index)

### godot::Dictionary godot::Dictionary VCoreGDExt::VModel::GetVoxel(const godot::Vector3i &_Pos)

### void void VCoreGDExt::VModel::SetVoxel(const godot::Vector3i &_Pos, int _MaterialIdx, const godot::Color &_Color)

### void void VCoreGDExt::VModel::RemoveVoxel(const godot::Vector3i &_Pos)

### void void VCoreGDExt::VModel::Clear()

> Clears all voxels.

### size_t size_t VCoreGDExt::VModel::GetBlockCount()

### VCore::VoxelModel VCore::VoxelModel VCoreGDExt::VModel::GetModel()

> For internal use only!

