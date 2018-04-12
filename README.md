Compiling on Windows:
Compiled using MSVS 2015.  Will add one for 2013 as well in the future.  
You'll need to set up OpenGL, GLEW, and trimesh.lib in the settings of the solution as required libraries.

Compiling on Linux:
In the file EnvSpec.h: You'll need to switch the comment from Defining Windows to Defining Linux.
The included Makefile should generate a run file that will launch the program.  Ensure that this is executed through the command line in order to provide command input functionality of the program.

Controls:
P - Pauses the Animation
Right-Click and Move to Rotate
Scroll-Wheel to Zoom
F - Switch to Wireframe Mode

Write-Up:
Basic Boid Implementation with Cohesion, Gather and Avoid logic. There's a Spherical Boundary with a defined width that is used to interpolate a pushback function that's similar to avoid (the close to the border, the harder it forces the boid back).

In the file: "Scene2.txt" there are some settings that can be modified for the Boids, particularly:
Indices:
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
		
The update also does a defined loop of update/integration of 16 iterations. This smoothes out the animation with a smaller delta_t. I implemented the screenspace division algorithm as well. This make it much faster for lots of boids before they group up. Once they group up, then they interact with one another and it sort of negates the benefit of the structure.
