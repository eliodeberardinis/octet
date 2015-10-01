////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class example_box : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
  public:
    /// this is called when we construct the class before everything is initialised.
    example_box(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      material *green = new material(vec4(146.0f/256, 212.0f/256, 144.0f/256, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
      mesh *box = new mesh_box(vec3(4));
      scene_node *node = new scene_node();
	  scene_node *node2 = new scene_node();
	  node2->translate(vec3(8, 0, 0));
      app_scene->add_child(node);
	  app_scene->add_child(node2);
      app_scene->add_mesh_instance(new mesh_instance(node, box, green));
	  app_scene->add_mesh_instance(new mesh_instance(node2, box, blue));
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the box  (there is only one mesh instance)
      scene_node *node = app_scene->get_mesh_instance(0)->get_node();
      node->rotate(9, vec3(1, 0, 0));
	  node->rotate(9, vec3(0, 1, 0));
      
	  scene_node *node2 = app_scene->get_mesh_instance(1)->get_node();
	  node2->rotate(1, vec3(1, 0, 0));
	  node2->rotate(0.5, vec3(0, 0, 1));
    }
  };
}
