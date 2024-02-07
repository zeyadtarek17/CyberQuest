#include "TextureBuilder.h"
#include <cmath>
#include "Model_3DS.h"
#include "GLTexture.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <glut.h>
#include <string>
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


int WIDTH = 1280;
int HEIGHT = 720;

boolean gameEnded = false;
GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 1000;


// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_energy;
Model_3DS model_powerup;
Model_3DS model_key;
Model_3DS model_portal;
Model_3DS model_player;
Model_3DS model_coin;
Model_3DS model_coin2;
Model_3DS model_coin3;
Model_3DS model_energy2;
Model_3DS model_energy3;


//GLTextures
GLTexture tex_ground;
GLTexture tex_ground2;

GLTexture tex_wall;

GLTexture tex_wall2;
GLuint texs;
GLuint texw;

int thirdpx = 0;
int thirdpy = 3;
int thirdpz = 27;
int firstpz = -0.1;
int firstpy = 2.5;
int level = 1;
int collide = 0;

boolean isFirst = false;
boolean backward = false;

inline double DEG2RAD(double degrees) {
	return degrees * (M_PI / 180.0);
}
class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
	Vector operator -(const Vector& other) const {
		return Vector(x - other.x, y - other.y, z - other.z);
	}
	Vector operator *(float scalar) const {
		return Vector(x * scalar, y * scalar, z * scalar);
	}
	Vector operator +(const Vector& other) const {
		return Vector(x + other.x, y + other.y, z + other.z);
	}
	Vector crossProd(const Vector& other) const {
		return Vector(y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	// Method to normalize the vector
	Vector unit() const {
		GLdouble length = sqrt(x * x + y * y + z * z);
		return Vector(x / length, y / length, z / length);
	}
};

Vector Eye(thirdpx, thirdpy, thirdpz);
Vector At(0, 2.5, -20);
Vector Up(0, 1, 0);

void updateCamera(); // Forward declaration

class Player {
public:
	float posx;
	float posz;
	int angle;
	int lives;
	int energyCollected;
	boolean keycollected;
	Model_3DS playerModel = model_player;


	Player(float x = 0, float z = 0, int setAngle = 180, int setlives = 3, int setenergyCollected = 0)
	{
		posx = 0;
		posz = 20;
		angle = setAngle;
		lives = setlives;
		energyCollected = setenergyCollected;
		keycollected = false;
	}

	/*void draw() {
		playerModel.pos.x = posx;
		playerModel.pos.z = posz;
		playerModel.rot.y = 180 + angle;
		playerModel.scale = 0.025;
		this->playerModel.Draw();
	}*/


	void rotateLeft() {
		angle += 3;
	}
	void rotateRight() {
		angle -= 3;
	}
	void moveForward() {
		angle = 180;
		float newPosZ = posz - 0.2;
		
		// Check if the new position is within the bounds
		if (newPosZ >= -20 && newPosZ <= 20) {
			posz = newPosZ;
			updateCamera();
		}
	}

	void moveBackward() {
		angle = 0;
		float newPosZ = posz + 0.2;
		
		// Check if the new position is within the bounds
		if (newPosZ >= -20 && newPosZ <= 20) {
			posz = newPosZ;
			updateCamera();
		}
	}

	void moveLeft() {
		angle = 270;
		float newPos = posx - 0.2;
		
		// Check if the new position is within the bounds
		if (newPos >= -20 && newPos <= 20) {
			posx = newPos;
			updateCamera();
		}
	}

	void moveRight() {
		angle = 90;
		float newPos = posx + 0.2;

		// Check if the new position is within the bounds
		if (newPos >= -20 && newPos <= 20) {
			posx = newPos;
			updateCamera();
		}
	}
};
Player player;



class Wall {
public:
	float posx;
	float posz;
	boolean decrement;

	Wall(float x, float z) {
		posx = x;
		posz = z;
		decrement = false;
	}

	void draw()
	{
		glDisable(GL_LIGHTING); // Disable lighting 

		glColor3f(0.6, 0.6, 0.6); // Dim the texture color a bit

		glEnable(GL_TEXTURE_2D); // Enable 2D texturing

		if (level == 1)
		{
			glBindTexture(GL_TEXTURE_2D, tex_wall.texture[0]); // Bind the desired texture
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, tex_wall2.texture[0]); // Bind the desired texture


		}
		
		glPushMatrix();
		glTranslated(posx, 1.2, posz);
		glScaled(7, 4, 1);

		glBegin(GL_QUADS); // Start drawing the cube

		// Front Face
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

		// Back Face
		glNormal3f(0.0, 0.0, -1.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, -0.5);

		// Top Face
		glNormal3f(0.0, 1.0, 0.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, 0.5);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);

		// Bottom Face
		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.5, -0.5, -0.5);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);

		// Right face
		glNormal3f(1.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
		glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, 0.5);

		// Left Face
		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);
		glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

		glEnd(); // End of drawing the cube
		glPopMatrix();

		glEnable(GL_LIGHTING); // Enable lighting again for other entities coming through the pipeline.

		glColor3f(1, 1, 1); // Reset material color to white
	}
	void move()
	{
		if (decrement)
		{
			posx -= 0.1;
		}
		else
		{
			posx += 0.1;
		}
	}
};
Wall wall1(-8,12);
Wall wall2(2, 2);
Wall wall3(9, -5);


void updateCamera() {
	if (!isFirst)
	{
		const float distanceBehindPlayer = 5.0f; // Distance behind the player
		const float cameraHeight = 5.0f; // Height of the camera above the player
		const float radianAngle = (player.angle * M_PI) / 180.0f; // Convert angle to radians

		// Calculate the camera's position relative to the player
		Eye.x = player.posx - distanceBehindPlayer * sin(radianAngle);
		Eye.y = cameraHeight;
		Eye.z = player.posz - distanceBehindPlayer * cos(radianAngle);

		// Point the camera towards the player
		At.x = player.posx;
		At.y = 2.5f; // Adjust if necessary to focus on a specific part of the player model
		At.z = player.posz;

		// Update the camera
		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	}
	else
	{
		// First-person camera settings
		if (backward) {
			Eye.z = player.posz +0.2;
		}
		else
		{
			Eye.z = player.posz - 0.3;
		}
		Eye.x = player.posx ;
		Eye.y = 3.0f; // You might need to define player's height
		const float radianAngle = (player.angle * M_PI) / 180.0f; // Convert angle to radians
		const float distanceBehindPlayer = 5.0f; // Distance behind the player

		At.x = player.posx + distanceBehindPlayer * sin(radianAngle);
		At.y = 2.0f;
		At.z = player.posz + distanceBehindPlayer * cos(radianAngle);

		// Update the camera
		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	}
	
}

void makeThePlayerReturn()
{
	if (collide == 1)
	{
		player.lives -= 1;
		collide = 0;
		player.posx = 0;
		player.posz = 20;
		player.angle = 180;
		updateCamera();
	}
	else if (collide == 2)
	{
		player.lives -= 1;
		collide = 0;
		player.posx = 0;
		player.posz = 9;
		player.angle = 180;
		updateCamera();
	}
	else if (collide == 3)
	{
		player.lives -= 1;
		collide = 0;
		player.posx = 0;
		player.posz = 0;
		player.angle = 180;
		updateCamera();
	}

	if (player.lives == 0)
	{
		gameEnded = true;
	}

}


bool checkCollisionLevel1() {
	
		// Define the player's bounding box
		float playerMinX = player.posx - 1.0; // Adjust these values based on your player's size
		float playerMaxX = player.posx + 1.0;
		float playerMinZ = player.posz - 1.0;
		float playerMaxZ = player.posz + 1.0;

		// Check each wall
		Wall walls[] = { wall1, wall2, wall3 };
		int no = 1;
		for (const Wall& wall : walls) {
			// Define the wall's bounding box
			float wallMinX = wall.posx - 3.5; // Adjust these values based on your wall's size
			float wallMaxX = wall.posx + 3.5;
			float wallMinZ = wall.posz - 0.5;
			float wallMaxZ = wall.posz + 0.5;

			// Check for overlap
			if (playerMaxX > wallMinX && playerMinX < wallMaxX && playerMaxZ > wallMinZ && playerMinZ < wallMaxZ) {
				collide = no;
				return true; // Collision detected
			}
			no++;

		}
	
	return false; // No collision
}

bool checkCollisionLevel1(const Player& player, float ex, float ez)
{
	if (level == 1)
	{
		// Define the bounding box of the player
		float playerMinX = player.posx - 1.0f; // Adjust these values based on the size of the player model
		float playerMaxX = player.posx + 1.0f;
		float playerMinZ = player.posz - 1.0f;
		float playerMaxZ = player.posz + 1.0f;

		// Check if the energy is within the bounding box of the player
		return (ex >= playerMinX && ex <= playerMaxX &&
			ez >= playerMinZ && ez <= playerMaxZ);
	}
	else
	{
		return false;
	}
}
bool checkCollisionLevel2(const Player& player, float ex, float ez)
{
	if (level == 2)
	{
		// Define the bounding box of the player
		float playerMinX = player.posx - 1.0f; // Adjust these values based on the size of the player model
		float playerMaxX = player.posx + 1.0f;
		float playerMinZ = player.posz - 1.0f;
		float playerMaxZ = player.posz + 1.0f;

		// Check if the energy is within the bounding box of the player
		return (ex >= playerMinX && ex <= playerMaxX &&
			ez >= playerMinZ && ez <= playerMaxZ);
	}
	else
	{
		return false;
	}
}


void myTimer(int value) {
	if (!gameEnded)
	{
		if (wall1.posx > 10 && !wall1.decrement) {
			wall1.decrement = true;
		}
		else if (wall1.posx < -10 && wall1.decrement)
		{
			wall1.decrement = false;
		}

		if (wall2.posx > 10 && !wall2.decrement) {
			wall2.decrement = true;
		}
		else if (wall2.posx < -10 && wall2.decrement)
		{
			wall2.decrement = false;
		}
		if (wall3.posx > 10 && !wall3.decrement) {
			wall3.decrement = true;
		}
		else if (wall3.posx < -10 && wall3.decrement)
		{
			wall3.decrement = false;
		}

		if (checkCollisionLevel1())
		{
			makeThePlayerReturn();
			PlaySound("Audio/young-man-being-hurt-95628.wav", NULL, SND_ASYNC | SND_FILENAME);
		}
		wall1.move();
		wall2.move();
		wall3.move();

		glutPostRedisplay();

		glutTimerFunc(1000 / 60, myTimer, 0);
	}
	
}

int cameraZoom = 0;
boolean lightflag = false;


// Textures

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void switchlights(int value) {
	//lightflag = !lightflag;
	if (lightflag) {
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		lightflag = false;
	}
	else {
		glEnable(GL_LIGHT2);
		glDisable(GL_LIGHT1);
		lightflag = true;
	}
	glutTimerFunc(2000, switchlights, 0);
}

void InitLightSource()
{
	//	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);
	//

	// 
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHT2);

	//
	//	COLOR                 R     G      B
	GLfloat diffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	//
	//	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { -15.0f, 8.0f, 5.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.007);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0007);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.00007);



	GLfloat diffuse1[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse1);
	//
	//	// Finally, define light source 0 position in World Space
	GLfloat light_position1[] = { -15.0f, 8.0f, 5.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, light_position1);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.007);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0007);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.00007);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void renderWall()
{
	glDisable(GL_LIGHTING); // Disable lighting 

	glColor3f(0.6, 0.6, 0.6); // Dim the texture color a bit

	glEnable(GL_TEXTURE_2D); // Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_wall.texture[0]); // Bind the desired texture

	glPushMatrix();
	glTranslated(0, 2.2, 12);
	glScaled(20, 6, 2);

	glBegin(GL_QUADS); // Start drawing the cube

	// Front Face
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);

	// Back Face
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
	glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, -0.5);

	// Top Face
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);

	// Bottom Face
	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, -0.5, -0.5);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);

	// Right face
	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -0.5, 0.5);

	// Left Face
	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -0.5, -0.5);
	glTexCoord2f(1.0, 0.0); glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f(1.0, 1.0); glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.5, -0.5);

	glEnd(); // End of drawing the cube
	glPopMatrix();

	glEnable(GL_LIGHTING); // Enable lighting again for other entities coming through the pipeline.

	glColor3f(1, 1, 1); // Reset material color to white
}

void renderBitmapString(float x, float y, void* font, const char* string) {
	const char* c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void print(int x, int y, string s)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	void* font = GLUT_BITMAP_TIMES_ROMAN_24;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(font, c);
	}
	glMatrixMode(GL_PROJECTION); //swapped this with...
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); //...this
	glPopMatrix();
	//added this
	glEnable(GL_TEXTURE_2D);
}
void RenderGround2()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground2.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.
}
//=======================================================================
// Display Function
//=======================================================================

void myDisplay(void)
{
	if (level == 1)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		InitLightSource();

		if (player.lives <= 0) {
			
		
			glLoadIdentity();	//Clear Model_View Matrix
			string message = "";
			if (player.lives <= 0) {
				glColor3f(1, 0, 0);
				message = "Game Over you lost";
			}
			else {
				message = "YOU WON!";
				glColor3f(0, 0, 1);
			}
			print(WIDTH / 2 - 30, HEIGHT / 2, message);
			glutSwapBuffers();
			glutIdleFunc(0);
			return;
		}

		

		// Draw Ground
		RenderGround();
		wall1.draw();
		wall2.draw();
		wall3.draw();


		//Model Player Draw
		glPushMatrix();
		glTranslatef(player.posx, 0, player.posz);
		glRotatef(180 + player.angle, 0.0f, 1.0f, 0.0f);
		glScalef(0.025, 0.025, 0.025);
		model_player.Draw();
		glPopMatrix();

		// Model Powerup Draw
		model_powerup.rot.y += 0.4;
		glPushMatrix();
		glTranslatef(-5, 0,6);
		model_powerup.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 0.7, 5);
		glPushMatrix();
		glScalef(0.01, 0.01, 0.01);
		model_energy.Draw();
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();

		model_energy.rot.y += 0.4;
		model_energy2.rot.y += 0.4;
		model_energy3.rot.y += 0.4;
		model_portal.rot.y += 0.1;

		glPushMatrix();
		glTranslatef(10, 0.7, 10);
		glPushMatrix();
		glScalef(0.01, 0.01, 0.01);
		model_energy2.Draw();
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-8, 0.7, 15);
		glPushMatrix();
		glScalef(0.01, 0.01, 0.01);
		model_energy3.Draw();
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();

		glTranslatef(0, 3, -20);
		glRotated(180, 0, 1, 0);
		model_portal.Draw();
		glPopMatrix();





		//sky box
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();
		glPushMatrix();
		qobj = gluNewQuadric();
		glTranslated(-5, 10, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, texs);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 3, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Render the score
		glColor3f(1.0f, 1.0f, 1.0f); // Set text color (e.g., white)
		char scoreText[32];
		sprintf(scoreText, "Score: %d", player.energyCollected*100); // Replace with your scoring variable
		renderBitmapString(WIDTH - 150, HEIGHT - 30, GLUT_BITMAP_HELVETICA_18, scoreText);
		char lives[32];
		sprintf(lives, "Lives : %d", player.lives); // Replace with your scoring variable
		renderBitmapString(WIDTH - 150, HEIGHT - 60, GLUT_BITMAP_HELVETICA_18, lives);

		// Restore the original projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// Swap buffers at the end of the frame
		glutSwapBuffers();

		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (player.lives <= 0 || gameEnded ) {
			/*glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);*/
			glLoadIdentity();	//Clear Model_View Matrix
			string message = "";
			if (player.lives <= 0) {
				glColor3f(1, 0, 0);
				message = "Game Over you lost";
			}
			else {
				message = "YOU WON!";
				glColor3f(0, 0, 1);
			}
			print(WIDTH / 2 - 30, HEIGHT / 2, message);
			glutSwapBuffers();
			glutIdleFunc(0);
			return;
		}


		/*GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);*/

		// Set up the view and projection matrices if needed

		// Draw HUD
		glColor3f(255, 255, 0);
		print(WIDTH / 2 - 40, HEIGHT - 60, "Score: " + to_string(player.energyCollected *100));
		glColor3f(255, 0, 0);
		print(20, HEIGHT - 70, "Lives: " + to_string(player.lives));
		glColor3f(0, 255, 0);
		print(WIDTH / 2 - 40, HEIGHT - 30, "LEVEL ");
		glColor3f(0.6, 0.6, 0.6);

		// Draw Ground
		RenderGround2();
		wall1.draw();
		wall2.draw();
		wall3.draw();


		// Model Player Draw
		glPushMatrix();
		glTranslatef(player.posx, 0, player.posz);
		glScalef(0.025, 0.025, 0.025);
		glRotatef(180 + player.angle, 0.0f, 1.0f, 0.0f);
		model_player.Draw();
		glPopMatrix();

		// Model Coin Draw
		glPushMatrix();
		glTranslatef(0, 0.7, 5);
		model_coin.rot.x = 90;
		model_coin.scale = 0.1;
		model_coin.pos.y = 1;
		model_coin.Draw();
		glPopMatrix();

		// Model Coin2 Draw
		glPushMatrix();
		glTranslatef(10, 0.7, 10);
		model_coin2.rot.z += 0.4;
		model_coin2.scale = 0.1;
		model_coin2.pos.y = 1;
		model_coin2.Draw();
		glPopMatrix();

		// Model Coin3 Draw
		glPushMatrix();
		glTranslatef(-8, 0.7, 15);
		model_coin3.rot.z += 0.4;
		model_coin3.scale = 0.1;
		model_coin3.pos.y = 1;
		model_coin3.Draw();
		glPopMatrix();

		// Model Portal Draw
		glPushMatrix();
		glTranslatef(0, 3, -20);
		glRotated(180, 0, 1, 0);
		model_portal.rot.y += 0.1;
		model_portal.Draw();
		glPopMatrix();

		
		// Model Key Draw
		glPushMatrix();
		glTranslatef(-8, 1, -9);
		model_key.rot.y += 0.1;
		model_key.Draw();
		glPopMatrix();

		// Draw Skybox
		glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();

		glPushMatrix();
		qobj = gluNewQuadric();
		glTranslated(-5, 10, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, texs);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 3, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();

		// Swap Buffers
		glutSwapBuffers();

	}
	
}
void checkallcollisionsoftheGame()
{
	if (checkCollisionLevel1(player, 0, -18))
	{
		level++;
		player.posx = 0;
		player.posz = 20;
		updateCamera();
	}
	if (checkCollisionLevel1(player, -8, 15)) {
		if (model_energy3.visible) {
			player.energyCollected ++;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);
		}
		model_energy3.visible = false;
	}

	if (checkCollisionLevel1(player, 0, 5)) {

		if (model_energy.visible)
		{
			player.energyCollected += 1;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);

		}
		model_energy.visible = false;
	}
	if (checkCollisionLevel1(player, 10, 10)) {
		if (model_energy2.visible)
		{
			player.energyCollected += 1;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);

		}
		model_energy2.visible = false;
	}
	if (checkCollisionLevel1(player,-5, 6)) {
		if (model_powerup.visible)
		{
			player.lives += 1;
			
		}
		model_powerup.visible = false;
	}
	if (checkCollisionLevel2(player, 10, 10)) {
		if (model_coin2.visible)
		{
			player.energyCollected += 1;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);

		}
		model_coin2.visible = false;
	}
	if (checkCollisionLevel2(player, 0, 5)) {

		if (model_coin.visible)
		{
			player.energyCollected += 1;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);

		}
		model_coin.visible = false;
	}
	if (checkCollisionLevel2(player, -8, 15)) {
		if (model_coin3.visible) {
			player.energyCollected += 1;
			PlaySound("Audio/coin-drop-4.wav", NULL, SND_ASYNC | SND_FILENAME);
			
		}
		model_coin3.visible = false;
	}
	if (checkCollisionLevel2(player, -8, -9)) {
		if (model_key.visible) {
			player.keycollected = true;
		}
		model_key.visible = false;
	}
	if ((checkCollisionLevel2(player, 0, -18) || checkCollisionLevel2(player, 1, -18) || checkCollisionLevel2(player, -1, -18)) && player.keycollected)
	{
		gameEnded = true;
		printf("asd");
		
	}

}
//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	if (!gameEnded)
	{
		switch (button)
		{
		case 'w':
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 'r':
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case '6':
			backward = false;
			checkallcollisionsoftheGame();
			player.moveRight();
			break;
		case '4':
			backward = false;
			checkallcollisionsoftheGame();
			player.moveLeft();
			break;
		case '5':
			checkallcollisionsoftheGame();
			backward = true;
			player.moveBackward();
			break;
		case '8':
			backward = false;
			checkallcollisionsoftheGame();
			player.moveForward();
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
		}

		glutPostRedisplay();
	}
	
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_RIGHT_BUTTON) {
			isFirst = !isFirst;
			updateCamera();
		}
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_player.Load("Models/ManThiago/Man N060215.3ds");
	model_portal.Load("Models/portal/DragonJawPortal.3ds");
	model_key.Load("Models/Key/Key9.3ds");
	model_coin.Load("Models/coin/coin.3ds");
	model_coin2.Load("Models/coin/coin.3ds");
	model_coin3.Load("Models/coin/coin.3ds");
	model_energy.Load("Models/energy/energy.3ds");
	model_energy2.Load("Models/energy/energy.3ds");
	model_energy3.Load("Models/energy/energy.3ds");
	model_powerup.Load("Models/powerup/uploads_files_742911_MonolithCrystal.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground2.bmp");
	tex_ground2.Load("Textures/ground.bmp");

	tex_wall.Load("Textures/thunder1.bmp");

	tex_wall2.Load("Textures/movingwall.bmp");

	loadBMP(&tex, "Textures/wall3.bmp", true);
	loadBMP(&texs, "Textures/lava.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	
	glutDisplayFunc(myDisplay);
	

	glutKeyboardFunc(myKeyboard);

	//glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	//myInit();

	LoadAssets();

	player.playerModel = model_player;

	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);*/
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	glutTimerFunc(1000 / 60, myTimer, 0); // 60 FPS
	glutTimerFunc(0, switchlights, 0);

	glutMainLoop();
}