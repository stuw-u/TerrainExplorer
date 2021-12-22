# TerrainExplorer
A terrain visualizer tool made in C++ using OpenGL (Glad) and GLFW.
Create your own noise context, change the context being used in ChunkDataGenerator.cpp in the GridDataPass function and visualize what the noise function looks like!

![The YosemiteValley noise context being visualize](https://cdn.discordapp.com/attachments/651607986661228599/922299358303629342/unknown.png)

Keep in mind that this is my first non-arduino c++ project, the code may be messy.
My goal for this project was to write as much code as I could on my own without copying anything 1:1.

## Controls
\[WASD] to move, \[Cursor] to aim and \[Space] to increase your speed

## Known Issues
- Changing the chunk generation bound to a large value isn't ideal. The world system isn't multithreaded, it was only moved to a different thread than the main game to prevent it from stuttering the framerate. If the bound is too large, you'll need to stop for a moment while the terrain data gets generated before seeing the terrain appear.
- The game uses trilinear sampling to avoid sampling the noise functions in every cell of every chunk. This technique, altought fast, leave multiple artefacts that reveal the underlying "grid" that makes up the world.  

## Future improvements
- Implement a biome system that blends different noise context toghether.
- Make the terrain generation multithreaded, give priority to the chunks closer to the player, and make sure the grid data generation doesn't prevent the mesh from being generated in parrallel.
- Make the noise system data oriented instead of requiring to manually edit the code
- Implement water and transparent meshes
- Implement structure generation
- Implement grass generation

## Ressources

### Ambient occlusion
https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/

### Biome blending (Unused)
https://github.com/KdotJPG/Scattered-Biome-Blender

### Noise generation
https://github.com/Auburn/FastNoiseLite

### Frustrum culling
https://github.com/Hopson97/HopsonCraft/blob/master/Source/Maths/Frustum.cpp

### Where I started to learn c++
https://youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb
