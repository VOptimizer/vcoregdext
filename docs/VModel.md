# Class: VModel

> A voxel model represents a collection of voxels, textures and materials.

### String get_name() const

### void set_name(String _Name)

### AABB get_b_box() const

#### Return:

> Returns the bounding box of a voxel model inside the voxel grid.

### int add_material( VMaterial  _Material)

> Must be called for each new material to be used in the model. The default material has the index 0.

###  VMaterial get_material(int _Index)

#### Return:

> Gets the reference to a voxel material by its index.

### Dictionary get_voxel(Vector3i _Pos)

#### Return:

> Gets a voxel a a given position. An empty dictionary means air, otherwise the format is as follows {"color": Color, "material": VMaterial, "transparent": bool}

### void set_voxel(Vector3i _Pos, int _MaterialIdx, Color _Color)

> Sets a voxel at a given position. If _MaterialIdx is invalid, the material will be fallback to the default one.

### void remove_voxel(Vector3i _Pos)

> Removes a voxel on a given position.

### void clear()

> Clears all voxels.

### uint64_t get_block_count()

### VoxelModel get_model()

> For internal use only!

