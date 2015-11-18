////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "tree_elio.h"


namespace octet {
	class lsystems : public app {

		class node {
			vec3 pos;
			float angle;
		public:
			node() {
				pos = vec3(0.0f, 0.0f, 0.0f);
				angle = 0.0f;
			}

			node(vec3 pos_, float angle_) {
				pos = pos_;
				angle = angle_;
			}

			vec3& get_pos() {
				return pos;
			}

			float& get_angle() {
				return angle;
			}
		};

		const float PI = 3.14159265f;
		const float SEGMENT_LENGTH = 0.5f;
		float SEGMENT_WIDTH = 0.1f;

		ref<visual_scene> app_scene;

		tree t;

		dynarray<node> node_stack;

		float tree_max_y = 0.0f;

		material *material_wood;
		material *material_leaf;
		material *material_leaf2;
		material *material_white;

		int current_example = 1;//create a function to change this. 

		int n = 1; // index for material/color to use
		unsigned int current_iteration = 0;
		const int min_example = 1;
		const int MAX_example = 7;
		unsigned int MAX_iteration = 10;
		float far_plane = 500.0f;

	public:
		lsystems(int argc, char **argv) : app(argc, argv) {
		}

		void app_init() {
			t.read_text_file(current_example);

			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.0f, 1.0f));

			
			material_wood = new material(vec4(0.39f, 0.20f, 0.0f, 1.0f));//brown wood
			material_leaf = new material(vec4(0.0f, 0.30f, 0.0f, 1.0f)); //green leaf
			material_leaf2 = new material(vec4(0.0f, 0.45f, 0.0f, 1.0f)); //light green leaf
			material_white = new material(vec4(1.0f, 1.0f, 1.0f, 1.0f)); //white

			create_geometry();
		}

		void draw_world(int x, int y, int w, int h) {

			set_MAX_iteration();
			handle_input();
			app_scene->begin_render(w, h);

			app_scene->update(1.0f / 30.0f);

			app_scene->render((float)w / h);
		}

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
			}

		
		}

	
		void handle_input() {

			//evolve the system
			if (is_key_going_down(key_space) && current_iteration < MAX_iteration) {
				++current_iteration;
				t.evolve();
				draw_again();
				std::cout << "current iteration: " << current_iteration<<"\n";
				
			}
			else if (is_key_going_down(key_space) && current_iteration >= MAX_iteration)
			{
				std::cout << "\nMaximum number of iterations reached!";
				
			}

			// devolve the system
			if (is_key_going_down(key_backspace) && current_iteration > 0) {

				if (current_iteration > 1){

					t.read_text_file(current_example);

					for (unsigned int i = 1; i <= current_iteration - 1; i++){
						t.evolve();
						draw_again();
					}
					current_iteration--;
					std::cout << "current iteration: " << current_iteration << "\n";
				}

				else if (current_iteration <= 1){
						t.read_text_file(current_example);
						draw_again();
						current_iteration--;
						std::cout << "current iteration read zero: " << current_iteration << "\n";
					   }
				
			}

			if (is_key_going_down(key_lmb)) {

				current_iteration = 0;
				angle_increment = 0.0f;
				
				if (current_example == MAX_example)
				{
					current_example = min_example;
				}
				else { ++current_example; }

					t.read_text_file(current_example);

					draw_again();
					std::cout << "\nCurrent example: " << current_example << "\n";// check
				
			}

			if (is_key_going_down(key_rmb)) {

				current_iteration = 0;
				angle_increment = 0.0f;
				
				if (current_example == min_example)
				{
					current_example = MAX_example;
				}
				else { --current_example; }

					t.read_text_file(current_example);

					draw_again();
					std::cout << "\ncurrent example: " << current_example << "\n";//check
				

			}

			//Zoom in
			if (is_key_down(key_shift))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.50f));
			}

			//Zoom out
			if (is_key_down(key_ctrl))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.50f));
			}

			//Move camera left
			if (is_key_down(key_left))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
			}

			//Move camera right
			if (is_key_down(key_right))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
			}

			//Move camera up
			if (is_key_down(key_up))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
			}

			//Move camera down
			if (is_key_down(key_down))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
			}

			//Rotation of the model
			if (is_key_down(key_delete))
			{
				
				for (int i = 0; i < app_scene->get_num_mesh_instances(); ++i) {
					mesh_instance *mi = app_scene->get_mesh_instance(i);
					mi->get_node()->rotate(2.0f, vec3(0, 1, 0));
				}
			}

			//increase angle
			if (is_key_down(key_esc) && current_iteration > 0 && current_iteration <= MAX_iteration-1) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					angle_increment += 1.5f;

					for (unsigned int i = 1; i <= current_iteration; i++){
						t.evolve();
						draw_again();
					}
				}

			}

			//decrese angle
			if (is_key_down(key_tab) && current_iteration > 0 && current_iteration <= MAX_iteration - 1) {

				if (current_iteration >= 1){

					t.read_text_file(current_example);
					angle_increment -= 1.5f;

					for (unsigned int i = 1; i <= current_iteration; i++){
						t.evolve();
						draw_again();
					}
				}

			}
		}

		void draw_again(){

			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();

			//setting the far plane further away so the tree doesn't disappear when zooming out
			app_scene->get_camera_instance(0)->set_far_plane(far_plane);

			tree_max_y = 0.0f;
			create_geometry();

			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, tree_max_y / 2.0f, 0.0f));
		
		}

		vec3 draw_segment(vec3 start_pos, float _angle) {
			vec3 mid_pos;
			vec3 end_pos;

			mid_pos.x() = start_pos.x() + SEGMENT_LENGTH * cos((_angle + 90.0f) * PI / 180.0f);
			mid_pos.y() = start_pos.y() + SEGMENT_LENGTH * sin((_angle + 90.0f) * PI / 180.0f);
			end_pos.x() = start_pos.x() + SEGMENT_LENGTH * 2.0f * cos((90.0f + _angle) * PI / 180.0f);
			end_pos.y() = start_pos.y() + SEGMENT_LENGTH * 2.0f * sin((90.0f + _angle) * PI / 180.0f);

			if (tree_max_y < end_pos.y()) {
				tree_max_y = end_pos.y();
			}

			mat4t mtw;
			mtw.loadIdentity();
			mtw.translate(mid_pos);
			mtw.rotate(_angle, 0.0f, 0.0f, 1.0f);

			mat4t mtw2;
			mtw2.loadIdentity();
			mtw2.rotate(90, 1, 0, 0);

			//mesh_box *box = new mesh_box(vec3(SEGMENT_WIDTH, SEGMENT_LENGTH, SEGMENT_WIDTH), mtw);
			mesh_cylinder *box = new mesh_cylinder(zcylinder(vec3(0),SEGMENT_WIDTH,SEGMENT_LENGTH), mtw2*mtw);

			scene_node *node = new scene_node();
			app_scene->add_child(node);

			if (n == 1){
				app_scene->add_mesh_instance(new mesh_instance(node, box, material_wood));
			}
			if (n==2) {
				app_scene->add_mesh_instance(new mesh_instance(node, box, material_leaf)); 
			}
			if (n == 3) {
				app_scene->add_mesh_instance(new mesh_instance(node, box, material_leaf2));
			}

			if (n == 4) {
				app_scene->add_mesh_instance(new mesh_instance(node, box, material_white));
			}

			return end_pos;
		}

		float angle_start = 0.0f;
		float angle_increment = 0.0f;

		void create_geometry() {
			dynarray<char> axiom = t.get_axiom();
			vec3 pos = vec3(0.0f, 0.0f, 0.0f);
			
			for (unsigned int i = 0; i < axiom.size(); ++i) {
				if (axiom[i] == '+') {

					
					switch (current_example)
					{
					case 1: angle_start += (25.7f + angle_increment);
						    break;
					case 2: angle_start += (20.0f + angle_increment);
						    break;
					case 3: angle_start += (22.5f + angle_increment);
						    break;
					case 4: angle_start += (20.0f + angle_increment);
						    break;
					case 5: angle_start += (25.7f +angle_increment);
 					        break;
					case 6: angle_start += (22.5f +angle_increment);
						    break;
					case 7: angle_start += (60.0f +angle_increment);
						    break;
					}


				}
				else if (axiom[i] == '-') {

					switch (current_example)
					{
					case 1: angle_start -= (25.7f + angle_increment);
						    break;
					case 2: angle_start -= (20.0f + angle_increment);
						    break;
					case 3: angle_start -= (22.5f + angle_increment);
						    break;
					case 4: angle_start -= (20.0f + angle_increment);
						    break;
					case 5: angle_start -= (25.7f + angle_increment);
						    break;
					case 6: angle_start -= (22.5f + angle_increment);
						    break;
					case 7: angle_start -= (60.0f + angle_increment);
						    break;
					}
				}
				else if (axiom[i] == '[') {
					node n = node(pos, angle_start);
					node_stack.push_back(n);
				}
				else if (axiom[i] == ']') {
					node n = node_stack[node_stack.size() - 1];
					node_stack.pop_back();
					angle_start = n.get_angle();
					pos = n.get_pos();
				}
				else if (axiom[i] == 'A') {
					//SEGMENT_WIDTH = 0.2f;
					
					n = 1;
					
				}
				else if (axiom[i] == 'B') {
					//SEGMENT_WIDTH = 0.1f;
					n = 2;

				}
				else if (axiom[i] == 'C') {
					//SEGMENT_WIDTH = 0.1f;
					n = 3;

				}
				else if (axiom[i] == 'D') {
					//SEGMENT_WIDTH = 0.1f;
					n = 4;

				}
				else if (axiom[i] == 'F') {

					//if (angle == 0.0f && pos.x() <= 0.0f && pos.x()>-0.001f && pos.y() <= (tree_max_y / 2.0f) /*&& n==1*/){
					//	SEGMENT_WIDTH = 0.2f;
					//}
					pos = draw_segment(pos, angle_start);
				}
			}
		}

	};
}