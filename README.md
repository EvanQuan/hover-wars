# CPSC 585 - Games Programming - Winter 2019

## [Documentation](https://drive.google.com/drive/folders/1bLh6WpMEgnnPTKAaAV5UKnIS_vkfy4ZO)

[General Notes](https://docs.google.com/document/d/1ALfsOyqVOHzWtrsITFJuJigNnoELT_yf2ml7x30rzuU/edit?usp=sharing)

[High-Concept Design Document](https://docs.google.com/document/d/1aplep-cNz6xHb8NYZChN0oXfnQwkUXKJKouJS7nf4Pw/edit?usp=sharing)

[Feature List](https://docs.google.com/document/d/1w7jZ_TSR2iCMgQbV4RhlWDOfbr-e_QE9J5EHTGxNIUQ/edit?usp=sharing)

[Schedule](https://docs.google.com/document/d/1QfO-LkLsXTUplEyc8ZsQTktRVn2b2XgmmaUhAyBSjyQ/edit?usp=sharing)

## Compilation

### Compiling on Windows:

**Compiled using MSVS 2015.**

1. You'll need to set up OpenGL and GLEW in the settings of the solution as
   required libraries.

### Compiling on Linux:

1. In the file EnvSpec.h: You'll need to switch the comment from Defining
   Windows to Defining Linux.

2. The included Makefile should generate a run file that will launch the
   program.  Ensure that this is executed through the command line in order to
   provide command input functionality of the program.

## Controls:

**P** - Pauses the Animation

**Right-Click** and Move to Rotate

**Scroll-Wheel** to Zoom

**F** - Switch to Wireframe Mode

## Write-Up:

Basic Boid Implementation with Cohesion, Gather and Avoid logic. There's
a Spherical Boundary with a defined width that is used to interpolate
a pushback function that's similar to avoid (the closer to the border, the
harder it forces the boid back).

In the file: "Scene2.txt" there are some settings that can be modified for the
Boids, particularly:

Indices:
```
0 - Num_Boids
1 - Avoid Radius
2 - Cohesion Radius
3 - Gather Radius
4 - Min Speed (length)
5 - Max Speed (length)
6 - Boundary Radius
7 - Delta T
8 - Scale
9 - Texture Location (Optional)
10 - Mesh Location (Optional)
```

The update also does a defined loop of update/integration of 16 iterations.
This smoothes out the animation with a smaller delta_t. I implemented the
screenspace division algorithm as well. This make it much faster for lots of
boids before they group up. Once they group up, then they interact with one
another and it sort of negates the benefit of the structure.


## Team

[Austin Easton](https://github.com/austinen)

[Evan Quan](https://github.com/EvanQuan)

[James Coté](https://github.com/jamescote)

[Jianan Ding](https://github.com/jiananding)
