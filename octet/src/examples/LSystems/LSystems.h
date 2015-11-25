////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "tree_elio.h"

namespace octet {
	class lsystems : public app {

		//Class for the node
		class node {
			vec3 pos;
			float angle1;
		public:
			//Constructor of the first segment
			node() {
				pos = vec3(0.0f, 0.0f, 0.0f);
				angle1 = 0.0f;
			}
			//Constructor of the following segments
			node(vec3 pos_, float angle_) {
				pos = pos_;
				angle1 = angle_;
			}

			vec3& get_pos() {
				return pos;
			}

			float& get_angle() {
				return angle1;
			}
		};

		ref<visual_scene> app_scene;

		//Declaration of an object of the class "Tree" present in tree_elio.h file to access the contained functions
		tree t;

		//Declaration of the array containing the information about each node
		dynarray<node> node_stack;

		//Declaration of Materials/colors to use
		material *material_wood;
		material *material_leaf;
		material *material_leaf2;
		material *material_black;
		material *material_autumn1;
		material *material_autumn2;
		material *material_spring1;
		material *material_spring2;
		material *material_snow;
		material *material_dark_snow;

		//Declaration and Initialization of some geometrical constants and variables
		float tree_max_y = 0.0f;
		const float PI = 3.14159265f;
		const float SEGMENT_LENGTH = 0.5f;
		float SEGMENT_WIDTH = 0.1f;

		//Declaration and Initialization of some state constants and variables
		int current_example = 1; 
		int color_type = 1;
		unsigned int current_iteration = 0;
		const int min_example = 1;
		const int MAX_example = 9;
		unsigned int MAX_iteration = 10;
		float far_plane = 500.0f;
		float angle_increment = 0.0f;
		int season = 1;
		float zoom_increment = 0.0f;
		float x_increment = 0.0f;
		float y_increment = 0.0f;
		float rotation_amount = 0.0f;
		bool auto_scale = false;
		bool rotation_on = false;

	public:
		lsystems(int argc, char **argv) : app(argc, argv) {
		}

		//Called once at the beginning to initialize everything
		void app_init() {

			//Reading of the first example
			t.read_text_file(current_example);

			//Sets the Maximum iteration for the first example
			set_MAX_iteration();

			//Checks if the first example is Deterministic or Stochastic
			is_stoc();

			//Prints instructions for the user
			print_instruction();

			//Initializes the scene and camera position
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.0f, 1.0f));

			//Definition and initialization of the differt materilas/colors used 
			material_wood = new material(vec4(0.39f, 0.20f, 0.0f, 1.0f));//Brown wood
			material_leaf = new material(vec4(0.0f, 0.30f, 0.0f, 1.0f)); //Dark green leaf
			material_leaf2 = new material(vec4(0.0f, 0.45f, 0.0f, 1.0f));//Light green leaf
			material_black = new material(vec4(0.0f, 0.0f, 0.0f, 1.0f)); //Black
			material_autumn1 = new material(vec4(0.63f, 0.08f, 0.08f, 1.0f)); //Dark red
			material_autumn2 = new material(vec4(0.95f, 0.09f, 0.09f, 1.0f)); //Light red
			material_spring1 = new material(vec4(0.99f, 0.2f, 0.59f, 1.0f));//Dark pink
			material_spring2 = new material(vec4(0.99f, 0.4f, 0.69f, 1.0f));//Light pink
			material_snow = new material(vec4(1.0f, 1.0f, 1.0f, 1.0f));//White
			material_dark_snow = new material(vec4(0.875f, 0.875f, 0.875f, 1.0f));//Grey (Dirty snow)

			create_geometry();
		}

		//Called every frame to update the scene
		void draw_world(int x, int y, int w, int h) {

			set_MAX_iteration();
			is_stoc();
			handle_input();
			
			app_scene->begin_render(w, h);

			app_scene->update(1.0f / 30.0f);

			app_scene->render((float)w / h);
		}

		//Print instructions to the console after initialization
		void print_instruction(){

			std::cout << "-------Welcome to L-Systems Generator-------\n";
			std::cout << "\nHotkeys Details: \n\n";
			std::cout << "- Spacebar/Backspase: Evolve/Devolve the system\n";
			std::cout << "- Esc/Tab: Increase/Decrease the angle between branches\n";
			std::cout << "- f2/f1: Increase/Decrease the thickness of the branches\n";
			std::cout << "- f3: Change season (Summer, Fall, Spring, Winter)\n";
			std::cout << "- delete: Rotate the model\n";
			std::cout << "- Shift/Ctrl: Zoom in/Zoom out\n";
			std::cout << "- Arrow keys: Move camera";
			std::cout << "\n\nCurrent example: "<<current_example<<"\n";
			std::cout << "Season: Summer\n";

			if (is_stoc())
			{
				std::cout << "\nStochastic\n";
			}

			else
			{
				std::cout << "\nDeterministic\n";
			}
		
		}

		//Sets the Max iteration for each example 
		void set_MAX_iteration(){
		
			switch (current_example)
			{
			case 1: MAX_iteration = 5;
				    break;
			case 2: MAX_iteration = 5;
				    break;
			case 3: MAX_iteration = 4;
				    break;
			case 4: MAX_iteration = 7;
				    break;
			case 5: MAX_iteration = 7;
				    break;
			case 6: MAX_iteration = 5;
				    break;
			case 7: MAX_iteration = 5;
				    break;
			case 8: MAX_iteration = 5;
				    break;
			case 9: MAX_iteration = 5;
				break;
			}

		
		}

		//Checks if the example is Stochastic or Deterministic
		bool is_stoc(){
		
			if (current_example == 8)
			{
				return true;
			}

			else {
				return false;
			}
		
		}

		//Handles the user-input (hotkeys) for different actions on the L-system
		void handle_input() {

			//Evolves the system up the the maximum pre-set iteration for that particular example
			if (is_key_going_down(key_space) && current_iteration < MAX_iteration) {
				
				++current_iteration;
				
				//Checks if the system is deterministic or stochastic and uses the relevant evolution method
				if (current_example != 9){
					if (is_stoc())
					{
						t.evolve_stoc();
					}

					else {
						t.evolve();
					}
				}

				//Example 9 is handled in a different stochastic way
				else { t.evolve_stoc_type_2();}

				//It optimizes the size of the lines for certain examples so they are visibile from further away
				if ((current_example == 4 || current_example == 5) && current_iteration == MAX_iteration && SEGMENT_WIDTH < 0.2)
				{
					SEGMENT_WIDTH += 0.1f;
					auto_scale = true;
				}

				//Draws the following iteration
				draw_again();

				//Optimizes camera position for some examples so they fit the screen
				if (current_example != 7 && current_example != 2 && current_example != 8 && current_example != 9 && current_iteration > 3){

					app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 40.0f));
					zoom_increment += 40.0f;
				}

				//Optimizes camera position for Example 7
				if (current_example == 7 && current_iteration >= 3){
					app_scene->get_camera_instance(0)->get_node()->translate(vec3(-10.0f, 0, 20.0f));
					zoom_increment += 20.0f;
				}

				//Print current Iteration to the console
				std::cout << "Current iteration: " << current_iteration<<"\n";	
			}

			//If the maximum evolution has been reached prints a warning message
			else if (is_key_going_down(key_space) && current_iteration >= MAX_iteration)
			{
				std::cout << "\nMaximum number of iterations reached!\n";
				
			}

			// Devolve the system
			if (is_key_going_down(key_backspace) && current_iteration > 0) {

				if (current_iteration > 1 && current_example!=9){

					//Read the current file again
					t.read_text_file(current_example);

					
						//Re-draws the L-system up the the previous iteration
						for (unsigned int i = 1; i <= current_iteration - 1; i++){

							if (is_stoc())
							{
								t.devolve_stoc(i);
							}

							else {
								t.evolve();
							}

							//Reverts the autoscale in the evolve function for certain examples
							if (auto_scale)
							{
								SEGMENT_WIDTH -= 0.1f;
								auto_scale = false;
							}

							draw_again();
						}
					
					//Camera adjustments equal and opposite to the evolution method
						if (current_example != 7 && current_example != 2 && current_example != 8 && current_iteration > 3){

						app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -40.0f));
						zoom_increment -= 40.0f;
					}

					//Camera adjustments equal and opposite to the evolution method
					if (current_example == 7 && current_iteration >= 3){
						app_scene->get_camera_instance(0)->get_node()->translate(vec3(10.0f, 0, -20.0f));
						zoom_increment -= 20.0f;
					}

					
					current_iteration--;
					std::cout << "Current iteration: " << current_iteration << "\n";
				}

				//Example 9 gets redrawn from scratch
				else if (current_iteration <= 1 || current_example==9){
						t.read_text_file(current_example);
						draw_again();

						if (current_example != 9){
							current_iteration--;
						}
						else { current_iteration = 0; }

						std::cout << "Current iteration: " << current_iteration << "\n";
					   }

				//If the system is stochastic this deletes the last element in the array containing the information on the rule to use
				if (is_stoc()){

					t.decrese_stoc_array();
				}	
			}

			//Change example: next example (mouse left click)
			if (is_key_going_down(key_lmb)) {

				//Resets all the state variables
				reset_variables();
				
				if (current_example == MAX_example)
				{
					current_example = min_example;
				}
				else { ++current_example; }

					t.read_text_file(current_example);

					draw_again();

					std::cout << "\nCurrent example: " << current_example << "\n";

					if (is_stoc()||current_example == 9)
					{
						std::cout << "\nStochastic\n";
					}

					else
					{
						std::cout << "\nDeterministic\n";
					}
			}

			//change example: go the previous example (mouse right click)
			if (is_key_going_down(key_rmb)) {
                
				//Resets all the state variables
				reset_variables();
				
				if (current_example == min_example)
				{
					current_example = MAX_example;
				}
				else { --current_example; }

					t.read_text_file(current_example);

					draw_again();
					std::cout << "\nCurrent example: " << current_example << "\n";

					if (is_stoc() ||current_example == 9)
					{
						std::cout << "\nStochastic\n";
					}

					else
					{
						std::cout << "\nDeterministic\n";
					}
			}

			//Zoom in
			if (is_key_down(key_shift))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.50f));
				zoom_increment -= 1.50f;
			}

			//Zoom out
			if (is_key_down(key_ctrl))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.50f));
				zoom_increment += 1.50f;
			}

			//Move camera left
			if (is_key_down(key_left))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
				x_increment -= 0.5f;
			}

			//Move camera right
			if (is_key_down(key_right))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
				x_increment += 0.5f;
			}

			//Move camera up
			if (is_key_down(key_up))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
				y_increment += 0.5f;
			}

			//Move camera down
			if (is_key_down(key_down))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
				y_increment -= 0.5f;
			}

			//Rotation of the model
			if (is_key_down(key_delete))
			{
				rotation_amount += 2.0f;
				rotation_on = true;
				
				for (int i = 0; i < app_scene->get_num_mesh_instances(); ++i) {
					mesh_instance *mi = app_scene->get_mesh_instance(i);
					mi->get_node()->rotate(2.0f, vec3(0, 1, 0));
				}
			}

			//Increase angle between branches (possible until the iteration before the maximum to protect from possible crash)
			if (is_key_down(key_esc) && current_iteration <= MAX_iteration-1) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					angle_increment += 1.5f;
					re_draw();
				}
			}

			//Decrese angle between brances (possible until the iteration before the maximum to protect from crash)
			if (is_key_down(key_tab) && current_iteration <= MAX_iteration - 1) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					angle_increment -= 1.5f;
					re_draw();
				}
			}

			//Increase Thickness of the lines (possible until the iteration before the maximum to protect from crash)
			if (is_key_down(key_f2) && current_iteration <= MAX_iteration - 1) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					SEGMENT_WIDTH += 0.1f;
					re_draw();
				}

			}

			//Decrease Thickness of the lines
			if (is_key_down(key_f1) && current_iteration <= MAX_iteration - 1 && SEGMENT_WIDTH > 0.1f) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					SEGMENT_WIDTH -= 0.1f;
					re_draw();
				}
			}

			//Changes color of the "leaves"
			if (is_key_going_down(key_f3) && current_example!=7 && current_iteration < MAX_iteration) {

				t.read_text_file(current_example);
				if (season == 4)
				{
					season = 1;
				}
				else { season++; }

				re_draw();

				switch (season)
				{
				case 1: std::cout << "\nSeason: Summer\n";
					break;
				case 2: std::cout << "\nSeason: Autumn\n";
					break;
				case 3: std::cout << "\nSeason: Spring\n";
					break;
				case 4: std::cout << "\nSeason: Winter\n";
					break;
				}
			}
		}

		//Called at each iteration to initialize some parameters and call the relevant functions to draw the subsequents segments
		void draw_again(){

			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();

			//Setting the far plane further away so the tree doesn't disappear when zooming out
			app_scene->get_camera_instance(0)->set_far_plane(far_plane);

			tree_max_y = 0.0f;

			//Called to interpret the geometry to draw from the file
			create_geometry();

			//Adjusts the camera
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(x_increment, (tree_max_y / 2.0f) + y_increment, zoom_increment));

			//Accounts for rotation inputed by the user and re-draws the iterated model with the right orientation
			if (rotation_on){
				for (int i = 0; i < app_scene->get_num_mesh_instances(); ++i) {
					mesh_instance *mi = app_scene->get_mesh_instance(i);
					mi->get_node()->rotate(rotation_amount, vec3(0, 1, 0));
				}
			}
		}

		//Interprets the geometrical parameters from the read files
		void create_geometry() {
			float angle = 0.0f;
			dynarray<char> axiom = t.get_axiom();
			vec3 pos = vec3(0.0f, 0.0f, 0.0f);

			for (unsigned int i = 0; i < axiom.size(); ++i) {
				if (axiom[i] == '+') {

					//Assigns the right angle increment corresponding to the relevant example
					switch (current_example)
					{
					case 1: angle += (25.7f + angle_increment);
						break;
					case 2: angle += (20.0f + angle_increment);
						break;
					case 3: angle += (22.5f + angle_increment);
						break;
					case 4: angle += (20.0f + angle_increment);
						break;
					case 5: angle += (25.7f + angle_increment);
						break;
					case 6: angle += (22.5f + angle_increment);
						break;
					case 7: angle += (60.0f + angle_increment);
						break;
					case 8: angle += (25.7f + angle_increment);
						break;
					case 9: angle += (25.7f + angle_increment);
						break;
					}
				}

				//Assigns the right angle decrement corresponding to the relevant example
				else if (axiom[i] == '-') {
					switch (current_example)
					{
					case 1: angle -= (25.7f + angle_increment);
						break;
					case 2: angle -= (20.0f + angle_increment);
						break;
					case 3: angle -= (22.5f + angle_increment);
						break;
					case 4: angle -= (20.0f + angle_increment);
						break;
					case 5: angle -= (25.7f + angle_increment);
						break;
					case 6: angle -= (22.5f + angle_increment);
						break;
					case 7: angle -= (60.0f + angle_increment);
						break;
					case 8: angle -= (25.7f + angle_increment);
						break;
					case 9: angle -= (25.7f + angle_increment);
						break;
					}
				}

				//Push the current position and orientation
				else if (axiom[i] == '[') {
					node nod = node(pos, angle);
					node_stack.push_back(nod);
				}

				//Pops the current position and orientation
				else if (axiom[i] == ']') {
					node nod = node_stack[node_stack.size() - 1];
					node_stack.pop_back();
					angle = nod.get_angle();
					pos = nod.get_pos();
				}

				//Interprets the color to use
				else if (axiom[i] == 'A') //Color of the Branches

				{

					color_type = 1; //Brown

				}

				else if (axiom[i] == 'B') //Color of some of the leaves depending on the chosen season
				{

					if (season == 1)//Summer season
					{
						color_type = 2;// Dark Green
					}
					else if (season == 2)//Autumn Season
					{
						color_type = 5;// Dark red
					}

					else if (season == 3)//Spring Season
					{
						color_type = 7;// Dark Pink
					}
					else if (season == 4)//Winter Season
					{
						color_type = 9;// White
					}
				}
				else if (axiom[i] == 'C') {//Color of other leaves depending on the chosen season

					if (season == 1)// Summer season
					{
						color_type = 3;// Lighter Green
					}
					else if (season == 2)// Autumn Season
					{
						color_type = 6;// Lighter Red
					}

					else if (season == 3)// Spring Season
					{
						color_type = 8;// Lighter Pink
					}

					else if (season == 3)// Winter Season
					{
						color_type = 10;// Dirty snow
					}
				}
				else if (axiom[i] == 'D') //Theme for example 7 (Koch curve)
				{

					color_type = 4;// Black
				}
				else if (axiom[i] == 'F') //Turtle graphics command: Draw forward
				{

					pos = draw_segment(pos, angle);
				}
			}
		}

		//Calculates positions and directions of each segment and draws them accordingly in the space
		vec3 draw_segment(vec3 start_pos, float _angle) {
			
			vec3 mid_pos;//Is the mid position between the current and the new segment
			vec3 end_pos;//Is the mid point of the new segment to be drawn

			//Calculations of the components
			mid_pos.x() = start_pos.x() + SEGMENT_LENGTH * cos((_angle + 90.0f) * PI / 180.0f);
			mid_pos.y() = start_pos.y() + SEGMENT_LENGTH * sin((_angle + 90.0f) * PI / 180.0f);
			end_pos.x() = start_pos.x() + SEGMENT_LENGTH * 2.0f * cos((90.0f + _angle) * PI / 180.0f);
			end_pos.y() = start_pos.y() + SEGMENT_LENGTH * 2.0f * sin((90.0f + _angle) * PI / 180.0f);

			//For camera adjustments
			if (tree_max_y < end_pos.y()) {
				tree_max_y = end_pos.y();
			}

			//Matrices to place the segments in space with the correct orientation and position
			mat4t mtw;
			mtw.loadIdentity();
			mtw.translate(mid_pos);
			mtw.rotate(_angle, 0.0f, 0.0f, 1.0f);

			mat4t mtw2;
			mtw2.loadIdentity();
			mtw2.rotate(90, 1, 0, 0);
			
			//Different Shapes for each segments (Cylinders used in this case, option to use rectangles)
			//mesh_box *box = new mesh_box(vec3(SEGMENT_WIDTH, SEGMENT_LENGTH, SEGMENT_WIDTH), mtw);//Recatangle
			mesh_cylinder *box = new mesh_cylinder(zcylinder(vec3(0),SEGMENT_WIDTH,SEGMENT_LENGTH), mtw2*mtw);//Cylinders

			scene_node *node = new scene_node();
			app_scene->add_child(node);

			//Mesh properties
			switch (color_type)
			{
			case 1: app_scene->add_mesh_instance(new mesh_instance(node, box, material_wood));
				break;
			case 2: app_scene->add_mesh_instance(new mesh_instance(node, box, material_leaf));
				break;
			case 3: app_scene->add_mesh_instance(new mesh_instance(node, box, material_leaf2));
				break;
			case 4: app_scene->add_mesh_instance(new mesh_instance(node, box, material_black));
				break;
			case 5: app_scene->add_mesh_instance(new mesh_instance(node, box, material_autumn1));
				break;
			case 6: app_scene->add_mesh_instance(new mesh_instance(node, box, material_autumn2));
				break;
			case 7: app_scene->add_mesh_instance(new mesh_instance(node, box, material_spring1));
				break;
			case 8: app_scene->add_mesh_instance(new mesh_instance(node, box, material_spring2));
				break;
			case 9: app_scene->add_mesh_instance(new mesh_instance(node, box, material_snow));
				break;
			case 10: app_scene->add_mesh_instance(new mesh_instance(node, box, material_dark_snow));
				break;
		   
			}

			return end_pos;
		}

		//Resets some state variables
		void reset_variables(){

			current_iteration = 0;
			angle_increment = 0.0f;
			SEGMENT_WIDTH = 0.1f;
			zoom_increment = 0.0f;
			x_increment = 0.0f;
			y_increment = 0.0f;
			rotation_amount = 0.0f;
			rotation_on = false;
			auto_scale = false;
			color_type = 1;

			t.reset_stoc();

		}

		//Re-draws the L-system up to the current iteration with new user-modified parameters (color, angle, thickness)
		void re_draw(){
			if (current_example!=9){
			for (unsigned int i = 1; i <= current_iteration; ++i){
				if (is_stoc())
				{
					t.devolve_stoc(i);
				}

				else {
					t.evolve();
				}
				draw_again();
			}
		}
			//Example 9 gets redrawn from scratch each time a parameter is changed
			else{
				draw_again();
				current_iteration = 0; 

				std::cout << "\nCurrent iteration: " << current_iteration << "\n";
			}
		}

	};
}