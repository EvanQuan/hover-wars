Compiling on Windows:
Compiled using MSVS 2015.  Will add one for 2013 as well in the future.  
You'll need to set up OpenGL, GLEW, and trimesh.lib in the settings of the solution as required libraries.

Compiling on Linux:
In the file EnvSpec.h: You'll need to switch the comment from Defining Windows to Defining Linux.
The included Makefile should generate a run file that will launch the program.  Ensure that this is executed through the command line in order to provide command input functionality of the program.

Controls:
WASD - controls the position of the light along the XZ-axis
Space and X - moves the light along the Y-axis; up and down respectively
Mouse Wheel - Zoom
Right-Click + Drag - Rotate around look-at position
C - swaps between camera modes: Spherical (Viewing World Coord (0.0, 0.0, 0.0)), Spherical - Following Cart and Positioned behind the cart.
P - Pauses animation

