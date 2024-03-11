# Class: VLoader

> The loader class allows you to load all supported voxel formats. For a complete list of all supported formats, please visit https://github.com/VOptimizer/VCore/tree/master/Docs/Voxelformats.

### Error load(String _Path)

> Loads a given file.

#### Return:

> Returns either OK on success or Error::ERR_CANT_OPEN on error.

###  VSceneNode get_scene_tree() const

#### Return:

> Returns the scenetree of the file. A scenetree represents the structure of the scene. In short on which position is which model.

### Array get_models() const

#### Return:

> Returns an array of VModels of the last loaded file.

