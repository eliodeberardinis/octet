////////////////////////////////////////////////////////////////////////////////
//
// (C) Elio de Berardinis 2015
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Super Mario 2D example: First Assignment for Intro to Game Programming
// (developed starting from Andy Thomason's Invaderer's project in OCTET Framework, (C) Andy Thomason 2012-2015)
//
// Level: 1 + Boss Fight
//
// Demonstrates: 
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Simple game mechanics
//   Texture loaded from GIF file
//   Audio
//   CSV file use for borders and objects placing
//


namespace octet {
  class sprite {
    // where is our sprite (overkill for a 2D game!)
    mat4t modelToWorld;

    // half the width of the sprite
    float halfWidth;

    // half the height of the sprite
    float halfHeight;

    // what texture is on our sprite
    int texture;

    // true if this sprite is enabled.
    bool enabled;
  public:
    sprite() {
      texture = 0;
      enabled = true;
    }

	//Gets the position of a sprite
	vec2 get_Position() 
	{
		return modelToWorld.row(3).xy();
	}

	//Sets a position
	void set_Position(vec2 pos) {		

		modelToWorld.translate(vec3(pos.x(), pos.y(), 0.0f) - modelToWorld.row(3).xyz());

	}

	// Used to initialize sprites
    void init(int _texture, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      texture = _texture;
      enabled = true;
    }

	 //Render Method for texture_shader
    void render(texture_shader &shader, mat4t &cameraToWorld) {
      // invisible sprite... used for gameplay.
      if (!texture) return;

      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(modelToProjection, 0);

      // this is an array of the positions of the corners of the sprite in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
    
      // this is an array of the positions of the corners of the texture in 2D
      static const float uvs[] = {
         0,  0,
         1,  0,
         1,  1,
         0,  1,
      };

      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);
    
      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

	// Render Method for my Shader (elio_shader)
	void render(elio_shader &shader, mat4t &cameraToWorld, vec4 bg_color) {
		mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		shader.render(modelToProjection, bg_color);

		float vertices[] = {
			-halfWidth, -halfHeight, 0,
			halfWidth, -halfHeight, 0,
			halfWidth, halfHeight, 0,
			-halfWidth, halfHeight, 0,
		};

		glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
		glEnableVertexAttribArray(attribute_pos);

		static const float uvs[] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1,
		};

		glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvs);
		glEnableVertexAttribArray(attribute_uv);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	// move the object
	void translate(float x, float y) {
		modelToWorld.translate(x, y, 0);
	}

	// rotate the object
	void rotate(float angle, float x, float y, float z) {
		modelToWorld.rotate(angle, x, y, z);
	}

    // position the object relative to another.
    void set_relative(sprite &rhs, float x, float y) {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
    }

    // return true if this sprite collides with another.
    // note the "const"s which say we do not modify either sprite
    bool collides_with(const sprite &rhs) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
      float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

      // both distances have to be under the sum of the halfwidths
      // for a collision
      return
        (fabsf(dx) < halfWidth + rhs.halfWidth) &&
        (fabsf(dy) < halfHeight + rhs.halfHeight)
      ;
    }

	//Used to detect the position of your character in respect of the enemies (not used in my example)
    bool is_above(const sprite &rhs, float margin) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];

      return
        (fabsf(dx) < halfWidth + margin)
      ;
    }

    bool &is_enabled() {
      return enabled;
    }
  };

  class invaderers_app : public octet::app {
	  // Matrix to transform points in our camera space to the world.
	  // This lets us move our camera
	  mat4t cameraToWorld;

	  // shader to draw a textured triangle
	  texture_shader texture_shader_;
	  elio_shader elio_shader_;

	  enum {

		  // Definitions of object sprites implemented through CSV file
		  flower_sprite,
		  first_block_sprite,
		  mushroom_sprite,
		  second_block_sprite,


		  // Definition of other general and  Gameplay related items
		  num_sound_sources = 8,
		  num_rows = 4,
		  num_cols = 5,
		  num_missiles = 1,
		  num_bombs = 3,
		  num_borders = 4,
		  num_invaderers = num_rows * num_cols,

		  // All the other sprite definitions (not implemented with CSV files)
		  ship_sprite = 0,
		  bowser_sprite,
		  peach_sprite,

		  game_over_sprite,
		  you_win_sprite,

		  first_invaderer_sprite,
		  last_invaderer_sprite = first_invaderer_sprite + num_invaderers - 1,

		  first_missile_sprite,
		  last_missile_sprite = first_missile_sprite + num_missiles - 1,

		  first_bomb_sprite,
		  last_bomb_sprite = first_bomb_sprite + num_bombs - 1,

		  first_border_sprite,
		  last_border_sprite = first_border_sprite + num_borders - 1,

		  num_sprites,

	  };

	  // timers for missiles and bombs
	  int missiles_disabled;
	  int bombs_disabled;

	  // accounting for enemies
	  int live_invaderers;
	  int num_lives;
	  int boss_lives;

	  // game state
	  bool game_over;
	  int score;
	  bool flower_picked;

	  // speed of enemy and other objects
	  float invader_velocity;
	  float boss_velocity;
	  float mushroom_velocity;

	  // sounds
	  ALuint whoosh;
	  ALuint bang;
	  ALuint bump;
	  ALuint power_appears;
	  ALuint power_up;
	  ALuint power_down;
	  ALuint mario_die;
	  ALuint bowser_fire;
	  ALuint bowser_dies;
	  ALuint stage_clear;
	  ALuint gameover_;

	  ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }

	  unsigned cur_source;
	  ALuint sources[num_sound_sources];

	  // big array of sprites
	  sprite sprites[num_sprites];
	  sprite test_sprite;

	  // random number generator
	  class random randomizer;

	  // a texture for our text
	  GLuint font_texture;

	  // information for our text
	  bitmap_font font;

	  enum direction{
		  LEFT,
		  RIGHT,
		  UP,
		  DOWN
	  };

	  direction myDirection;

	  //Declaration of global variables, 2D arrays and dynamic arrays to be used with CSV files
	  static const int map_width = 20;
	  static const int map_height = 20;
	  int map[map_height][map_width];
	  int map2[map_height][map_width];
	  dynarray<sprite> map_sprites_bush;
	  dynarray<sprite> map_sprites_dirt;
	  dynarray<sprite> object_sprites;

	  //Counter for the number of border tiles inserted with CSV file
	  int num_bush = 0;

	  //Declaration of some global variables used for the character sprite
	  float mario_height = 0.25f;
	  float mario_width = 0.25f;

	  //Initial coordinates of the mushroom
	  float x_init_mush;
	  float y_init_mush;

	  //Color for the background shader
	  vec4 background_color;

	  //called to read a CSV file for the background and borders map
	  void read_csv() {

		  std::ifstream file("mapcsv.csv");

		  char buffer[2048];
		  int i = 0;

		  while (!file.eof()) {
			  file.getline(buffer, sizeof(buffer));

			  char *b = buffer;
			  for (int j = 0;; ++j) {
				  char *e = b;
				  while (*e != 0 && *e != ';') ++e;

				  map[i][j] = std::atoi(b);

				  if (*e != ';') break;
				  b = e + 1;
			  }
			  ++i;
		  }
	  }

	  // Called to read CSV file to set the objects (blocks, mushroom, flower)
	  void read_csv2() {

		  std::ifstream file("mapcsv2.csv");

		  char buffer[2048];
		  int i = 0;

		  while (!file.eof()) {
			  file.getline(buffer, sizeof(buffer));

			  char *b = buffer;
			  for (int j = 0;; ++j) {
				  char *e = b;
				  while (*e != 0 && *e != ';') ++e;

				  map2[i][j] = std::atoi(b);

				  if (*e != ';') break;
				  b = e + 1;
			  }
			  ++i;
		  }
	  }

	  //Called to initialize the background and borders maps from the CSV file
	  void setup_visual_map() {

		  GLuint bush = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/tile_brick.gif");
		  GLuint dirt = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/tile_dirt.gif");

		  for (int i = 0; i < map_height; ++i) {
			  for (int j = 0; j < map_width; ++j) {

				  sprite s;

				  if (map[i][j] == 1) {

					  s.init(bush, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
					  map_sprites_bush.push_back(s);
					  num_bush++;
				  }
				  else if (map[i][j] == 0) {
					  s.init(dirt, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
					  map_sprites_dirt.push_back(s);

				  }



			  }
		  }
	  }

	  //Called to initialize the objects (blocks, mushroom, flower) from the CSV2 file
	  void setup_object_map() {

		  GLuint mushroom = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/mushroom.gif");
		  GLuint block = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/mistery_block.gif");
		  GLuint flower = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/flower.gif");

		  for (int i = 0; i < map_height; ++i) {
			  for (int j = 0; j < map_width; ++j) {

				  sprite s;

				  if (map2[i][j] == 1) {

					  s.init(mushroom, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);

					  //used to obtain the initial coordinates of the mushroom
					  x_init_mush = -3 + 0.15f + 0.3f*j;
					  y_init_mush = 3 - 0.15f - 0.3f*i;

					  s.is_enabled() = false;
					  object_sprites.push_back(s);
				  }

				  else if (map2[i][j] == 2) {

					  s.init(block, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
					  object_sprites.push_back(s);
				  }

				  else if (map2[i][j] == 3) {

					  s.init(flower, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
					  s.is_enabled() = false;
					  object_sprites.push_back(s);
				  }

			  }
		  }
	  }

	  //called when we are hit
	  void on_hit_ship() {

		  flower_picked = false;

		  //Executed when mario is small. If he gets hit, he loses a life
		  if (mario_height < 0.4f){

			  object_sprites[flower_sprite].is_enabled() = false;

			  --num_lives;
			  --score;
			  boss_lives = 20;
			  boss_velocity = 0.04f;

			  if (num_lives == 0) {
				  game_over = true;

				  // sound effect for game over
				  ALuint source = get_sound_source();
				  alSourcei(source, AL_BUFFER, gameover_);
				  alSourcePlay(source);

				  sprites[game_over_sprite].translate(-20, 0);
			  }

			  else {

				  //sound effect when mario dies
				  ALuint source = get_sound_source();
				  alSourcei(source, AL_BUFFER, mario_die);
				  alSourcePlay(source);
			  }

			  //re-initialize the mushroom
			  GLuint mushroom = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/mushroom.gif");
			  object_sprites[mushroom_sprite].init(mushroom, x_init_mush, y_init_mush, 0.3f, 0.3f);
			  object_sprites[mushroom_sprite].is_enabled() = false;

			  //Re-draws mario at the bottom left of the screen after he dies
			  GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/big_mario.gif");
			  sprites[ship_sprite].init(ship, -2.5f, -2.5f, mario_width, mario_height);

			  if (myDirection != RIGHT){
				  sprites[ship_sprite].rotate(180, 0, 1, 0);
			  }
		  }


		  //Executed when mario is big (Has obtained mushroom or fire flower) if he gets hit he shrinks back to his original size without loosing a life
		  else if (mario_height > 0.25f){

			  //re-sets the flower state to "not-picked'
			  flower_picked = false;

			  //sound effect when mario shrinks
			  ALuint source = get_sound_source();
			  alSourcei(source, AL_BUFFER, power_down);
			  alSourcePlay(source);

			  //gets the position of mario in the moment he is hit
			  vec2 pos = sprites[ship_sprite].get_Position();

			  //re-scales mario
			  mario_width = 0.25f;
			  mario_height = 0.25f;

			  //re-draws mario in the position he is when he is hit but re-scaled
			  GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/big_mario.gif");
			  sprites[ship_sprite].init(ship, pos.x(), pos.y(), mario_width, mario_height);

			  if (myDirection != RIGHT){
				  sprites[ship_sprite].rotate(180, 0, 1, 0);
			  }
		  }
	  }

	  // called when we hit an enemy
	  void on_hit_invaderer() {
		  ALuint source = get_sound_source();
		  alSourcei(source, AL_BUFFER, bang);
		  alSourcePlay(source);

		  live_invaderers--;
		  score++;
		  if (live_invaderers == 4) {
			  invader_velocity *= 4;

		  }
		  else if (live_invaderers == 0) {
			  sprites[bowser_sprite].translate(-17.6f, 0.0f);
			  sprites[bowser_sprite].is_enabled() = true;
			  background_color = vec4(0.8f, 0.3f, 0.1f, 1.0f);

		  }
	  }

	  //called when we hit the boss 
	  void on_hit_boss() {
		  ALuint source = get_sound_source();
		  alSourcei(source, AL_BUFFER, bang);
		  alSourcePlay(source);

		  boss_lives--;

		  if (boss_lives == 10) {
			  boss_velocity *= 3;

		  }
		  else if (boss_lives == 0) {
			  ALuint source = get_sound_source();
			  alSourcei(source, AL_BUFFER, bowser_dies);
			  alSourcePlay(source);

			  background_color = vec4(1.0f, 0.4f, 0.7f, 1.0f);

			  sprites[bowser_sprite].is_enabled() = false;
			  sprites[bowser_sprite].translate(20, 0);
			  sprites[peach_sprite].is_enabled() = true;
			  score = score + 20;

		  }
	  }

	  void collide_with_enemy(){
		  const float ship_speed = 0.05f;
		  for (unsigned int j = 0; j != num_invaderers; ++j)
		  {
			  sprite &invaderer = sprites[first_invaderer_sprite + j];
			  sprite &bowser = sprites[bowser_sprite];
			  if (sprites[ship_sprite].collides_with(invaderer) || sprites[ship_sprite].collides_with(bowser))
			  {


				  if (sprites[bowser_sprite].is_enabled())
				  {
					  sprites[ship_sprite].translate(-10 * ship_speed, 0);
				  }
				  on_hit_ship();
			  }
		  }



	  }

	  //use the keyboard to move the ship
      void move_ship() {
      const float ship_speed = 0.05f;
	  
      //left, right, up and down arrows
	  if (is_key_down(key_left)) {
		  if (myDirection != LEFT)
		  {
		  //Rotates Mario to make him face in the correct direction (same for every direction)
			  myDirection = LEFT;
			  sprites[ship_sprite].rotate(180, 0, 1, 0);
			  
		  }

		 sprites[ship_sprite].translate(+ship_speed, 0);

		  //Sets collisions against the borders/walls and the block objects obtained from CSV files 
		  //(same for every direction except for UP, look at bocks_pressed())
		  for (int i = 0; i < num_bush; i++){
			   if (sprites[ship_sprite].collides_with(map_sprites_bush[i]) || 
				   sprites[ship_sprite].collides_with(object_sprites[first_block_sprite])|| 
				   sprites[ship_sprite].collides_with(object_sprites[second_block_sprite])) {

				   sprites[ship_sprite].translate(-ship_speed, 0);
			  }
	       }
	    }

	    else if (is_key_down(key_right)) {

		     if (myDirection != RIGHT)
		   {
			  
			  myDirection = RIGHT;
			  sprites[ship_sprite].rotate(180, 0, 1, 0);
		   }

          sprites[ship_sprite].translate(+ship_speed, 0);

		  for (int i = 0; i < num_bush; i++) {
			   if (sprites[ship_sprite].collides_with(map_sprites_bush[i]) ||
				   sprites[ship_sprite].collides_with(object_sprites[first_block_sprite]) ||
				   sprites[ship_sprite].collides_with(object_sprites[second_block_sprite])) {

			       sprites[ship_sprite].translate(-ship_speed, 0);
		      }
            }
          }

	    else if (is_key_down(key_up)) {

		   sprites[ship_sprite].translate(0, +ship_speed);

		   for (int i = 0; i < num_bush; i++) {
			   if (sprites[ship_sprite].collides_with(map_sprites_bush[i])) {
				   sprites[ship_sprite].translate(0, -ship_speed);
			   }
		    }
	     }

	    else if (is_key_down(key_down)) {

		   sprites[ship_sprite].translate(0, -ship_speed);

		   for(int i = 0; i < num_bush; i++) {
			   if (sprites[ship_sprite].collides_with(map_sprites_bush[i]) ||
				   sprites[ship_sprite].collides_with(object_sprites[first_block_sprite]) ||
				   sprites[ship_sprite].collides_with(object_sprites[second_block_sprite])) {
				   sprites[ship_sprite].translate(0, +ship_speed);
			   }
		     }
	       }

		//Check if Mario gets an object (Flower, Mushroom)
		objects_collide();

		//Check collisions with the blocks containing the objects
		blocks_pressed();

		//Collisions with enemies
		collide_with_enemy();

		//interaction with peach
		princess_met();

    }

      //fire button (space)
      void fire_missiles() {
		if (flower_picked == true){
			if (missiles_disabled) {
				--missiles_disabled;
			}
			else if (is_key_going_down(' ')) {
				// find a missile
				for (int i = 0; i != num_missiles; ++i) {
					if (!sprites[first_missile_sprite + i].is_enabled()) {
						sprites[first_missile_sprite + i].set_relative(sprites[ship_sprite], 0.02f, 0);
						sprites[first_missile_sprite + i].is_enabled() = true;
						missiles_disabled = 5;
						ALuint source = get_sound_source();
						alSourcei(source, AL_BUFFER, whoosh);
						alSourcePlay(source);
						break;
					}
				}
			}
		}
    }

      // Make the Boss fire a bomb
      void fire_bombs() {
      if (bombs_disabled) {
        --bombs_disabled;
      } else {
    
				 sprite &ship = sprites[ship_sprite];
				 sprite &bowser = sprites[bowser_sprite];

				 if (bowser.is_enabled()) {
            
							for (int i = 0; i != num_bombs; ++i) {

								if (!sprites[first_bomb_sprite+i].is_enabled()) {
								sprites[first_bomb_sprite+i].set_relative(bowser, -0.25, 0);
								sprites[first_bomb_sprite+i].is_enabled() = true;
								bombs_disabled = 30;
								ALuint source = get_sound_source();
								alSourcei(source, AL_BUFFER, bowser_fire);
								alSourcePlay(source);
								return;
                        }
                      }
              return;
          }
        }
      }

      // animate the missiles
      void move_missiles() {
      const float missile_speed = 0.3f;
      for (int i = 0; i != num_missiles; ++i) {
          sprite &missile = sprites[first_missile_sprite+i];
		
		if (missile.is_enabled()) {
			missile.translate(missile_speed, 0);		
			for (int j = 0; j != num_invaderers; ++j) {
				sprite &invaderer = sprites[first_invaderer_sprite + j];
				if (invaderer.is_enabled() && missile.collides_with(invaderer)) {
					invaderer.is_enabled() = false;
					invaderer.translate(20, 0);
					missile.is_enabled() = false;
					missile.translate(20, 0);
					on_hit_invaderer();

					goto next_missile;
		         }
	            }

			if(sprites[bowser_sprite].is_enabled()) {
				sprite &boss = sprites[bowser_sprite];
				if (missile.collides_with(boss)) {
					missile.is_enabled() = false;
					missile.translate(20, 0);
					on_hit_boss();

					goto next_missile;
				}
			  }

			for (unsigned int j = 0; j < map_sprites_bush.size(); ++j){
				if (missile.collides_with(map_sprites_bush[j])) {
					missile.is_enabled() = false;
					missile.translate(20, 0);
				}
		      }
        }
      next_missile:;
      }
    }

      // animate the bombs
      void move_bombs() {
      const float bomb_speed = 0.1f;
      for (int i = 0; i != num_bombs; ++i) {
		  
        sprite &bomb = sprites[first_bomb_sprite+i];
		if (bomb.is_enabled()) {
			bomb.translate(-bomb_speed, 0);
			if (bomb.collides_with(sprites[ship_sprite])) {
				bomb.is_enabled() = false;
				bomb.translate(20, 0);
				bombs_disabled = 50;
				on_hit_ship();
				goto next_bomb;
			}

			//re-sets the bombs when they collide with the borders
			for (unsigned int j = 0; j < map_sprites_bush.size(); ++j){
				if (bomb.collides_with(map_sprites_bush[j])) {
					bomb.is_enabled() = false;
					bomb.translate(20, 0);
				 }
			 }

			// make bomb disappear if they collide with missile
			for (unsigned int j = 0; j !=num_missiles; ++j){
				sprite &missile = sprites[first_missile_sprite + j];
				if (bomb.collides_with(missile)) {
					bomb.is_enabled() = false;
					bomb.translate(20, 0);
					missile.is_enabled() = false;
					missile.translate(20, 0);
				}
			}
        }
      next_bomb:;
      }
    }

      // move the array of enemies 
      void move_invaders(float dx, float dy) {
      for (int j = 0; j != num_invaderers; ++j) {
        sprite &invaderer = sprites[first_invaderer_sprite+j];
        if (invaderer.is_enabled()) {
          invaderer.translate(dx, dy);
        }
      }
    }

	  // move the Boss 
	  void move_boss(float dx, float dy) {
		if (sprites[bowser_sprite].is_enabled()) {
			sprite &bowser = sprites[bowser_sprite];
			bowser.translate(dx, dy);
			}
		}
	
	  // move the mushroom 
	  void move_mushroom(float dx, float dy) {
			sprite &mushroom = object_sprites[mushroom_sprite];
			if (mushroom.is_enabled()) {
				mushroom.translate(dx, dy);
			}
		
	}

      // check if any invaders hit the sides.
      bool invaders_collide(sprite &border) {
      for (int j = 0; j != num_invaderers; ++j) {
        sprite &invaderer = sprites[first_invaderer_sprite+j];
        if (invaderer.is_enabled() && invaderer.collides_with(border)) {
          return true;
        }
      }
      return false;
    }

	  // check if mushroom hits the side
	  bool mushroom_collide(sprite &border) {
		sprite &mushroom = object_sprites[mushroom_sprite];
		if (mushroom.is_enabled() && mushroom.collides_with(border)) {
			return true;
	     }
		
		return false;
	}

	  // check if boss hits the side
	  bool boss_collide(sprite &border) {

		sprite &bowser = sprites[bowser_sprite];
		if (bowser.collides_with(border)) {
			return true;
		}

		return false;
	}

	  //Interaction with objects
	  void objects_collide(){
		if (sprites[ship_sprite].collides_with(object_sprites[mushroom_sprite]) && object_sprites[mushroom_sprite].is_enabled() == true)
		{
			object_sprites[mushroom_sprite].is_enabled() = false;

			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, power_up);
			alSourcePlay(source);

			mario_height = 0.4f;
			mario_width = 0.25f;
			vec2 pos = sprites[ship_sprite].get_Position();

			GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/big_mario.gif");
			sprites[ship_sprite].init(ship, pos.x(), pos.y(), mario_width, mario_height);
			if (myDirection != RIGHT){
				sprites[ship_sprite].rotate(180, 0, 1, 0);
			}

			GLuint mushroom = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/mushroom.gif");
			object_sprites[mushroom_sprite].init(mushroom, x_init_mush, y_init_mush, 0.3f, 0.3f);
		}

		if (sprites[ship_sprite].collides_with(object_sprites[flower_sprite]) && object_sprites[flower_sprite].is_enabled() == true && flower_picked == false)
		{
			object_sprites[flower_sprite].is_enabled() = false;
			flower_picked = true;

			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, power_up);
			alSourcePlay(source);

			mario_height = 0.4f;

			vec2 pos = sprites[ship_sprite].get_Position();

			GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/fire_mario.gif");
			sprites[ship_sprite].init(ship, pos.x(), pos.y(), mario_width, mario_height);
			if (myDirection != RIGHT){
				sprites[ship_sprite].rotate(180, 0, 1, 0);
			}
		}

	}

	  //Interactions with blocks containing objects
	  void blocks_pressed(){
		const float ship_speed = 0.05f;
		if (sprites[ship_sprite].collides_with(object_sprites[second_block_sprite]) && is_key_down(key_up))
		{

			sprites[ship_sprite].translate(0, -5 * ship_speed);

			object_sprites[mushroom_sprite].is_enabled() = true;

			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, bump);
			alSourcePlay(source);


		}

		if (mario_height > 0.25f) {

			if (sprites[ship_sprite].collides_with(object_sprites[first_block_sprite]) && is_key_down(key_up))
			{
				if (flower_picked == false){
					sprites[ship_sprite].translate(0, -5 * ship_speed);

					object_sprites[flower_sprite].is_enabled() = true;

					ALuint source = get_sound_source();
					alSourcei(source, AL_BUFFER, power_appears);
					alSourcePlay(source);


				}

				else {
					sprites[ship_sprite].translate(0, -5 * ship_speed);

					ALuint source = get_sound_source();
					alSourcei(source, AL_BUFFER, bump);
					alSourcePlay(source);


				}
			}

		}

		else  if (sprites[ship_sprite].collides_with(object_sprites[first_block_sprite]) && is_key_down(key_up))
		{

			sprites[ship_sprite].translate(0, -5 * ship_speed);
			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, bump);
			alSourcePlay(source);


		}



		if (mario_height > 0.25f){

			object_sprites[mushroom_sprite].is_enabled() = false;
		}
	}

	  //Interaction with princess
	  void princess_met(){
		if (sprites[peach_sprite].is_enabled() && sprites[ship_sprite].collides_with(sprites[peach_sprite]))
		{
			ALuint source = get_sound_source();
			alSourcei(source, AL_BUFFER, stage_clear);
			alSourcePlay(source);


			game_over = true;
			sprites[you_win_sprite].translate(-20, 0);

		}
	
	}

	  // Deals with displaying text (score, lives, etc.)
      void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      enum { max_quads = 32 };
      bitmap_font::vertex vertices[max_quads*4];
      uint32_t indices[max_quads*6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, font_texture);

      shader.render(modelToProjection, 0);

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x );
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u );
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    }

  public:

      // this is called when we construct the class
      invaderers_app(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {
    }

      // this is called once OpenGL is initialized
      void app_init() {

      // set up the shaders
      texture_shader_.init();
	  elio_shader_.init();

	  // reads the CSV files for the borders (csv) and the objects (csv2)
	  read_csv();
	  read_csv2();

	  // initializes the map borders and the objects defined in the CSV files
	  setup_visual_map();
	  setup_object_map();

      //initializes the default starting direction
	  myDirection = RIGHT;

      // set up the matrices with a camera 3 units from the origin
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 3);

	  // Initializes the sprites with the correct texture
      font_texture = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");

      GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/big_mario.gif");
      sprites[ship_sprite].init(ship, -2.5f, -2.5f, mario_width, mario_height);

	  //GameOver and You Win sprites are created off-screen
      GLuint GameOver = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/GameOver.gif");
      sprites[game_over_sprite].init(GameOver, 20, 0, 3, 1.5f);
	  GLuint YouWin = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/youwon.gif");
	  sprites[you_win_sprite].init(YouWin, 20, 0, 3, 1.5f);

      GLuint invaderer = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/goomba.gif");
      for (int j = 0; j != num_rows; ++j) {
        for (int i = 0; i != num_cols; ++i) {
          assert(first_invaderer_sprite + i + j*num_cols <= last_invaderer_sprite);
          sprites[first_invaderer_sprite + i + j*num_cols].init(
            invaderer, ((float)i - num_cols * 0.5f) * 1.0f, 2.50f - ((float)j * 1.5f), 0.25f, 0.25f
          );
        }
      }

      // use the missile texture
      GLuint missile = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/MarioFireball.gif");
      for (int i = 0; i != num_missiles; ++i) {
        // create missiles off-screen
        sprites[first_missile_sprite+i].init(missile, 20, 0, 0.20f, 0.20f);
        sprites[first_missile_sprite+i].is_enabled() = false;
      }


      // use the bomb texture
      GLuint bomb = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/Fireball.gif");
      for (int i = 0; i != num_bombs; ++i) {
        // create bombs off-screen
        sprites[first_bomb_sprite+i].init(bomb, 20, 0, 0.25, 0.10f);
        sprites[first_bomb_sprite+i].is_enabled() = false;
      }

	  // Create the boss and use boss texture
	  GLuint bowser = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/bowser.gif");
	  // create boss off-screen
	  sprites[bowser_sprite].init(bowser, 20, 1.5f, 0.50f, 0.50f);
	  sprites[bowser_sprite].is_enabled() = false;

	  // Create the princess and use princess texture
	  GLuint peach = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/peach.gif");
	  sprites[peach_sprite].init(peach, 1.5f, 1.5f, 0.55f, 0.55f);
	  sprites[peach_sprite].is_enabled() = false;
	  
      // sounds
      whoosh = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/sound_fireball.wav");
      bang = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/stomp.wav");
	  bump = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bump.wav");
	  power_appears = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/powerup_appears.wav");
	  power_up = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/powerup.wav");
	  power_down = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/power_down.wav");
	  mario_die = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/mariodie.wav");
	  bowser_fire = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bowserfire.wav");
	  bowser_dies = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bowser_dies.wav");
	  stage_clear = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/stage_clear.wav");
	  gameover_ = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/gameover.wav");

      cur_source = 0;
      alGenSources(num_sound_sources, sources);

      // sundry counters and game state.
      missiles_disabled = 0;
      bombs_disabled = 50;
      invader_velocity = - 0.03f;
	  mushroom_velocity = 0.02f;
	  boss_velocity = 0.04f;
      live_invaderers = num_invaderers;
      num_lives = 10;
	  boss_lives = 20;
      game_over = false;
      score = 0;
	  flower_picked = false;
	  background_color = vec4(0.4f, 0.8f, 0.1f, 1.0f);
    }

      // called every frame to move things
      void simulate() {
       if (game_over) {
        return;
      }

       move_ship();

	   fire_missiles();
	  
       fire_bombs();

       move_missiles();

       move_bombs();

       move_invaders(invader_velocity, 0);

	   move_mushroom(mushroom_velocity, 0);

	   move_boss(0, boss_velocity);

	   //check if invaders collide with borders
	   for (unsigned int i = 0; i < map_sprites_bush.size(); i++){
		  sprite &border = map_sprites_bush[i];
		   if (invaders_collide(border)) {
			  for (int i = 0; i != num_invaderers; ++i)
			  {
				  sprites[first_invaderer_sprite + i].rotate(180, 0, 1, 0);
			  }
			  
			  move_invaders(invader_velocity, 0);
			 
		   }
	    }

	   //check if mushroom collides
	   for (unsigned int i = 0; i < map_sprites_bush.size(); i++){
		   sprite &border = map_sprites_bush[i]; 
		   if (mushroom_collide(border)) {
			  mushroom_velocity = -mushroom_velocity;
			  move_invaders(mushroom_velocity, 0);
		   }
	    }

	   //check if boss collides
	   for (unsigned int i = 0; i < map_sprites_bush.size(); i++){
		  sprite &border = map_sprites_bush[i]; 
		   if (boss_collide(border)) {

			  boss_velocity = -boss_velocity;
			  move_boss(0, boss_velocity);

		   }
	    }
   }

     // this is called to draw the world
      void draw_world(int x, int y, int w, int h) {
      
	  simulate();

      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

	  int screen_width = 0;
	  int screen_height = 0;
	  get_viewport_size(screen_width, screen_height);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glDisable(GL_DEPTH_TEST);

      // allow alpha blend (transparency when alpha channel is 0)
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	  //draw the map sprites (border)
	  for (unsigned int i = 0; i < map_sprites_bush.size(); ++i) {
		  map_sprites_bush[i].render(texture_shader_, cameraToWorld);
	  }

	  //draw the map sprites (background). My shader (elio_shader_) is implemented here on top of the tiles for background set in the CSV file
	  for (unsigned int i = 0; i < map_sprites_dirt.size(); ++i) {
		  map_sprites_dirt[i].render(elio_shader_, cameraToWorld, background_color);
	  }

	  //draw the object sprites (blocks, mushroom, flower) implemented through the CSV2 file
	  for (unsigned int i = 0; i < object_sprites.size(); ++i) {
		  if (object_sprites[i].is_enabled())
			object_sprites[i].render(texture_shader_, cameraToWorld);  
	  }

      // draw all the other sprites
      for (int i = 0; i != num_sprites; ++i) {
		  if (sprites[i].is_enabled())
		  sprites[i].render(texture_shader_, cameraToWorld);
      }

	  //Prints the score and lives
      char score_text[32];
      sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
      draw_text(texture_shader_, -1.75f, 2, 1.0f/256, score_text);

      // move the listener with the camera
      vec4 &cpos = cameraToWorld.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
    }

     };
}
