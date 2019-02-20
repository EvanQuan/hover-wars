# CPSC 585 - Games Programming - Winter 2019

## Compilation

### Compiling on Windows:

**Compiled using MSVS 2017.**

1. The Libraries and Settings should all be set up in the VS project.

### Compiling on Linux:

**NOT TESTED**
1. In the file EnvSpec.h: You'll need to switch the comment from Defining
   Windows to Defining Linux.

2. The included Makefile should generate a run file that will launch the
   program.

## Controls

#### Keyboard

##### Movement

**W** - Move Forward

**A** - Move Left

**S** - Move Back

**D** - Move Right

**J** - Turn Left

**L** - Turn Right

##### Dash

**I** - Dash Forward

**H** - Dash Left

**K** - Dash Back

**;** - Dash Right

##### Abilities

**Space** - Rocket

**Left Shift** - Trail

**'** - Spikes

#### Other

**P** - Pause the game


**Left Click** - Spawns a particle Emitter at the mouse intersection with the xz-plane @ yAxis = 0

**Right Click** - Holding down Right Click and moving the mouse will allow you to adjust the camera around the vehicle

#### Debug Commands (for Debug mode only)

##### Switching Keyboard Player

**1** - Player 1

**2** - Player 2

**3** - Player 3

**4** - Player 4

**F** - Toggle Wireframe Mode

**C** - Toggle Debug Camera for current player

**Keypad 0** - Set User Interface display count to 0

**Keypad 1** - Set User Interface display count to 1

**Keypad 2** - Set User Interface display count to 2

**Keypad 3** - Set User Interface display count to 3

**Keypad 4** - Set User Interface display count to 4

#### XBOX Controller

Currently, basic driving is implemented. The controls are:

**Left Joystick** - Applies a force on the vehicle in that direction on the xz-plane, WRT to Vehicle space.

**Right Joystick** - Rotates the vehicle left or right.

**RT** - Drops a particle emitter at the vehicle's current location until released.

## Write-Up:

Objects in the world now have Materials attached to their meshes. These Materials consists of a diffuse map, a specular map or shade and a shininess value. Full lighting is implemented, including Directional Lighting, Point Lighting and Spot Lighting. The current maximum set to be rendered in the scene is 1 Directional Light, 4 Point Lights and 4 Spot Lights; these will probably increase later on.

When you load up the program, you'll see a textured plane with a white spot light just overhead of the origin. The World Axis is rendered at the origin for reference/testing purposes and 3 point lights are rendered as Axis parameters as well. You'll see quite a few bunnies in the positive xzy quadrant that are testing the static instanced rendering. Holding the right mouse button down and moving the mouse will rotate the camera around the vehicle. If you look higher towards the bunnies, you'll also see a billboard testing object. It renders locked to the up normal vector and facing the camera. It uses the current default texture settings.

The world loads in from the file scene2.scene and can be modified as per the rules stated at the top of that file.

## Team

[Austin Easton](https://github.com/austinen)

[Evan Quan](https://github.com/EvanQuan)

[James Coté](https://github.com/jamescote)

[Jianan Ding](https://github.com/jiananding)

## Libraries

[OpenGL]

[FTGL 2.1.3~rc5](https://sourceforge.net/projects/ftgl/)

[GLFW 3.2.1](https://www.glfw.org/)
