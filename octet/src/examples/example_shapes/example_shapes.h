////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "csvReading.h"

namespace octet {
  /// Scene containing a box with octet.

  class example_shapes : public app {

    // scene for drawing box
    ref<visual_scene> app_scene;
	btDiscreteDynamicsWorld* world; 

	//Constant to set rotation limits
	const float PI = 3.14159;

	//camera & View and Move instances
	mouse_look mouse_look_instance;
	ref<camera_instance> main_camera;

	//Instance of the fps_controller
	helper_fps_controller fps_instance;

	//References to the scene nodes of the projectiles and the player
	ref<scene_node> player_node;
	ref<scene_node> projectile_node;

	//Parameters for Camera zooming and movement
	float zoom_increment = 0.0f;
	float x_increment = 0.0f;
	float y_increment = 0.0f;

	//Parameters to store data of the projectiles
	mesh_instance *ProjectilesArray[5];
	int numProjectiles = 0;
	int projectileIndex;

	//Instance of the ReadCsv class #included as external .h file
	ReadCsv Read_csv;

	// Sound effects to play on hitting some things in the demo
	int soundsIndex;
	int playerIndex;
	int hangBoxIndex;

	ALuint sound;
	ALuint sound2;
	unsigned int soundSource;
	unsigned int numSoundSources = 32;
	ALuint sources[32];
	bool playSound;

	//Frame Counter
	int framePassed = 0;

	//Declaration of global RigidBodies parameters for the pendlum example
	btRigidBody* PendantSphere = NULL;
	btRigidBody* PendantBox = NULL;

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }

    /// this is called once OpenGL is initialized
    void app_init() {

      //Initial settings for camera, light and geometry
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));
	  world = app_scene->getWorld();

	  //Initializing mouse pointer and fps_instance
	  mouse_look_instance.init(this, 200.0f / 360, false);
      fps_instance.init(this);

	  //Translating camera and setting clipping planes
	  main_camera = app_scene->get_camera_instance(0);
	  main_camera->get_node()->translate(vec3(0, 4, 0));
	  main_camera->set_far_plane(10000);

	  //Setting the Player's "body" dimensions
	  float player_height = 1.8f;
	  float player_radius = 0.25f;
	  float player_mass   = 90.0f;

	  //Read the CSV file to create the bridge and store the data in arrays
	  Read_csv.read_file();

	  //Creating the player object as a red sphere
	  mat4t mat;
	  mat.loadIdentity();
	  mat.translate(0.0f, player_height*6.0f, 50.0f);

	  mesh_instance *mi2 = app_scene->add_shape(
		  mat,
		  new mesh_sphere(vec3(0), player_radius),
		  new material(vec4(1, 0, 0, 1)),
		  true, player_mass,
		  new btCapsuleShape(0.25f, player_height)
	  );

	  //Obtaining the Player Index to use for collision detection
	  player_node = mi2->get_node();
	  playerIndex = player_node->get_rigid_body()->getUserIndex();

	  //Creating the Music player as a yellow box
	  mat.loadIdentity();
	  mat.translate(vec3(-8, 1, -5));
	  mesh_instance *mi3 = app_scene->add_shape(mat, new mesh_box(vec3(2)), new material(vec4(1, 1, 0.0f, 1)), false);
	  soundsIndex = mi3->get_node()->get_rigid_body()->getUserIndex();

	  //Initializing Music player
	  sound = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");
	  sound2 = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/whoosh.wav");
	  soundSource = 0;
	  alGenSources(numSoundSources, sources);
	  playSound = true;

	  //Inizializing some materials used in the Demo for simplicity
      material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
      material *blue = new material(vec4(0, 0, 1, 1));
	  material *black = new material(vec4(0, 0, 0, 1));

	  //Creating the Spring Pendulum Sphere (Static Object)
	  mat.loadIdentity();
      mat.translate(0, 20, 0);
	  app_scene->add_shapeRB(mat, new mesh_sphere(vec3(2), 2), blue, &PendantSphere, false);

	  //Creating the spring pendulum Box (Moving Object)
      mat.loadIdentity();
	  mat.translate(0, 15, 0);

	  //Obtaining the pendulum Box index to use for collision detection
	  mesh_instance *hangBox = app_scene->add_shapeRB(mat, new mesh_box(vec3(2, 2, 2)), red, &PendantBox, true, 1.0f);
	  hangBoxIndex = hangBox->get_node()->get_rigid_body()->getUserIndex();
	  	  
	  //Create a spring Contrain between the Sphere and the Box, Creating a pendulum
	  CreateSpringConstrain();

	  //Used to demonstrate Hinge Contraints and CSV-reading and instantiation
	  create_bridge();

	  //Blue cylinder (another object to interact with in the scene)
      mat.loadIdentity();
      mat.translate( 3, 6, 0);
      app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, true);

      // Terrain
      mat.loadIdentity();
      mat.translate(0, -1, 0);
	  btRigidBody *rb1 = NULL;
	  mesh_instance *ground = app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), black, false);
	  rb1 = ground->get_node()->get_rigid_body();

	}

	//Function used to obtain the sound to play
	ALuint GetSoundSource() {
		soundSource = soundSource % numSoundSources;
		soundSource++;
		return sources[soundSource];
	}

	//Function used to detect collissions and play sounds in response
	void CollisionsCall() {

		//Manifolds are created in Bullet and represent active collisions
		int num_manifolds = world->getDispatcher()->getNumManifolds();

		//For Every active manifold we obtain the indexes of the objects involed in the collision
		for (unsigned int i = 0; i < num_manifolds; ++i) {
			btPersistentManifold *manifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			int index0 = manifold->getBody0()->getUserIndex();
			int index1 = manifold->getBody1()->getUserIndex();

			//We check if the 2 indexes involved are from the projectiles and the pendulum and we play a type of sound
			if (index0 == projectileIndex|| index1 == projectileIndex) {
				if (index0 == hangBoxIndex || index1 == hangBoxIndex) {
					if (playSound) {
						ALuint source = GetSoundSource();
						alSourcei(source, AL_BUFFER, sound2);
						alSourcePlay(source);
						playSound = false;
					}
				}
			}

			//If instead we hit another object such as the yellow musicBox we play another sound
			if (index0 == projectileIndex || index1 == projectileIndex) {
				if (index0 == soundsIndex || index1 == soundsIndex) {
					if (playSound) {
						ALuint source = GetSoundSource();
						alSourcei(source, AL_BUFFER, sound);
						alSourcePlay(source);
						playSound = false;
					}
				}
			}
		}
	}

	//Function that creates small projectiles and adds forces to it to simulate projectiles
	void ShootProjectiles()
	{
		//We set the spawn position and the direction of the projectiles to the camera
		mat4t mtw;
		mtw.translate(main_camera->get_node()->get_position());
		vec3 forward = -main_camera->get_node()->get_z();

		//Projectiles are instantiated as set of 5 and saved in an array where we add a force and obtain their indexes for collisions
		if (numProjectiles < 5)
		{
			ProjectilesArray[numProjectiles] = app_scene->add_shape(mtw, new mesh_sphere(vec3(1), 0.2f), new material(vec4(0, 1, 0.8f, 1)), true, 0.5f);
			projectile_node = ProjectilesArray[numProjectiles]->get_node();
			projectileIndex = projectile_node->get_rigid_body()->getUserIndex();
			ProjectilesArray[numProjectiles]->get_node()->apply_central_force(forward*300.0f);
			numProjectiles++;
		}

		//After a set of 5 is created the first 5 are deleted.
		else
		{
			for (int i = 0; i < 5; ++i)
			{
				app_scene->delete_mesh_instance(ProjectilesArray[i]->get_mesh_instance());//Not working
			}

			numProjectiles = 0;
		}

		
	}

	//Function that handles camera control (in God Mode) and the shooting. Moving of the player in Player-Mode is handled by fps_instance
	void HandleInput() 
	
	{
		//Shoot the projectiles
		if (is_key_going_down(key_lmb)) 
		{
			ShootProjectiles();
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
	}

	//Function that creates a spring contraint between the box and the sphere pendulum
	void CreateSpringConstrain()
	{
		//Frames are needed in bullet to define the offset at which the spring attaches to the objects
		btTransform frameInA, frameInB;

		//We set the offset for the 2 objects
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.), btScalar(-2.), btScalar(0.)));

		//Create the Actual Contraint between the two meshes with the offsets set in the Frames
		btGeneric6DofSpringConstraint* pGen6DOFSpring = new btGeneric6DofSpringConstraint(*PendantBox, *PendantSphere, frameInA, frameInB, true);
		
		//Setting Linear Limits
		pGen6DOFSpring->setLinearUpperLimit(btVector3(0., 5., 0.));
		pGen6DOFSpring->setLinearLowerLimit(btVector3(0., -5., 0.));

		//Setting Rotational Limits
		pGen6DOFSpring->setAngularLowerLimit(btVector3(-1.5f, -1.5f, -1.5f));
		pGen6DOFSpring->setAngularUpperLimit(btVector3(1.5f, 1.5f, 1.5f));

		//Adding the Contraint to the world
		world->addConstraint(pGen6DOFSpring, true);

		//Setting some additional physics parameters such as size, stiffness and damping
		pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));
		pGen6DOFSpring->enableSpring(0, true);
		pGen6DOFSpring->setStiffness(0, 10.0f);
		pGen6DOFSpring->setDamping(0, 0.3f);
	}
	
	//This function demonstrate the Hinge Constraint and the CSV reading and instantiation with a simulation of a suspended bridge.
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
      app_scene->begin_render(vx, vy);

	  HandleInput();

	  CollisionsCall();

	  if (++framePassed > 60) {
		  framePassed = 0;
		  playSound = true;
	  }

	  //update camera
	  scene_node *camera_node = main_camera->get_node();
	  mat4t &camera_to_world = camera_node->access_nodeToParent();
	  
      mouse_look_instance.update(camera_to_world);
	  fps_instance.update(player_node, camera_node);

	  // update matrices. assume 30 fps.
	  app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);

	
    }
  };
}
