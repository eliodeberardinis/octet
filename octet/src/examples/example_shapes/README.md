#TOOLS & MIDDLEWARE Assignment 1 - Physics Playground

##INTRODUCTION

The Project Started from "Example_Shapes" already present in Octet and providing basic OpenGL rendering for 3D shape. In particular, the scene displays 3 falling shapes.
From here I expanded the project by implementing the required features for the assignment. Demonstrating Hinge and spring constraints, setting up objects from CSV files and handling collisions callbacks.
I demonstrated this physics and technical aspects in different ways as explained in the following section.

##DEVELOPMENT

(for code details please check "example_shapes" on github/eliodeberardinis/octet on the branch "Tools_Middleware_2016")

The code is mostly divided into 2 header files "Example_Shapes.h" and "csvReading.h". The first one houses all the main variables and methods to create all the geometry, check the collisions, create the constraints and update the scenes.
The second one houses a separate class that deals with the reading, storing and interpreting a text file to set up objects in the scene.
In both cases the code is heavily commented so in this report I will focus on explaining how the requirements of the assignment have been implemented and which methods are involved leaving all the basic funcions such as geometry creation and housekeeping to the reader through the comments in the code.
In order to make the player interact with the software, I used the octet class "helper_fps_controller" to move around as in a first person shooter and interact with the world as a human player (jump, walk on structures etc). This mode is called player mode and is the default one. I also added a mode called "God", accessible by pressing "Backspace" where the player can simply move the camera around without any physics contraints like gravity, collisions etc. In this mode the experience is less realistic but feels faster and smoother.
In both cases the players will be able to shoot some spherical projectiles to further interact with the world (more on this on "Collision Callbacks" paragraph).

### Spring Constraints

To demonstrate a Spring Constraint I created 2 simple shapes: a Sphere that functions as the static element and a box functioning as the dynamic element attached to the sphere through a spring. I called it a "pendulum".
The shapes are both created when the program is initialized in "App_Init()" and the spring is created with the method "CreateSpringConstrain()". Here, I first create the frames needed to set the correct offsets from the centre of the objects at which the spring will attach, then I create the actual spring by passing the previously-created meshes and frames to the relevant function.
Finally, I set up the limits for linear and angular displacement and the spring properties such as stiffness, size and damping factor.

### Hinge Constraints and CSV reading

To demonstrate the Hinge constraint and the CSV reading/setting I created a "Suspended Bridge" through the method "CreateCSVbridge".
After reading the CSV file in App_Init() the information is stored in an array present in the class csvReading called "Variables". Players can decide how to create their bridges simply by writing 'B' if they want a bridge base or a 'p' if they want a bridge plank.
Any combination will give rise to a different bridge (e.g. B,p,p,p,p,B,p,p,p,B,B,B).
The method understands which shape to instantiate, at what distance and height depending on the subsequent element in the array. Once all the paramenters are read and the shapes added to the scene according to the user's directions the correct hinges are created by passing the relevant information to the method "new btHingeConstraint(...)".

###Collision Callbacks

To demonstrate collision callbacks I use sounds. When a projectile (created in the method "ShootProjectiles") hits certain structures in the game (the pendulum, the blue cylinder, the yellow box) a sound is triggered and played.
The method handling this is "CollisionCalls()". 
Every time a collision happens, Bullet creates a "Manifold" where it saves a current collision. By identifying the elements (rigid bodies) in this collision I detect if 2 specific objects have hit each other and trigger the relevant sound.
To do this every time I instantiate a projectile or one of the shapes involved I save its index as an int and compare it in the CollisionCalls() method

##COCLUSIONS

In this physics playground the player is able to move freely as a "human" or a "God" and interact with the objects around: walk on the bridge, hit a music box or swing a pendulum. 
The structures are present in order to show the implementation of some important physics contraints and methods through Bullet Physics.
There's obviously room for improvement. For example instantiating different kind of pendulums through CSV, improving the player's controls, the collisions and adding external libraries for the sound such as FMOD. Also, creating a Python script to generate the CSV files would have been a clever way to speed up the playground design.
Nonetheless I think I did a complete job and learned a lot more of the physics implementation of bullet in C++.
##CREDITS

Mircea Catana for helping with CSV file and hinge contraints.

Federico Soncini for helping with the fps class.

##DEMO VIDEO:

https://www.youtube.com/watch?v=uyzfqLmLshs&feature=youtu.be

##Images:

![Alt text](https://github.com/eliodeberardinis/octet/blob/Tools_Middleware_2016/octet/assets/invaderers/tm1.JPG?raw=true"Screen 1")

![Alt text](https://github.com/eliodeberardinis/octet/blob/Tools_Middleware_2016/octet/assets/invaderers/pENDULUM.JPG?raw=true"Screen 2")

![Alt text](https://github.com/eliodeberardinis/octet/blob/Tools_Middleware_2016/octet/assets/invaderers/BRIDGE.JPG?raw=true"Screen 3")



