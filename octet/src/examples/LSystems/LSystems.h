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

			handle_input();

			app_scene->begin_render(w, h);

			//glClearColor(0, 0, 0, 1);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // changed directly in visual scene (is there another way?)

			app_scene->update(1.0f / 30.0f);

			app_scene->render((float)w / h);
		}

		int n = 1;
		unsigned int current_iteration = 0;
		const int min_example=1;
		const int MAX_example=7;

		float far_plane = 500.0f;

		void handle_input() {

			//to evolve the system
			if (is_key_going_down(key_space)) {
				++current_iteration;
				t.evolve();
				draw_again();
				std::cout << "current iteration: " << current_iteration<<"\n";
				
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
				
				if (current_example == min_example)
				{
					current_example = MAX_example;
				}
				else { --current_example; }

					t.read_text_file(current_example);

					draw_again();
					std::cout << "\ncurrent example: " << current_example << "\n";//check
				

			}


			if (is_key_down(key_shift))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.50f));
			}
			if (is_key_down(key_ctrl))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.50f));
			}
			if (is_key_down(key_left))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
			}
			if (is_key_down(key_right))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
			}

			if (is_key_down(key_up))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
			}
			if (is_key_down(key_down))
			{
				app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
			}

			if (is_key_down(key_delete))
			{
				app_scene->get_camera_instance(0)->get_node()->rotate(1.5f,2);
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

		vec3 draw_segment(vec3 start_pos, float angle) {
			vec3 mid_pos;
			vec3 end_pos;

			mid_pos.x() = start_pos.x() + SEGMENT_LENGTH * cos((angle + 90.0f) * PI / 180.0f);
			mid_pos.y() = start_pos.y() + SEGMENT_LENGTH * sin((angle + 90.0f) * PI / 180.0f);
			end_pos.x() = start_pos.x() + SEGMENT_LENGTH * 2.0f * cos((90.0f + angle) * PI / 180.0f);
			end_pos.y() = start_pos.y() + SEGMENT_LENGTH * 2.0f * sin((90.0f + angle) * PI / 180.0f);

			if (tree_max_y < end_pos.y()) {
				tree_max_y = end_pos.y();
			}

			mat4t mtw;
			mtw.loadIdentity();
			mtw.translate(mid_pos);
			mtw.rotate(angle, 0.0f, 0.0f, 1.0f);
			mesh_box *box = new mesh_box(vec3(SEGMENT_WIDTH, SEGMENT_LENGTH, SEGMENT_WIDTH), mtw);

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

		void create_geometry() {
			dynarray<char> axiom = t.get_axiom();
			vec3 pos = vec3(0.0f, 0.0f, 0.0f);
			float angle = 0.0f;
			for (unsigned int i = 0; i < axiom.size(); ++i) {
				if (axiom[i] == '+') {

					//std::cout << "\Angle: " << angle << "\n";//check
					switch (current_example)
					{
					case 1: angle += 25.7f;
						    break;

					case 2: angle += 20.0f;
						    break;
					case 3: angle += 22.5f;
						    break;
					case 4: angle += 20.0f;
						    break;
					case 5: angle += 25.7f;
						    break;
					case 6: angle += 22.5f;
						    break;
					case 7: angle += 60.0f;
						break;
					}
				}
				else if (axiom[i] == '-') {

					//std::cout << "\Angle: " << angle << "\n";//check

					switch (current_example)
					{
					case 1: angle -= 25.7f;
						break;
					case 2: angle -= 20.0f;
						break;
					case 3: angle -= 22.5f;
						break;
					case 4: angle -= 20.0f;
						break;
					case 5: angle -= 25.7f;
						break;
					case 6: angle -= 22.5f;
						break;
					case 7: angle -= 60.0f;
						break;
					}
				}
				else if (axiom[i] == '[') {
					node n = node(pos, angle);
					node_stack.push_back(n);
				}
				else if (axiom[i] == ']') {
					node n = node_stack[node_stack.size() - 1];
					node_stack.pop_back();
					angle = n.get_angle();
					pos = n.get_pos();
				}
				else if (axiom[i] == 'A') {
					
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
					pos = draw_segment(pos, angle);
				}
			}
		}

	};
}