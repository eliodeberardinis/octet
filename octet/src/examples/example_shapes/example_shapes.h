////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "BridgePlank.h"
#include <string>
#include "csvReading.h"

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

	ReadCsv Read_csv;

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
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
	  world = app_scene->getWorld();

	  if (this != nullptr) 
	  {
		  mouse_look_instance.init(this, 200.0f / 360, false);
		  fps_instance.init(this);
		  printf("Entered Here\n");
	  }

	  main_camera = app_scene->get_camera_instance(0);
	  main_camera->get_node()->translate(vec3(0, 4, 0));
	  main_camera->set_far_plane(10000);

	  float player_height = 1.8f;
	  float player_radius = 0.25f;
	  float player_mass   = 90.0f;

	  Read_csv.read_file();

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
	  app_scene->add_shapeRB(mat, new mesh_sphere(vec3(2), 2), red, &firstSphere, false);

      mat.loadIdentity();
	  mat.translate(0, 15, 0);
	 //firstBox declared globally
	  app_scene->add_shapeRB(mat, new mesh_box(vec3(2, 2, 2)), red, &firstBox, true, 1.0f);
	  
	  //CreateHingeConstrain();
	  
	  CreateSpringConstrain();
	  //create_springs();

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


	void shoot()
	{
		mat4t mtw;
		mtw.translate(main_camera->get_node()->get_position());

		//btRigidBody *projRB = NULL;

		vec3 forward = -main_camera->get_node()->get_z();
		mesh_instance *projectile = app_scene->add_shape(mtw, new mesh_sphere(vec3(1), 0.2f), new material(vec4(0, 1, 0.8f, 1)), true, 1.5f);

		projectile->get_node()->apply_central_force(forward*900.0f);
		
	}

	void HandleInput() 
	
	{

		if (is_key_going_down(key_lmb)) 
		{
			shoot();
		}
	
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

	//Mircea
	void create_springs() {
		mat4t mtw;
		mtw.translate(-3, 10, 0);
		btRigidBody *rb1 = NULL;
		mesh_instance *mi1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);
		rb1 = mi1->get_node()->get_rigid_body();

		mtw.loadIdentity();
		mtw.translate(-5, 8, 0);
		btRigidBody *rb2 = NULL;
		mesh_instance *mi2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(0, 1, 0, 1)), true, 1.0f);
		rb2 = mi2->get_node()->get_rigid_body();

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(-0.5f), btScalar(0.0f)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(0.5f), btScalar(0.0f)));

		btGeneric6DofSpringConstraint *c1 = new btGeneric6DofSpringConstraint(*rb1, *rb2, frameInA, frameInB, true);
		c1->setLinearUpperLimit(btVector3(0., 5.0f, 0.));
		c1->setLinearLowerLimit(btVector3(0., -5.0f, 0.));

		c1->setAngularLowerLimit(btVector3(-1.5f, -1.5f, 0));
		c1->setAngularUpperLimit(btVector3(1.5f, 1.5f, 0));

		world->addConstraint(c1, false);

		c1->setDbgDrawSize(btScalar(5.f));
		c1->enableSpring(0, true);
		c1->setStiffness(0, 10.0f);
		c1->setDamping(0, 0.5f);
	}

	//Elio (Migno)
	void CreateSpringConstrain()
	{
		//add spring contraint

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.), btScalar(-2.), btScalar(0.)));

		btGeneric6DofSpringConstraint* pGen6DOFSpring = new btGeneric6DofSpringConstraint(*firstBox, *firstSphere, frameInA, frameInB, true);
		pGen6DOFSpring->setLinearUpperLimit(btVector3(0., 5., 0.));
		pGen6DOFSpring->setLinearLowerLimit(btVector3(0., -5., 0.));

		pGen6DOFSpring->setAngularLowerLimit(btVector3(-1.5f, -1.5f, -1.5f));
		pGen6DOFSpring->setAngularUpperLimit(btVector3(1.5f, 1.5f, 1.5f));

		world->addConstraint(pGen6DOFSpring, true);
		pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));

		pGen6DOFSpring->enableSpring(0, true);
		pGen6DOFSpring->setStiffness(0, 10.0f);
		pGen6DOFSpring->setDamping(0, 0.3f);

		//pGen6DOFSpring->enableSpring(5, true);
		//pGen6DOFSpring->setStiffness(5, 39.478f);
		//pGen6DOFSpring->setDamping(5, 0.3f);

		//pGen6DOFSpring->setEquilibriumPoint();
	}
	
	void create_bridge() {

		float plankDistance = 0.0f;
		int numPlanks = 7;
		int HingeType = 1;
		bool useCSV = true;

		mesh_instance *PlankArray[50];
		btHingeConstraint *PlankHinges[50];

		if (useCSV)
		{
			//create and add meshes read from Csv file
			for (int i = 0; i < Read_csv.variables.size(); ++i)
			{
				mat4t mtw;
				mtw.loadIdentity();

				if (Read_csv.variables[i] == 'B')
				{

					mtw.translate(vec3(plankDistance, 0.5f, 0));
					PlankArray[i] = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);
					plankDistance += 0.5f;

					if (Read_csv.variables[i + 1] == 'B')
					{
						plankDistance += 0.5f;
					}
				}

				else if (Read_csv.variables[i] == 'p')
				{
					mtw.translate(vec3(plankDistance, 1.25f, 0));

					if (i % 2 != 0)
					{
						PlankArray[i] = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.125f, 1)), new material(vec4(0, 1, 0, 1)), true, 20.0f);
					}

					else
					{
						PlankArray[i] = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.125f, 1)), new material(vec4(0, 1, 1, 1)), true, 20.0f);
					}

					if (Read_csv.variables[i + 1] == 'B')
					{
						plankDistance += 0.5f;
					}
				}

				plankDistance += 1.1f;
			}

			// create hinges
			for (int i = 0; i < Read_csv.variables.size() - 1; ++i)
			{
				if (Read_csv.variables[i] == 'B' && Read_csv.variables[i + 1] == 'p')
				{
					HingeType = 1;
				}

				else if (Read_csv.variables[i] == 'p' && Read_csv.variables[i + 1] == 'p')
				{
					HingeType = 2;
				}

				else if (Read_csv.variables[i] == 'p' && Read_csv.variables[i + 1] == 'B')
				{
					HingeType = 3;
				}

				else if (Read_csv.variables[i] == 'B' && Read_csv.variables[i + 1] == 'B')
				{
					HingeType = 4;
				}

				switch (HingeType)
				{

				case 1:
					PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
						btVector3(1.0f, 0.5f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
						btVector3(0, 0, 1), btVector3(0, 0, 1), false);
					break;

				case 2:

					PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
						btVector3(0.5f, 0.125f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
						btVector3(0, 0, 1), btVector3(0, 0, 1), false);
					break;

				case 3:
					PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
						btVector3(0.5f, 0.125f, 0.0f), btVector3(-1.0f, 0.5f, 0.0f),
						btVector3(0, 0, 1), btVector3(0, 0, 1), false);
					break;

				case 4:
					PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
						btVector3(1.0f, 0.5f, 0.0f), btVector3(-1.0f, 0.5f, 0.0f),
						btVector3(0, 0, 1), btVector3(0, 0, 1), false);
					break;
				}

				PlankHinges[i]->setLimit(-PI * 0.1f, PI* 0.1f);
				world->addConstraint(PlankHinges[i]);
			}
		}

		else {
			//Base 1
			mat4t mtw;
			mtw.loadIdentity();
			mtw.translate(vec3(0, 0.5f, 0));
			mesh_instance *b1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

			//Planks
			for (int i = 0; i < numPlanks; ++i)
			{
				mtw.loadIdentity();
				mtw.translate(vec3(1.6f + plankDistance, 1.25f, 0.0f));

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

			//Base 2
			mtw.loadIdentity();
			mtw.translate(vec3(1.6f + plankDistance + 0.5f, 0.5f, 0.0f));
			mesh_instance *b2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

			//hinges
			//First hinge
			btHingeConstraint *c1 = new btHingeConstraint(*(b1->get_node()->get_rigid_body()), *(PlankArray[0]->get_node()->get_rigid_body()),
				btVector3(1.0f, 0.5f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
				btVector3(0, 0, 1), btVector3(0, 0, 1), false);
			c1->setLimit(-PI * 0.1f, PI* 0.1f);
			world->addConstraint(c1);

			//In Between hinges
			for (int i = 0; i < numPlanks - 1; ++i)
			{
				PlankHinges[i] = new btHingeConstraint(*(PlankArray[i]->get_node()->get_rigid_body()), *(PlankArray[i + 1]->get_node()->get_rigid_body()),
					btVector3(0.5f, 0.125f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
					btVector3(0, 0, 1), btVector3(0, 0, 1), false);
				PlankHinges[i]->setLimit(-PI * 0.1f, PI* 0.1f);
				world->addConstraint(PlankHinges[i]);
			}

			//Last Hinge
			btHingeConstraint *c5 = new btHingeConstraint(*(PlankArray[numPlanks - 1]->get_node()->get_rigid_body()), *(b2->get_node()->get_rigid_body()),
				btVector3(0.5f, 0.125f, 0.0f), btVector3(-1.0f, 0.5f, 0.0f),
				btVector3(0, 0, 1), btVector3(0, 0, 1), false);
			c5->setLimit(-PI * 0.1f, PI* 0.1f);
			world->addConstraint(c5);
		}
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
