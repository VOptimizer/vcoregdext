# Class: VCoreGDExt::VLoader

> The loader class allows you to load all supported voxel formats. For a complete list of all supported formats, please visit https://github.com/VOptimizer/VCore/tree/master/Docs/Voxelformats.

### godot::Error godot::Error VCoreGDExt::VLoader::Load(const godot::String &_Path)

> Loads a given file.

#### Return:

> Returns either OK on success or an error code otherwise.

### godot::Ref<  godot::Ref< VSceneNode > VCoreGDExt::VLoader::GetSceneTree() const

#### Return:

> Returns the scenetree of the file.

### godot::Array godot::Array VCoreGDExt::VLoader::GetModels() const

#### Return:

> Returns an array of VModels of the last loaded file.

