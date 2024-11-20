# XPBD

Project for IG3DA

# Features
- Edit and save scenes
- Switch from rasterizer to raytracer
- Spheres, Torus, and any shape with triangles

# Controls
- Press SPACE to toggle Raytracing
- Press P to take a screenshot
- Scroll to zoom in/out
- Use mouse to turn around subject

# TODO
- Add a triangle mesh for torus (they appear as circles in the rasterizer)
- Fix flickering at the bottom of the image with some scenes

## First build

```
mkdir build
cmake -B build
make -C build
./Raytracing
```

## Dependencies

- Dear ImGUI: https://github.com/ocornut/imgui
- glfw: https://github.com/glfw/glfw
- glm: https://github.com/g-truc/glm
- glad: https://gen.glad.sh/

You may need to generate your glad files for your machine.

# Gallery
![demo](<data/output/JO boite reflet.png>)
![demo](<data/output/JO full reflets.png>)
![demo](<data/output/balls.png>)

