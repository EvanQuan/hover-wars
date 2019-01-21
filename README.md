# CPSC 585 - Games Programming - Winter 2019

## Compilation

### Compiling on Windows:

**Compiled using MSVS 2017.**

1. The Libraries and Settings should all be set up in the VS project.

### Compiling on Linux:

1. In the file EnvSpec.h: You'll need to switch the comment from Defining
   Windows to Defining Linux.

2. The included Makefile should generate a run file that will launch the
   program.

## Controls:

**P** - Pauses the Animation

**Right-Click** and Move to Rotate

**Scroll-Wheel** to Zoom

**F** - Switch to Wireframe Mode

**WASDX & SPACE** - Will move the light around the scene:

   **WASD**       - Along the XZ-plane

   **X & SPACE**  - Along the Y-Axis

## Write-Up:

When the program is run, a small test environment is loaded up. There is
a textured plane with diffuse lighting, a Car with toon shading, a textured
Sphere with Diffuse lighting and a Cube that's meant to represent the starting
position of the light along with the color of the light. Although the light
moves with the above-mentioned controls, the cube does not move.

This Scene is loaded with the Scene2.txt file using the entity component
framework and loaded with the Scene Loader class as per the Framework
documentation.

## Team

[Austin Easton](https://github.com/austinen)

[Evan Quan](https://github.com/EvanQuan)

[James Coté](https://github.com/jamescote)

[Jianan Ding](https://github.com/jiananding)
