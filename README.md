# :video_game: Hover Wars



<!-- ![](title-image) -->

## Download and Running

[You can find the most recent release
here](https://github.com/EvanQuan/hover-wars/releases).

1. Download the `zip` or `tar.gz` and extract it.
2. Run `HoverWars.exe` to start the program.

#### Compiling on Windows:

**Compiled using MSVS 2017.**

1. The Libraries and Settings should all be set up in the VS project.

## Controls

The game supports both keyboard and controller support, although it is highly
recommended to use a controller.

##### Keyboard

<details>
<summary>Click here to show.</summary>

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

**Left Click** - Spawns a particle Emitter at the mouse intersection with the
             xz-plane @ yAxis = 0

**Right Click** - Holding down Right Click and moving the mouse will allow you
              to adjust the camera around the vehicle

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

</details>

##### XBOX/PlayStation Controller

<details>
<summary>Click here to show.</summary>

**Left Joystick** - Applies a force on the vehicle in that direction on the
                    xz-plane, WRT to Vehicle space.

**Right Joystick** - Rotates the vehicle left or right.

**Right Trigger** - Press to fire rocket

**Right Bumper** - Hold to switch to alternate camera

**Left Trigger** - Hold to activate flame trail

**Left Bumper** - Press to activate spikes

**X, Y, A, B** - Directional dash

**D-pad** - Honk

</details>

## Write-Up:

##### Since Milestone 4:

<details>
<summary>Click here to show.</summary>
</details>

##### Since Milestone 3:

<details>
<summary>Click here to show.</summary>

### Game Rules

First blood - the first hit of the game grants extra points. This incentivizes
more aggressive play at the start.

A clarification from Milestone 3: there are no deaths and respawns. Our system
focuses on gathering points by hitting other players with abilities like
Mario Kart's battle system.
To punish getting hit, we tried:
- **respawn on hit** - we found it very disorienting having to constantly
  respawn to a random respawn point every time you got hit and having to
  regain your bearings. We suspect it would also be disorienting to see
  hovercrafts spawn near you without warning.
- **losing control for a short duration** - this caused a problem with spikes,
  as you could stun-lock another player by constantly ramming into them and
  spamming spikes. This was partially alleviated by applying a force to
  hovercrafts as they collide with one another, but it's still bad when
  multiple bots are teaming up against a single player.

We settled for resetting ability cooldowns, and losing more points based on
killstreak (explanation next).

### Powerups

#### Timed

Whenever a player hits the player with the most points, they are given a speed
most powerup, which lasts for 20 seconds.

A speed boost is also given to first blood.

#### Stacking

Ability cool downs have been rebalanced to have a much longer time than
before.

Rocket - 5s

Spikes - 3s

Dash - 3s

Whenever players increase their killstreak, their cool downs shorten
by 10% up to a minimum cool down value:

Rocket - 1s

Spikes - 1s

Dash - 0.5s

If their killstreak is lost, their cool downs reset to their base values.

This awards more defensive play and incentives players to put more effort into
dodging enemy attacks. Getting hit now gives a much stronger "feels bad"
response, especially after a long killstreak.

### Ending the Game

The game ends when the game timer hits 0. The player with the most points
wins.

#### End Game Awards

At the end of the game, players are given certain awards for accomplishing
certain tasks, gaining extra bonus points. The currently implemented awards
are:

**Gotta Go Fast!** - Most speed boosts +200

**Tactical** - Largest killstreak +200

**Rocket Man** - Most rocket kills +100

**Pyromaniac** - Most flame trail kills +100

**Porcupine** - Most spike trail kills +100

**Consolation** - Most deaths +100

If there are is more than 1 human player and more than 1 bot in the game,
these awards are available:

**Ludite** - Most bot kills +200

**Misanthropist** - Most player kills +200

#### Points

While still weaker than a player, bots are much challenging than initially
planned, and so now award the same number of points as hitting other human
players.

Points awarded to the attacker now increases with the killstreak of the player
they hit. This should make high killstreak players a larger priority to
target.

### Menus

The game now has a menu system instead of starting directly in-game.

**Start Menu** - the initial splash screen when starting the program, akin to
many old arcade games. Pressing accept will move to the Main Menu.

**Main Menu** - allows the player to start a new game or quit the program.
They can go back to the Start Menu.

**Pregame Menu** - Lets the user choose the game parameters before starting
the game. The user can choose the number of players, number of bots, and game
round time before choosing to start the game, which goes to the Game Menu.
They can also go back to the Main Menu.

**Game Menu** - This is the in-game behaviour as seen before. The player can
press Start to go to the Pause Menu.

**Pause Menu** - The game pauses. The user can continue the game, or end the
game, which goes to the post-game menu.

**Postgame Menu** - After the game, all the end game awards are assigned, and
all the points are tallied up to determine the round winner. This then leads
to quitting the program.

### User Interface

Speed boost prompt displays for the duration of the player's speed boost.

### Sound

Entering the pause menu pauses all in-game sounds, and plays a pause music
loop. Returning to the Game Menu resumes playing in-game sounds as it left
off.

Moving the cursor, or selecting options in the menus plays a random cursor
sound.

Spikes make a sound when deactivating.

Rockets make a sound when firing and when impacting a surface.


### Abilities

Spikes
- Now render and are animated when they are activated and deactivated.

Rockets
- Physics and rendering are now complete

Flame Trail
- Now work with physics

### Driving model

- When hovercrafts collide, a force pushes them away. This makes it more
  difficult to constantly ram into another vehicle non-stop.

- Hovercraft centre of gravity has been lowered to make flipping more
  difficult and recovering from flipping easier.

</details>

##### Since Milestone 2:

<details>
<summary>Click here to show.</summary>

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
texture size for the shadow is rather large to avoid overly pixellated
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
    - Physics collision detection

Spikes

- Implemented
    - Controls
    - Cooldown
    - Sound
    - Internal collision detection
    - Collision processing
    - Physics collision detection
    - Graphics

Directional dash

- Fully implemented

Honk

- Fully implemented

Alternate camera

- Fully implemented


**Stats**
- All in game stat tracking is fully implemented in order for the game rules
  to work

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

**Points**

Points are tracked as as followed (are open to change):

Base points gained for killing a bot **10**

Base points gained for killing a player **50**
- It is more rewarding to hit another player than a bot

Points gained for getting the first kill of the round (first blood) **150**
- Incentivize aggressive play at the start of the round.

Points gained for getting revenge **100**
- This gives an incentive to players to seek revenge other other players
- Provides a means for players who are not performing too well to catch up,
  but only if they have the skill to perform that kill

Points gained per killstreak **20**
- This stacks
- This rewards players who can obtain a high killstreak

Points gained for picking up a powerup **10**
- Minor extra incentive to pick up powerups

Base points lost for getting killed **30**
- Should be less points than a kill, not too much

Points lost per killstreak **10**
- This stacks
- This makes it risky to those with highstreaks as they will also lose more
  points on death


**Game Time**
- Implemented
    - The round timer tracks time correctly
- Not implemented
    - The round ends when the timer hits 0:00
    - Once the round ends, the player with the highest score wins

### User Interface

- Implemented
  - Abilities statuses
  - Score
    - Score change
  - Game time
  - Domination, revenge, killstreak messages
- Not implemented
  - Abilities images


### Sound

Music
- Music transitions and loops

Collision sounds
- Different entities result in different collision sound events.
    - Hovercraft against hovercraft
    - Hovercraft against world
    - Rocket against hovercraft (implemented, but not testable)
    - Flame against hovercraft (implemented, but not testable)
    - Spikes against hovercraft
    - Hovercraft and powerup (not implemented)
- Sound events can randomize between a pool of available sound for that sound
  event to increase variance. Random sound pools implemented for.
    - Hovercraft against hovercraft
    - Hovercraft against world
    - Spike activation

- First blood
    - First blood sound plays when the first kill is made

- Killstreaks
    - Killstreak sound plays when players reaches a high killstreak

- Domination/Revenge
    - Domination and revenge sounds play when players dominate another player
      or get revenge

- AI
    - The AI works on a basic follow model.
    - It oscillates between following the player and seeking a random point on
      the map.
    - The goal of this AI is to collide with the player with spikes engaged.
    - This will result in the AI gaining a point.

</details>

## Team

[Austin Eaton](https://github.com/austinen)

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
