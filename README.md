# CPSC 585 - Games Programming - Winter 2019

## Compilation

### Compiling on Windows:

**Compiled using MSVS 2017.**

1. The Libraries and Settings should all be set up in the VS project.

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

**B** - Toggle Bounding Box rendering

**M** - Toggle Spatial Map Debug rendering

**Keypad 0** - Set User Interface display count to 0

**Keypad 1** - Set User Interface display count to 1

**Keypad 2** - Set User Interface display count to 2

**Keypad 3** - Set User Interface display count to 3

**Keypad 4** - Set User Interface display count to 4

**R** - Player 1 hits Player 2

**T** - Player 2 hits Player 1

#### XBOX Controller

**Left Joystick** - Applies a force on the vehicle in that direction on the xz-plane, WRT to Vehicle space.

**Right Joystick** - Rotates the vehicle left or right.

**Right Trigger** - Press to fire rocket

**Right Bumper** - Hold to switch to alternate camera

**Left Trigger** - Hold to activate flame trail

**Left Bumper** - Press to activate spikes

**X, Y, A, B** - Directional dash

**D-pad** - Honk

## Write-Up:

Since Milestone 2:

### Font Rendering

The TTF file for the font is parsed using FreeType and saved into an internal
bitmap layout of all the valid characters for rendering text. Then each glyph
information (uv offset/size, bearing, advance) is stored in a hashmap for
indexing into the bitmap. This allows us to render full lines of text with
1 draw call as opposed to having a texture for each glyph and a draw call for
each character. 

### Spatial Data Map
There's a basic Spatial Data Map in place that is intended to be utilized for
pathfinding and other necessities. On scene load, all the entities are given
to the spatial data map who populates a static map as well as a hashtable
listing the spatial parameters for each entity in the scene. Dynamic Entities
are updated as they move while static entities are left untouched within the
static data map. While running in debug mode, you can view a visual
representation of the spatial data structure by pressing 'm'. This rendering
is rather expensive and is only available in debug mode. The color codes are
as follows:

- Red: This represents a Static Entity. Path-finding will probably treat these
  spaces as unpassable terrain for finding a path to their target.
- White: This represents the Space a Point Light covers. I had intended to
  speed up lighting calculations by only using lights that the entity is near.
  This is currently not the case, but it may be revisited in the future.
- Purple: This represents a Spot Light. We had a vision for the aesthetic to
  be gritty and cyber-punky. We wanted Spotlights to cast shadows as dynamic
  entities crossed under them. In order to limit shadow calculations, I wanted
  to have a default shadow map for each light and only calculate shadows for
  spotlights that have a dynamic entity within their space. This may not make
  it to release.
- Jade: This represents a dynamic entity. Dynamic Entities cover multiple
  spaces based on their spatial dimensions and it's important that they stay
  updated every frame.

### Shadow Mapping

The directional light in the scene casts shadows. The shadow map is specified
in the scene that loaded in under the directional light entity entry. The
texture size for the shadow is rather large to avoid overly pixillated
shadows. Also, a smoothing kernel is applied to the shadows to soften their
edges and blend with the fragment colors in a nicer way.

### Game Rules

**Abilities**

Rocket

- Implemented
    - Controls
    - Cooldown
    - Sound
    - Internal collision detection
    - Collision processing
- Not implemented
    - Physics
    - Rendering

Flame Trail

- Implemented
    - Controls
    - Fuel gauge
    - Graphics
    - Sound
    - Internal collision detection
    - Collision processing
- Not implemented
    - Physics collision detection

Spikes

- Implemented
    - Controls
    - Cooldown
    - Sound
    - Internal collision detection
    - Collision processing
    - Physics collision detection
- Not implemented
    - Graphics

Directional dash

- Fully implemented

Honk

- Fully implemented

Alternate camera

- Fully implemented


**Stats**
- All in game stat tracking is fully implemented in order for the game to run

```
Player:
    Score:
        Current score
        Change in score
        Total score
    Kills:
        Total kills
        Total kills against players
        Total kills against each player
        Total kills against bots
    Dominations:
        Current dominations between players
    Killstreaks:
        Current total killstreak
        Current killstreak between players
        Largest total killstreak
        Largest total killstreak between players
    Powerups:
        Total powerups picked up
    Abilities:
        Total abilities used
        Count of each ability used
```

To test this, run in `Debug` mode and press:


**R** - Player 1 hits Player 2

**T** - Player 2 hits Player 1

This demonstrates how players gain and lose points based on their current
killstreaks, dominations and revenges.


Domination
- If player A kills player B 3 times in a row without being killed by player
  B, player A will `dominate` player B.

Revenge
- If player A is dominating player B, and player B kills player A, player
  B gets `revenge` on player A. Player A no longer dominates player B.

Killstreak
- Number of kills a player has in a row without being killed by anyone

Points are tracked as as followed (are open to change):

Base points gained for killing a bot (10)

Base points gained for killing a player (50)

Points gained for getting revenge 100

Points gained per killstreak 20
- This stacks

Points gained for picking up a powerup 10

Base points lost for getting killed 30

Points lost per killstreak 10
- This stacks



**Game Time**
- Implemented
    - The round timer tracks time correctly
- Not implemented
    - The round ends when the timer hits 0:00


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

[FreeType](https://www.freetype.org/)

[FMod](https://www.fmod.com/)

[GLFW 3.2.1](https://www.glfw.org/)

[GLM 0.9.9](https://glm.g-truc.net/0.9.9/index.html)

[Physx](https://www.geforce.com/hardware/technology/physx)
