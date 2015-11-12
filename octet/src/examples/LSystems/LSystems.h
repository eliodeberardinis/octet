////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
	/// Scene containing a box with octet.
	class Lsystem : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;
		RulesEngine rulesEngine;
		ref<camera_instance> the_camera;

		mouse_look mouse_look_helper;
		helper_fps_controller fps_helper;
		ref<scene_node> player_node;

		std::vector<Tree> *trees;
		Tree tree;

		int counter = 0;
		int framesPerStep = 20;

		std::vector<mesh_cylinder> *meshes;

		int idGen = 1;

	public:


		/// this is called when we construct the class before everything is initialised.
		Lsystem(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {

			meshes = new std::vector<mesh_cylinder>();

			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			the_camera = app_scene->get_camera_instance(0);
			the_camera->set_far_plane(10000);

			mouse_look_helper.init(this, 200.0f / 360.0f, false);
			//fps_helper.init(this);

			material *red = new material(vec4(1, 0, 0, 1));
			mesh_box *box = new mesh_box(vec3(4));
			scene_node *node = new scene_node();
			app_scene->add_child(node);
			//app_scene->add_mesh_instance(new mesh_instance(node, box, red));

			//meshes = new std::vector<mesh_cylinder>();

			rulesEngine.setAxiom("F");
			rulesEngine.addRule("F", "FF-[-F+F+F]+[+F-F-F]", 1);//rulesEngine.addRule("F", "C0F/F-[C1-F+F]+[C2+F-F]", 1);
			/*rulesEngine.addRule("X", "C0F*F++[C1+F/F]+[C2-FF]", 0.30f);
			rulesEngine.addRule("X", "C0F**F", 0.70f);*/
			//rulesEngine.addRule("]", "L]", 1); //add intermediate leaves

			////rulesEngine.addRule("FF", "F*", 0.5f);

			//basic single branch probability example
			/*rulesEngine.setAxiom("F");
			rulesEngine.addRule("F", "F+F+F", 0.6f);
			rulesEngine.addRule("F", "F-F-F", 0.4f);*/

			std::string result1 = rulesEngine.iterate();
			std::string result2 = rulesEngine.iterate();
			std::string result3 = rulesEngine.iterate();
			rulesEngine.clearRules();
			rulesEngine.addRule("]", "L]", 1); //add end leaves
			std::string result4 = rulesEngine.iterate();


			trees = new std::vector<Tree>(500);

			mat4t root;
			root.loadIdentity();
			tree.init(root, app_scene, trees, meshes, result4, &idGen, framesPerStep);
			tree.setAngle(-25);
			tree.setAngleY(20);
			//tree.setAngleY(0);
			tree.setSegmentLength(0.5f);
			tree.setSegmentThickness(0.005f);
			tree.setInitialColor(vec4(0.50196078431f, 0.50196078431f, 0, 0));
			tree.setFinalColor(vec4(0.271f, 0.192f, 0.047f, 0));
			tree.setFramesUntilFinalColor(200);

			trees->push_back(tree);



			//std::thread second(tree->Grow, result3);
			//tree->Grow();


			float player_height = 1.83f;
			float player_radius = 0.25f;
			float player_mass = 90.0f;

			mat4t mat;
			mat.loadIdentity();
			mat.translate(0, player_height*0.5f, -50);

			mesh_instance *mi = app_scene->add_shape(
				mat,
				new mesh_sphere(vec3(0), player_radius),
				new material(vec4(0, 0, 1, 1)),
				false, player_mass,
				new btCapsuleShape(0.25f, player_height)
				);
			player_node = mi->get_node();

		}

		void draw_world(int x, int y, int w, int h) {

			if (is_key_down(key_ctrl))
			{
				//new segments creation
				counter++;
				if (counter > framesPerStep)
				{
					for (int i = 0; i < trees->size(); i++)
					{
						if ((*trees)[i].enabled)
							(*trees)[i].Grow();
					}
					counter = 0;
				}

				//segment growing
				for (int i = 0; i < trees->size(); i++)
				{
					if ((*trees)[i].enabled)
						(*trees)[i].GrowSegments();
				}
			}


			app_scene->set_render_debug_lines(true);
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);

			// tumble the box  (there is only one mesh instance)
			scene_node *node = app_scene->get_mesh_instance(0)->get_node();
			node->rotate(1, vec3(1, 0, 0));
			node->rotate(1, vec3(0, 1, 0));

			scene_node *camera_node = the_camera->get_node();
			mat4t &camera_to_world = camera_node->access_nodeToParent();
			mouse_look_helper.update(camera_to_world);




			if (is_key_down(key_left))
			{
				camera_node->translate(vec3(-0.15f, 0, 0));
				//camNode->rotate(5, vec3(0, 0, 1));
			}
			if (is_key_down(key_right))
			{
				camera_node->translate(vec3(0.15f, 0, 0));
			}
			if (is_key_down(key_up))
			{
				camera_node->translate(vec3(0, 0, -0.15f));
			}
			if (is_key_down(key_down))
			{
				camera_node->translate(vec3(0, 0, 0.15f));
			}
			if (is_key_down(key_backspace))
			{
				camera_node->translate(vec3(0, 0.15f, 0));
			}
			if (is_key_down(key_alt))
			{
				camera_node->translate(vec3(0, -0.15f, 0));
			}


		}
	};
}