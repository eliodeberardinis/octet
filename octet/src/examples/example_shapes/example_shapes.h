////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//


#include "BridgePlank.h"

namespace octet {
  /// Scene containing a box with octet.

  class example_shapes : public app {

    // scene for drawing box
    ref<visual_scene> app_scene;
	btDiscreteDynamicsWorld* world; 

	const float PI = 3.14159;

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

      material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
      material *blue = new material(vec4(0, 0, 1, 1));

      mat4t mat;
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
	  mesh_instance *ground = app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);
	  rb1 = ground->get_node()->get_rigid_body();
    
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
		mat4t mtw;
		mtw.loadIdentity();
		mtw.translate(vec3(0, 0.5f, 0));
		mesh_instance *b1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

		mtw.loadIdentity();
		mtw.translate(vec3(1.6f, 1.25f, 0.0f));
		mesh_instance *p1 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(2.7f, 1.25f, 0.0f));
		mesh_instance *p2 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(3.8f, 1.25f, 0.0f));
		mesh_instance *p3 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(4.9f, 1.25f, 0.0f));
		mesh_instance *p4 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(6.5f, 0.5f, 0.0f));
		mesh_instance *b2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

		// hinges

		btHingeConstraint *c1 = new btHingeConstraint(*(b1->get_node()->get_rigid_body()), *(p1->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.5f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c1->setLimit(-PI * 0.1f, PI* 0.1f);
		world->addConstraint(c1);

		btHingeConstraint *c2 = new btHingeConstraint(*(p1->get_node()->get_rigid_body()), *(p2->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c2->setLimit(-PI * 0.1f, PI* 0.1f);
		world->addConstraint(c2);

		btHingeConstraint *c3 = new btHingeConstraint(*(p2->get_node()->get_rigid_body()), *(p3->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c3->setLimit(-PI * 0.1f, PI* 0.1f);
		world->addConstraint(c3);

		btHingeConstraint *c4 = new btHingeConstraint(*(p3->get_node()->get_rigid_body()), *(p4->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c4->setLimit(-PI * 0.1f, PI* 0.1f);
		world->addConstraint(c4);

		btHingeConstraint *c5 = new btHingeConstraint(*(p4->get_node()->get_rigid_body()), *(b2->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.5f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c5->setLimit(-PI * 0.1f, PI* 0.1f);
		world->addConstraint(c5);
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

	  /*firstBox->setLinearVelocity(btVector3(0, 0, 0));
	*/
    }
  };
}
