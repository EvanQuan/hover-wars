# CPSC 585 - Games Programming - Winter 2019

![](https://raw.githubusercontent.com/wiki/EvanQuan/qvim/title_art.jpg)

## [Documentation](https://drive.google.com/drive/folders/1bLh6WpMEgnnPTKAaAV5UKnIS_vkfy4ZO)

[General Notes](https://docs.google.com/document/d/1ALfsOyqVOHzWtrsITFJuJigNnoELT_yf2ml7x30rzuU/edit?usp=sharing)

[High-Concept Design Document](https://docs.google.com/document/d/1aplep-cNz6xHb8NYZChN0oXfnQwkUXKJKouJS7nf4Pw/edit?usp=sharing)

[Feature List](https://docs.google.com/document/d/1w7jZ_TSR2iCMgQbV4RhlWDOfbr-e_QE9J5EHTGxNIUQ/edit?usp=sharing)

[Schedule](https://docs.google.com/document/d/1QfO-LkLsXTUplEyc8ZsQTktRVn2b2XgmmaUhAyBSjyQ/edit?usp=sharing)

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

When the program is run, a small test environment is loaded up. There is a textured plane with diffuse lighting, a Car with toon shading, a textured Sphere with Diffuse lighting and a Cube that's meant to represent the starting position of the light along with the color of the light. Although the light moves with the above-mentioned controls, the cube does not move.

This Scene is loaded with the Scene2.txt file using the entity component framework and loaded with the Scene Loader class as per the Framework documentation.

## Team

[Austin Easton](https://github.com/austinen)

[Evan Quan](https://github.com/EvanQuan)

[James Coté](https://github.com/jamescote)

[Jianan Ding](https://github.com/jiananding)
