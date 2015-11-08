SUPER MARIO 2D

INTRODUCTION

The project started from “Example_Invaderers” in OCTET that was modified to become a simple 2D game inspired by Nintendo’s Super Mario Bros.
The game is in 2 levels (level 1 and boss battle) with the same layout. Mario can move in all four directions like in the classic “Legend of Zelda” game for the NES. In the first level Mario has to avoid 20  Goombas enemies arranged in rows of five across the screen and moving fast left and right. Mario has to collect the mushroom and then the fire-flower from the mystery blocks in order to be able to shoot and destroy the goombas. Once all the goombas are destroyed Bowser appears shooting fireballs and moving Up and Down. The same mechanic repeats and once Mario has hit Bowser 20 times, defeating him , Princess Peach appears and once she is re-united with Mario the game ends. 

DEVELOPMENT

(for code details please check example_invaderers on github/eliodeberardinis on the branch Intro_programming_ass_1)

The first step was to modify the “move_ship()” function to be able to move the character in all four directions (LEFT, RIGHT, UP, DOWN).
Then, in order to make things look more realistic, the LEFT and RIGHT directions were modified so that the character’s sprite faces the direction it is moving. This was achieved by rotating the sprite along the Y-Axis with the function rotate(…).

Next, CSV files were implemented to easily draw the map’s borders, background and objects (mushroom, fire-flower, blocks) that the character interacts with. After noticing the gameplay window was set to [750 X 750]pxl and that the camera was positioned 3 units away, the gameplay window was theoretically divided in [20 X 20] tiles each being a [0.3 X 0.3] square. Two 2D arrays, size [20 X 20] of type int (called map and map2) were then created to store the values read from the CSV files for the background/borders and the objects respectively. Subsequently, three “dynarray”s of type “sprite” were created to store information on the position and texture of the background (map_sprites_dirt), borders (map_sprites_bush) and objects (object_sprites). Two functions (read_csv() and read_csv2()) were created to read the two CSV files based on the “Read_csv_file” example in OCTET and store the information in the arrays. Finally, two more functions were created (setup_visual_map() and setup_object_map()) to initialize the tiles of background/borders and objects respectively and store the information in the dynarrays.

These four functions are called in the function app_init() when the program is initialized. Everything is then rendered in the draw_world(…) function using the render method of texture_shader.h already present in OCTET except for the background tiles (stored in the dynarray “map_sprites_dirt”) that were rendered with a custom made shader and render method present in elio_shader.h also initialized in app_init() and included in shaders.h.

The custom-made shader (elio_shader.h) was created inheriting from OCTET’s shader class and consists of an unmodified vertex_shader, a custom-made fragment_shader and a render method that simply colours the rendered tiles according to RGB values. An effect was added to change the colour intensity along the Y axis (not implemented in the game). An extra variable (vec4 color) can be passed to the render method to change the colour according to some game events (The background colour changes at the boss stage and at end game).

With the CSV files and the new shader it is easy to draw the level, colour the background and optimize the position of each object.

Collisions needed to be tailored to fit the new method of creating borders. For this reason the function move_ship() was further modified in each direction by adding simple IF statements that would stop Mario (by translating it of a velocity of the same magnitude but opposite sign) in case of collision with borders or specific objects (item blocks). In case of collision with an item block while the “up-arrow” key is pressed, a different function was created (blocks_pressed(), called inside move_ship()) to simulate the action of pressing the item block and activating the item they contain.

The mushroom item is animated with the function move_mushroom(), and its collisions are checked by the mushroom_collide() function called in simulate(). The enemies (Goombas) collisions are checked by invaders_collide() function (also called in simulate()) where the enemies are animated by rotating their sprites each time they collide with a border.

The boss (Bowser) was animated with move_boss() function and its collisions checked and implemented with boss_collide function called in simulate().

Interactions of the main character (Mario) with the objects (mushroom, flower) are handled by the function objects_collide() (called in move_ship()) where Mario is scaled up, change sprite or gains the ability to shoot fireballs. Mario is easily scaled on the spot with the help of get_position() function. Interactions are activated/deactivated with the use of a boolean type variable (flower_picked) and the size of the main character (mario_height) to take into account the collision with the flower item or enemies.

Collision with the enemies (Goombas or Bowser) are handled by the collide_with_enemy() function where Mario is scaled down, looses a life and respawns by calling the modified on_hit_ship() function.

The functions move_missiles() and fire_missiles() have been modified in order to activate the fireballs only when the flower item has been obtained and to shoot along the x-axis rather than the y-axis.

The functions move_bomb() and fire_bombs() were both modified to be activated only at the boss stage, attached to the boss sprite and shoot along the x-axis. The function move_bomb() calls on_hit_ship() in case of a collision with Mario. Collisions of missiles with bombs were also taken into account in the move_bomb() function.

The function princess_met() activates the end-game scenario once Mario collides with the princess sprite that appears if the boss has been beaten.

Finally, relevant sound effects and sprites were added to the game.

CREDITS
Jean-Pascal for helping with the move_ship function and adding sprites rotation.
Mircea for helping with CSV file, shader implementation and solving many small problems here and there.
Tin-Tin for helping with get_position function.

DEMO VIDEO:

https://youtu.be/mHgXu78f3hg

Images:

![Alt text](https://github.com/eliodeberardinis/octet/blob/try_border_grass/octet/assets/invaderers/Game_Screen_1.png?raw=true"Screen 1")

![Alt text](https://github.com/eliodeberardinis/octet/blob/try_border_grass/octet/assets/invaderers/Game_Screen_2.png?raw=true"Screen 2")

![Alt text](https://github.com/eliodeberardinis/octet/blob/try_border_grass/octet/assets/invaderers/Game_Screen_3.png?raw=true"Screen 3")

![Alt text](https://github.com/eliodeberardinis/octet/blob/try_border_grass/octet/assets/invaderers/Game_Screen_4.png?raw=true"Screen 4")


