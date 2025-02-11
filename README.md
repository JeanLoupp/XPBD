# XPBD

Project for IG3DA class in Telecom Paris. A report can be found [here](Report.pdf).

# Features
- Rope, cloth, soft body simulation
- UI to change parameters

# Controls
- Use the mouse and left click to move the camera
- Right click to grab a particle
- Scroll to zoom in/out


## First build

```
mkdir build
cmake -B build
make -C build
./XPBD
```

## Dependencies

- Dear ImGUI: https://github.com/ocornut/imgui
- glfw: https://github.com/glfw/glfw
- glm: https://github.com/g-truc/glm
- glad: https://gen.glad.sh/
