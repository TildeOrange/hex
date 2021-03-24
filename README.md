# Hex
A hexagonal-grid world generator

## What is this?
This is a prototype (if you couldn't tell by the low-quality code and half-~assed~...written comments) island generation thingy. It creates a hexagonal island from hexagonal-tiles. The "biomes" are generated like a voronoi diagram. Eventually, I plan to layer perlin noise on top of this to create hills and mountains.

## How to build.
If you're on windows, use the MSVC compiler (I used the x64 version) and run build.bat. It is very important you have the right libraries. I use static x64 versions of GLFW, GLEW, and STB. Currently, the STB lib is not used so it can be omitted. These libraries should be placed in lib\binaries\debug and their include files in lib\include

This program is theoretically cross platform, but I have not tested this so you have been warned.

## How to run.
First, make sure the working directory has the res folder.

Secondly, use these command options:
- -display_size or -d (i.e. -d *width*x*height* (must be integers)) (defaults to 1600x900)
- -fov or -f (i.e. -f *fov* (can be a decimal)) (defaults to 90 deg)
- -radius or -r (i.g. the world radius) (i.e. -r *radius* (must be an integer)) (defaults to 128)
- -seed or -s (i.g. the world generation seed) (i.e. -s *seed* (must be an integer)) (defaults to 48)

Warning: The display **cannot** be resized so pick an appropriate resolution for your monitor.

While running, the camera can be rotated by moving the mouse or using the direction keys and translated with the W, A, S, and D keys. To toggle the mouse capture, press escape (if the mouse is free, then the movement is turned off).

## Todo
- Different meshes for tile types. This is partially implemented (the grass mesh is seperate but identical to the sand and ocean mesh), but I haven't put the time into making anything.
- Terrain Height
- Textures
- Level Of Detail. A few more polygons per meshes tend to kill the FPS (especially with a 256 wide world).
- Fustrum Culling. There needs to be a major rewrite to how the tile instances are passed to the shaders. Right now, I would have to copy all the data associated with a tile (12 bytes) to a drawing buffer. Ideally, I could just write down the indices of all visible tiles, but this might require using a shader-storage buffer or something similar.
- Rivers! This is 80% of why I started this project.
- Trees! This is 20% of why I started this project.
