////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//


#include "BridgePlank.h"
#include <string>

namespace octet {
  /// Scene containing a box with octet.

  class example_shapes : public app {

    // scene for drawing box
    ref<visual_scene> app_scene;
	btDiscreteDynamicsWorld* world; 

	const float PI = 3.14159;

	//camera & View and Move instances
	mouse_look mouse_look_instance;
	ref<camera_instance> main_camera;

	helper_fps_controller fps_instance;
	ref<scene_node> player_node;

	float zoom_increment = 0.0f;
	float x_increment = 0.0f;
	float y_increment = 0.0f;

	//btDiscreteDynamicsWorld *dynamics_world;

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }

	btRigidBody* firstSphere = NULL;
	btRigidBody* firstBox = NULL;


    /// this is called once OpenGL is initialized
    void app_init() {

      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      //app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
	  world = app_scene->getWorld();

	  if (this != nullptr) 
	  {
		  mouse_look_instance.init(this, 200.0f / 360, false);
		  //fps_instance.init(this);
		  printf("Entered Here\n");
	  }

	  main_camera = app_scene->get_camera_instance(0);
	  main_camera->get_node()->translate(vec3(0, 4, 0));
	  main_camera->set_far_plane(10000);

	  float player_height = 1.8f;
	  float player_radius = 0.25f;
	  float player_mass = 90.0f;

	  mat4t mat;

	  //Creating the player object as a red sphere
	  mat.loadIdentity();
	  mat.translate(0.0f, player_height*6.0f, 50.0f);

	  mesh_instance *mi2 = app_scene->add_shape(
		  mat,
		  new mesh_sphere(vec3(0), player_radius),
		  new material(vec4(1, 0, 0, 1)),
		  true, player_mass,
		  new btCapsuleShape(0.25f, player_height)
	  );
	  player_node = mi2->get_node();
	  //player_index = player_node->get_rigid_body()->getUserIndex();
	  //Finished creating a player


      material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
      material *blue = new material(vec4(0, 0, 1, 1));
	  material *black = new material(vec4(0, 0, 0, 1));

	  mat.loadIdentity();
      mat.translate(0, 20, 0);
	  //firstsphere declared globally
	  app_scene->add_shapeRB(mat, new mesh_sphere(vec3(2, 2, 2), 2), red, &firstSphere, true);

      mat.loadIdentity();
	  mat.translate(2, 5, 0);
	 //firstBox declared globally
	  app_scene->add_shapeRB(mat, new mesh_box(vec3(2, 2, 2)), red, &firstBox, true);
	  
	  //CreateHingeConstrain();
	  
	  CreateSpringConstrain();

	  create_bridge();//Change this function

      mat.loadIdentity();
      mat.translate( 3, 6, 0);
      app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, true);

      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);

	  //Following Mircea's methods (Alternative way to obtain the Rigidbody property from the object) (not needed for this)
	  btRigidBody *rb1 = NULL;
	  mesh_instance *ground = app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), black, false);
	  rb1 = ground->get_node()->get_rigid_body();
    
	}

	void HandleInput() 
	
	{
	
		//Zoom in
		if (is_key_down(key_shift))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.50f));
			//player_node->translate(vec3(0, 0, -1.50f)); //Trying to move the player (not working)
			zoom_increment -= 1.50f;
		}

		//Zoom out
		if (is_key_down(key_ctrl))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.50f));
			//player_node->translate(vec3(0, 0, 1.50f));
			zoom_increment += 1.50f;
		}

		//Move camera left
		if (is_key_down(key_left))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
			//player_node->translate(vec3(-0.5f, 0, 0.0f));
			x_increment -= 0.5f;
		}

		//Move camera right
		if (is_key_down(key_right))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
			//player_node->translate(vec3(0.5f, 0, 0.0f));
			x_increment += 0.5f;
		}

		//Move camera up
		if (is_key_down(key_up))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
			//player_node->translate(vec3(0.0f, 0.5f, 0.0f));
			y_increment += 0.5f;
		}

		//Move camera down
		if (is_key_down(key_down))
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
			//player_node->translate(vec3(0.0f, -0.5f, 0.0f));
			y_increment -= 0.5f;
		}

	}

	void CreateHingeConstrain()
	{
		//Adding Hinge Constraints

		btHingeConstraint* hinge = new btHingeConstraint(*firstSphere, *firstBox, btVector3(-3, 6, 0), btVector3(0, 10, 0), btVector3(0, 1, 0), btVector3(0, 1, 0));
		hinge->setLimit(0, 180);
		world->addConstraint(hinge);
	}

	void CreateSpringConstrain()
	{
		//add spring contraint

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(10.), btScalar(0.), btScalar(0.)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));

		btGeneric6DofSpringConstraint* pGen6DOFSpring = new btGeneric6DofSpringConstraint(*firstBox, *firstSphere, frameInA, frameInB, true);
		pGen6DOFSpring->setLinearUpperLimit(btVector3(5., 0., 0.));
		pGen6DOFSpring->setLinearLowerLimit(btVector3(-5., 0., 0.));

		pGen6DOFSpring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
		pGen6DOFSpring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));

		world->addConstraint(pGen6DOFSpring, true);
		pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));

		pGen6DOFSpring->enableSpring(0, true);
		pGen6DOFSpring->setStiffness(0, 39.478f);
		pGen6DOFSpring->setDamping(0, 0.5f);
		pGen6DOFSpring->enableSpring(5, true);
		pGen6DOFSpring->setStiffness(5, 39.478f);
		pGen6DOFSpring->setDamping(0, 0.3f);
		pGen6DOFSpring->setEquilibriumPoint();
	}


	//From Mircea. Change it
	void create_bridge() {

		float plankDistance = 0.0f;

		mesh_instance *PlankArray[4];
		btHingeConstraint *PlankHinges[3];

		mat4t mtw;
		mtw.loadIdentity();
		mtw.translate(vec3(0, 0.5f, 0));
		mesh_instance *b1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

		for (int i = 0; i < 4; ++i)
		{
			mtw.loadIdentity();
			mtw.translate(vec3(1.6f + plankDistance, 1.25f, 0.0f));
			std::string CurrentPlank = "p" + std::to_string(i + 1);

			if (i % 2 != 0)
			{
				PlankArray[i] = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.125f, 1)), new material(vec4(0, 1, 0, 1)), true, 20.0f);
			}

			else
			{
				PlankArray[i] = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.125f, 1)), new material(vec4(0, 1, 1, 1)), true, 20.0f);
			}

			plankDistance += 1.1f;
		}

		mtw.loadIdentity();
		mtw.translate(vec3(6.5f, 0.5f, 0.0f));
		mesh_instance *b2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

		// hinges
		btHingeConstraint *c1 = new btHingeConstraint(*(b1->get_node()->get_rigid_body()), *(PlankArray[0]->get_node()->get_rigid_body()),
			btVector3(1.0f, 0.5f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c1->setLimit(-PI * 0.1f, PI* 0.1f );
		world->addConstraint(c1);

		for (int i = 0; i < 3; ++i)
		{
			PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
				btVector3(0.5f, 0.125f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
				btVector3(0, 0, 1), btVector3(0, 0, 1), false);
			PlankHinges[i]->setLimit(-PI * 0.1f, PI* 0.1f);
			world->addConstraint(PlankHinges[i]);
		}

		btHingeConstraint *c5 = new btHingeConstraint(*(PlankArray[3]->get_node()->get_rigid_body()), *(b2->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.125f, 0.0f), btVector3(-1.0f, 0.5f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c5->setLimit(-PI * 0.1f,  PI* 0.1f);
		world->addConstraint(c5);
	}


    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {

      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
	 
	  HandleInput();
      app_scene->begin_render(vx, vy);

	  //update camera
	  scene_node *camera_node = main_camera->get_node();
	  mat4t &camera_to_world = camera_node->access_nodeToParent();
	  
	  if (this != nullptr)
	  {
		  
		  mouse_look_instance.update(camera_to_world);
		  //fps_instance.update(player_node, camera_node);

		 // printf("Entered Draw World no Null\n");
	  }

	  // update matrices. assume 30 fps.
	  app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);

	
    }
  };
}
