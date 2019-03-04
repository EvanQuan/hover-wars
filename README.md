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

**Left Control** - Spikes

#### Other

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

**R** - Player 1 hits player 2

**T** - Player 2 hits player 1

#### XBOX Controller

**Left Joystick** - Applies a force on the vehicle in that direction on the xz-plane, WRT to Vehicle space.

**Right Joystick** - Rotates the vehicle left or right.

**Right Trigger** - Press to fire rocket

- Implemented
    - Controls
    - Cooldown
    - Sound
    - Internal collision detection
    - Collision processing
- Not implemented
    - Physics
    - Rendering

**Right Bumper** - Hold to switch to alternate camera

- Fully implemented

**Left Trigger** - Hold to activate flame trail

- Implemented
    - Controls
    - Fuel gauge
    - Graphics
    - Sound
    - Internal collision detection
    - Collision processing
- Not implemented
    - Physics collision detection

**Left Bumper** - Press to activate spikes

- Implemented
    - Controls
    - Cooldown
    - Sound
    - Internal collision detection
    - Collision processing
    - Physics collision detection
- Not implemented
    - Graphics

**X, Y, A, B** - Directional dash

- Fully implemented

**D-pad** - Honk

- Fully implemented

## Write-Up:

Objects in the world now have Materials attached to their meshes. These Materials consists of a diffuse map, a specular map or shade and a shininess value. Full lighting is implemented, including Directional Lighting, Point Lighting and Spot Lighting. The current maximum set to be rendered in the scene is 1 Directional Light, 4 Point Lights and 4 Spot Lights; these will probably increase later on.

When you load up the program, you'll see a textured plane with a white spot light just overhead of the origin. The World Axis is rendered at the origin for reference/testing purposes and 3 point lights are rendered as Axis parameters as well. You'll see quite a few bunnies in the positive xzy quadrant that are testing the static instanced rendering. Holding the right mouse button down and moving the mouse will rotate the camera around the vehicle. If you look higher towards the bunnies, you'll also see a billboard testing object. It renders locked to the up normal vector and facing the camera. It uses the current default texture settings.

The world loads in from the file scene2.scene and can be modified as per the rules stated at the top of that file.


### Sound

Music
- Music transitions and loops

Collision sounds
- Different entities result in different collision sound events.
    - Hovercraft against hovercraft
    - Hovecraft against world
    - Rocket against hovercraft (implemented, but not testable)
    - Flame against hovercraft (implemented, but not testable)
    - Spikes against hovercraft
    - Hovercraft and powerup (not implemented)
- Sound events can randomize between a pool of available sound for that sound event to increase variance. Random sound pools implemented for.
    - Hovercraft against hovercraft
    - Hovercraft against world
    - Spike activation

- Killstreaks
    - Killstreak sound plays when players reaches a high killstreak
    
- Domination/Revenge
    - Domination and revenge sounds play when players dominate another player or get revenge

## Team

[Austin Easton](https://github.com/austinen)

[Evan Quan](https://github.com/EvanQuan)

[James Coté](https://github.com/jamescote)

[Jianan Ding](https://github.com/jiananding)

## Libraries

[OpenGL 3.2.1](https://www.opengl.org/)

[FTGL 2.1.3~rc5](https://sourceforge.net/projects/ftgl/)

[FMod](https://www.fmod.com/)

[GLFW 3.2.1](https://www.glfw.org/)

[GLM 0.9.9](https://glm.g-truc.net/0.9.9/index.html)
