# Forest Dash

## Asigment for our videogames development subject.

## Webpage
### https://ap011y0n.github.io/2d-Platformer/
## Repository
### https://github.com/Ap011y0n/2d-Platformer

## Team

### Albert Garcia Belerda - https://github.com/Ap011y0n
#### 0.5: Physics, load things from Tiled, cleaning code
#### 0.8: Entities, framerate cap
#### 1.0: UI base class, Console, Bugfixing
### Javier D. Belda González - https://github.com/JBGelsenorJ
#### 0.5: Audio, death condition, Map 2, godmode, camera borders
#### 0.8: Pathfinding and enemy behaviour
#### 1.0: Pause menu, volume sliders, coins, webpage, ingame HUD (coins, score)
### Macià Dalmau Roig- https://github.com/maciadalmau
#### 0.5: Map 1, animations, sprites, shake, parallax, 
#### 0.8: Framerate cap, enemies animations and machine state, brofiler, bow animation, sfx
#### 1.0: Main menu(settings,credits...), ingame HUD (lifes,timer), atlas
## How to play
### Your objective is to reach the end of the level, but be careful, you can't be still for very much, there is a bar behind you that only regenerates while running (not jumping), also watch out for the dangers of your trip, such as enemies, spikes, death pits, deep underground lakes...

## Controls
### A: Backward movement
### D: Forward movement
### W: Jump
### S: Crouch (no need to use yet)
### Q/E arrow: Dash
### +/- up/down volume
### Left click melee attack
### Right click bow (need to be loaded)

## Debug
### F1: First level from start
### F2: Second level from start
### F3: Start of the current level
### F5: Save the current state
### F6: Load the previous state
### F8: Show UI rects
### F9: Show colliders
### F10: God mode
### F11: framerate cap / no cap

## Console
### Open with º
### godmode --> Enables/disables player godmode"
### list --> If you haven't noticed yet, displays all available commands"
### fps <number> --> sets a framerate cap between 30 and 120"
### map <number> --> changes map"
### Quit --> Closes the application"

## Changelog
### 0.5: 
#### Created 2 levels , added player with animations, music and fx selected and implemented, debug features added, time bar added, death/win condition implemented, dash added.
### 0.8:
#### Levels have been changed to implement more usage of the dash, now there are enemies that pathfind the player, the player can kill them with a melee attack or a range one, module entites implemented now
### 1.0
#### Main menu, pause menu, HUD, coins, console
#### the player and the rest of the entities derivate from this class, framerate now capped to 30 (you can desactivate this)
### Innovations: 
#### for 0.5: Animations loaded from Tiled, time bar, colliders made in Tiled, volume for music and fx can be manipulated and saved.
#### for 0.8: Bow animation depends of position of the mouse, checkpoints, enemies created from tmx
#### for 1.0: HUD and console, pause menu, mains menu and coins
### Cave Assets: "Warped" by Luis Zuno https://twitter.com/ansimuz?lang=es
### Forest Assets: "Forest game assets" by Anokolisa https://twitter.com/anokolisa
### Mountain background: "Taiga" by vnitti https://twitter.com/vnitti_art
### Player: "Animated Pixel Adventurer" by rvros https://rvros.itch.io/
### Background music thanks to: https://freesound.org/people/Sirkoto51/sounds/443128/
### Sound effects obtained from https://www.zapsplat.com
### Using the following SDL libraries: 
### SDL: https://www.libsdl.org/ 
### SDL_Image: https://www.libsdl.org/projects/SDL_image/ 
### SDL_Mixer: https://www.libsdl.org/projects/SDL_mixer/

## License 
### MIT License

### Copyright (c) 2019 Ap011y0n, JBGelsenorJ & maciadalmau

### Permission is hereby granted, free of charge, to any person obtaining a copy
### of this software and associated documentation files (the "Software"), to deal
### in the Software without restriction, including without limitation the rights
### to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
### copies of the Software, and to permit persons to whom the Software is
### furnished to do so, subject to the following conditions:

### The above copyright notice and this permission notice shall be included in all
### copies or substantial portions of the Software.

### THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
### IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
### FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
### AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
### LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
### OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
### SOFTWARE.
