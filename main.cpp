#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <iostream>

#include <unistd.h>

#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <ctime>

#include <array>
#include <vector>
#include <string>

#include "draw_text.h"	//uses header file from labs to draw instructions on the screen
#include "lights_material.h"
#include "draw_shape.h"
#include "load_and_bind_texture.h"

#define PI 3.1415926538
// #define print(a) std::cout << #a << ": " << (a) << std::endl

//used to control the rotation of the court
float rotateOffset = 0.0f;
float normalised_mouseX, normalised_mouseY;	//stores normalised window coordinates for the mouse
float mouseX, mouseY;		//stores world coordinates for the x and y positions of the mouse

float cameraX, cameraY, cameraZ;			//coords of front camera
float topCameraX, topCameraY, topCameraZ;	//coords of back camera

//Stores the various textures used
unsigned int menuTextures[3];
unsigned int blockTextures[4];
unsigned int ballTextures[3];


unsigned int worldWidth = 1000;
unsigned int windowWidth = 800;
unsigned int mapWidth = 400;
unsigned int windowHeight = 800;



std::array<float,2> courtXboundary;	//stores the 2 x boundarys in an array [left,right]
std::array<float,2> courtYboundary;	//stores the 2 y boundarys in an array
std::array<float,2> courtZboundary; //stroes the 2 z boundarys in an array

bool gameStarted;	//set to true when the game has begun
bool endGame;		//set to true if game is lost
bool wonGame;		//set to true if game is won
bool showInstructions, showMenu, showGameover, showCongratulations; //texture images that are used to display messages
bool useMouse;		//press this to switch to the mouse keys
bool drawingPrintMap = false;

//structs to store difficulty and directions
enum Difficulty {EASY, MEDIUM, HARD};
enum Direction {STILL, LEFT, RIGHT, UP, DOWN, FORWARD, BACKWARD};

//Class to store functions and data associated with the 3d playing court
class Court{
	//private
	const float width, height, length;
public:
	bool rotating;
	Court() : width(800.0f), height(600.0f), length(1200.0f), rotating(false){

	}

	//draws the court for standard viewing from the front face
	void draw(){
		set_material(mattblack);
		drawCourt(true,width,height,length);

	}

	//draws the court for viewing from the top
	void drawFromTop(){
		set_material(mattblack);
		drawCourt(false,width,height,length);
	}

	std::array<float,2> getXboundary(){
		std::array<float,2> boundaries = {width/2.0f , -width/2.0f};
		return boundaries;
	}

	std::array<float,2> getYboundary(){
		std::array<float,2> boundaries = {height/2.0f , -height/2.0f};
		return boundaries;
	}

	std::array<float,2> getZboundary(){
		std::array<float,2> boundaries = {length/2.0f , -length/2.0f};
		return boundaries;
	}

	//sets the court to rotate mode
	void startRotating(){ rotating = true;}

	//stops the code form rotating
	void stopRotating(){ rotating = false;}

	void updateRotate(float x){ rotateOffset += x;}

	float getWidth(){return width;}
	float getHeight(){return height;}
	float getLength(){return length;}

};

// Class to store data on the main paddle such as colour, width etc
class Paddle{
	//private
	float width, height, length;
	float sideGap;		//used to define how close the paddle will get to the edges of the court
	unsigned int basicPaddle;		//will be used to execute a pre-compiled display list of a basic paddle

public:
	float xOffset, yOffset, zOffset;
	float xStep, yStep;	//controls the speed of the paddle
	float yAdd;	// stores how fa
	Direction currXdirection;
	Direction currYdirection;
	Difficulty currDifficulty;	//stores the current difficulty level
	bool moving;	//used to check whether paddle is currently moving


	//first define constructor
	Paddle(){
		xStep = yStep = 10.0f;
		moving = false;
	}

	//initialises the starting coord for paddle based on the court size.
	void init(){
		xOffset = 0;
		yOffset = courtYboundary[1] + height + 100.0f;
	}

	//sets a different level of difficulty
	void setDifficulty(Difficulty level){
		switch(level){
			case EASY:
				width = 200;
				height = 200.0f;
				length = 200.0f;
				make();
				currDifficulty = EASY;
				zOffset = courtZboundary[0] - length;
				break;
			case MEDIUM:
				width = 180.0f;
				height = 180.0f;
				length = 180.0f;
				zOffset = courtZboundary[0] - length;
				make();
				currDifficulty = MEDIUM;
				break;
			case HARD:
				width = 150.0f;
				height = 150.0f;
				length = 150.0f;
				zOffset = courtZboundary[0] - length;
				make();
				currDifficulty = HARD;
				break;
		}
	}

	//draws the paddle
	void draw(){
		glCallList(basicPaddle);
	}

	//Resets the paddles values when user restarts game(presses s)
	void restart(Court c){
		init();
		moving = false;
		setDifficulty(EASY);
	}

	//generates a handle which is used to compile paddle geometry
	void make(){
		unsigned int handle = glGenLists(1);
		glNewList(handle, GL_COMPILE);
			glDisable(GL_LIGHTING);
			glColor4f(0.8f, 0.1f, 0.1f, 0.2f) ;
			drawCuboid(width,height,length);
			// glDisable(GL_LIGHTING);
			glLineWidth(2.0f);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawCuboidBorder(width,height,length);
			glEnable(GL_LIGHTING);
		glEndList();
		basicPaddle = handle;
	}

	//when called moves the paddle by changing its coordinates based on the direction keys
	void move(){
		float xEdge[2] = {xOffset + (width/2.0f),  xOffset - (width/2.0f)};
		float yEdge[2] = {yOffset + (height/2.0f), yOffset - (height/2.0f)};

		//Checks if the paddle will hit the court edges on its next step. If so the x direction is reversed
		if (currXdirection != STILL){
			if (currXdirection == LEFT){
				if ((xEdge[1] - xStep) > courtXboundary[1]) xOffset -= xStep;
			}else if (currXdirection == RIGHT){
				if ((xEdge[0] + xStep) < courtXboundary[0]) xOffset += xStep;
			}
		}
		if (currYdirection != STILL){
			if (currYdirection == UP){
				if ((yEdge[0] + yStep) < courtYboundary[0]) yOffset += yStep;
			}else if (currYdirection == DOWN){
				if ((yEdge[1] - yStep) > courtYboundary[1]) yOffset -= yStep;
			}
		}
	}

	//works like the move method but used the mouse coordinates instead
	void moveMouse(){
		float mult = cameraZ / (cameraZ - zOffset -length);
		sideGap = 2.0f;	//used so that the paddle doesnt touch the side of the court

		//the 1.2 and 1.3 constants were found out after testing and are used to get a better feeling when using the mouse
		xOffset = courtXboundary[0] * normalised_mouseX * mult * 1.5f;
		yOffset = courtYboundary[0] * normalised_mouseY * mult * 1.5f;

		if ( (xOffset + (width/2.0f)) >= courtXboundary[0]) xOffset = courtXboundary[0] - width/2.0f - sideGap;
		else if ( (xOffset - (width/2.0f)) <= courtXboundary[1]) xOffset = courtXboundary[1] + width/2.0f + sideGap;

		if ( (yOffset + (height/2.0f)) >= courtYboundary[0]) yOffset = courtYboundary[0] - height/2.0f - sideGap;
		else if (yOffset - (height/2.0f) <= courtYboundary[1]) yOffset = courtYboundary[1] + height/2.0f + sideGap;

		zOffset = zOffset;
	}


	//returns the coord for the left and right faces of the paddle
	std::array<float,2> getXcoords(){
		std::array<float,2> coord;
		coord[0] = xOffset - (width/2.0f);
		coord[1] = xOffset + (width/2.0f);
		return coord;
	}

	//returns the coord for the top and bottom of the paddle
	std::array<float,2> getYcoords(){
		std::array<float,2> coord;
		coord[0] = yOffset - (height/2.0f);
		coord[1] = yOffset + (height/2.0f);
		return coord;
	}

	//returns the coord for the front face and backface of the paddle
	std::array<float,2> getZcoords(){
		std::array<float,2> coord;
		coord[0] = zOffset - (length/2.0f);
		coord[1] = zOffset + (length/2.0f);
		return coord;
	}

	//retrieve methods
	float getWidth(){return width;}
	float getHeight(){return height;}
	float getLength(){return length;}

};


//class to store all functions and variables associated with the ball
class Ball{
	float xAngle, yAngle;	//stores angle in radians to x and y axis respectiveley
	float xStep, yStep, zStep;	//controls the speed in the x direction
	float zMult;
	Direction yDirection, xDirection, zDirection;	//stores the current direction the ball is travelling in xyz terms
	unsigned int basicBall;		//will be used to execute a pre-compiled display list of a basic ball

public:
	float xOffset, yOffset, zOffset;	//coord of the ball
	float speed;	//controls how many pixels should be moved each time the ball is drawn

	bool  moving;		//true if the ball is moving(in gameplay)
	float radius;

	//constructor for the Ball class
	Ball (){
		radius = 30.0f;
		zDirection = BACKWARD;	//away from the paddle
		zMult = 2.0f;
	}

	//init method sets ball to the fornt fave of the paddle
	void init(Paddle p){
		xOffset =  yOffset = zOffset = 0.0f;
		xOffset += p.xOffset;
		yOffset += p.yOffset;
		zOffset += p.getZcoords()[0] - radius - 5.0f ;
	}

	//Method changes the speed of the ball when user inputs a change in difficulty
	void setDifficulty(Difficulty level){
		xStep = xStep / speed;
		yStep = yStep / speed;
		zStep = zStep /speed;

		switch (level){
			case EASY:
				speed = 10.0f;
				goto endcase;
			case MEDIUM:
				speed = 15.0f;
				goto endcase;
			case HARD:
				speed = 20.0f;
				goto endcase;
		}
		endcase:
		xStep = speed * xStep;
		yStep = speed * yStep;
		zStep = speed * zStep;

	}

	//Method used to get a handle for a new display list. THis is used to compile the ball geometry
	void make(){
		unsigned int handle = glGenLists(1);
		glNewList(handle, GL_COMPILE);
			GLUquadric *quad;
			quad = gluNewQuadric();
			gluSphere(quad,radius,40,40);
		glEndList();
		basicBall = handle;		//saves handle
	}

	//Draws the ball onto the screen
	void draw(){
		set_material(white_shiny);
		glBindTexture(GL_TEXTURE_2D, ballTextures[0]);

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);


		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glCallList(basicBall);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

	}

	//draws the balls shadow
	void drawShadow(){
		drawCircle(radius);
	}

	//restart function, called when the game is restarted
	void restart(Paddle p){
		init(p);
		setDifficulty(EASY);
		randomAngle();
		moving = false;
	}

	//Produces random x and y speeds to make the ball look like its going off at random angles
	void randomAngle(){
		srand48(time(NULL));	//used to ensure true randomness
		// for (int x =0; x<((rand() *1000)/RAND_MAX);x++)
			xAngle = float(drand48()) * (PI/6.0f)  + (PI/4.0f);
			yAngle = float(drand48()) * (PI/6.0f) + (PI/4.0f);
		//Randomising the angle x and y directions
		float mult[2] = {-1.0, 1.0};
		int index;

		index = lrand48() % 2;
		xStep = mult[index] * speed * tan(xAngle);
		index = lrand48() % 2;
		yStep =  mult[index]* speed * tan(yAngle);
		zStep = -speed * zMult;

		if(xStep > 0.0f) xDirection = RIGHT;
		else xDirection = LEFT;

		if(yStep > 0.0) yDirection = UP;
		else yDirection = DOWN;

		zDirection = BACKWARD;
	}
	//

	//Called everytime the ball should move
	void move(Paddle &p){
		// float paddleTopCoord = p.yOffset + (p.getHeight() / 2.0f);
		float xEdge[2] = {xOffset - radius, xOffset + radius};
		float yEdge[2] = {yOffset - radius, yOffset + radius};
		float zEdge[2] = {zOffset - radius, zOffset + radius};

		//checking collisions on the left and right walls
		if (xDirection == LEFT && (xEdge[0] + xStep) <= courtXboundary[1]){
			xStep = -xStep;
			xDirection = RIGHT;
			// std::cout << "x direction now right\n";
		}else if(xDirection == RIGHT && (xEdge[1] + xStep) >= courtXboundary[0]){
			xStep = -xStep;
			xDirection = LEFT;
			// std::cout << "x direction now keft\n";
		}

		//checking collisions on the top and bottom walls
		if (yDirection == UP && (yEdge[1] + yStep) >= courtYboundary[0]){
			yStep = -yStep;
			yDirection = DOWN;
		}else if(yDirection == DOWN && yEdge[0] + yStep <= courtYboundary[1]){
			yStep = -yStep;
			yDirection = UP;
		}

		//gets z coord of paddle to see if the ball has gone past it
		std::array<float,2> paddleZcoord = p.getZcoords();

		if(zDirection == BACKWARD && (zEdge[0] + zStep) <= courtZboundary[1]){
			zStep = -zStep;
			zDirection = FORWARD;
		}else if (zDirection == FORWARD && (zEdge[1]) > paddleZcoord[1] && (zEdge[0] > paddleZcoord[0])){
			//getting x and y coords of paddle to check for collision
			std::array<float,2> paddleXcoord = p.getXcoords();
			std::array<float,2> paddleYcoord = p.getYcoords();

			bool condition1 = xEdge[0] + xStep >= paddleXcoord[0] && xEdge[1] + xStep <= paddleXcoord[1];
			bool condition2 = yEdge[0] + yStep >= paddleYcoord[0] && yEdge[1] + yStep <= paddleYcoord[1];
			if(condition1 && condition2){
				yStep = -yStep;
				if(yStep>0.0f) yDirection = UP;
				else yDirection = DOWN;

				zStep = -zStep;
				zDirection = BACKWARD;

			}
		}
		if (zEdge[1] > (courtZboundary[0] + 200.0f)){
			moving = false;
			endGame = true;
		}

		xOffset += xStep;
		yOffset += yStep;
		zOffset += zStep;
	}

	void reverseYdirection(){
		yStep = - yStep;
		if (yDirection == DOWN) yDirection = UP;
		else yDirection = DOWN;
	}

	void reverseXdirection(){
		xStep = -xStep;
		if (xDirection == RIGHT) xDirection = LEFT;
		else xDirection = RIGHT;
	}

	void reverseZdirection(){
		zStep = -zStep;
		if(zDirection == FORWARD) zDirection = BACKWARD;
		else zDirection = FORWARD;
	}

	Direction getXdirection(){ return xDirection;}
	Direction getYdirection(){ return yDirection;}
	Direction getZdirection(){ return zDirection;}


};

/* ----------------------------BLOCK Class-------------------------
* Class used to store the data for individual blocks, and operations
* on the block
*/
class Block{
	GLfloat color[3];	//stores the rgb color of a block
	float width;
	int textureNo;	//this is set to a random number in Grid to create random block sizes

public:
	float xCoord, yCoord, zCoord;	//stores coordinates of the blocks
	float blockHandle;				//stores the handle for the compiled block geometry in the display list
	bool hit;	//set to true when the block has been hit

	Block() {
		hit=false;
	}
	void setXYZ(float x, float y, float z){
		xCoord = x;
		yCoord = y;
		zCoord = z;
	}

	void setWidth(float w){ width = w;}
	float getWidth(){return width;}

	void setHandle(unsigned int h){
		blockHandle = h;
	}

	//sets the texture number that will be used for the block to the random one that is assigned for it in grid
	void setTexture(int tNumber){
		textureNo = tNumber;
	}


	void setColor(GLfloat col[3]){
		for (int i = 0; i < 3; i++) {
			color[i] = col[i];
		}
	}

	//uses display lists to draw blocks efficiently.
	void draw(){
		// std::cout << "Block coord" << xCoord <<" "<<yCoord<<" "<< zCoord<<'\n';
		// std::cout << "colors = "<<color[0]<<" "<<color[1]<<" "<<color[2] << '\n';
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			set_material(marble);
			glTranslatef(xCoord, yCoord, zCoord);

			glBindTexture(GL_TEXTURE_2D, blockTextures[textureNo]);
			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glTranslatef(0.0,-width/2.0f,0.0f);
			glCallList(blockHandle);	//bottom side

			glTranslatef(0.0,width,0.0f);
			glCallList(blockHandle);	//top side

			glTranslatef(0.0,-width/2.0f,width/2.0f);
			glPushMatrix();
				glRotatef(90.0,1.0,0.0,0.0);
				glCallList(blockHandle);	// front side
				glRotatef(-90.0,1.0,0.0,0.0);
				glTranslatef(0.0,0.0,-width);
				glPushMatrix();
					glRotatef(-90.0,1.0,0.0,0.0);
					glCallList(blockHandle);		//back side
					glRotatef(90.0,1.0,0.0,0.0);
					glTranslatef(-width/2.0f,0.0f,width/2.0f);
					glPushMatrix();
						glRotatef(90.0,0.0,0.0,1.0);
						glCallList(blockHandle);	//left side
						glRotatef(-90.0,0.0,0.0,1.0);
						glTranslatef(width,0.0,0.0);
						glPushMatrix();
							glRotatef(-90.0,0.0,0.0,1.0);
							glCallList(blockHandle);	//rightside
							glRotatef(90.0,0.0,0.0,1.0);
						glPopMatrix();
					glPopMatrix();
				glPopMatrix();

			glPopMatrix();

			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	void drawShadow(){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			set_material(white);
			glTranslatef(xCoord, courtYboundary[1]+2.0f , zCoord);
			drawRectangle(false,true,width,width);
		glPopMatrix();
	}

	unsigned int make(){
		unsigned int handle = glGenLists(1);
		glNewList(handle, GL_COMPILE);
			drawRectangle(true,true,width,width);
		glEndList();
		return handle;
	}

	//returns the coord for the left and right faces of the block
	std::array<float,2> getXcoords(){
		std::array<float,2> coord;
		coord[0] = xCoord - width;
		coord[1] = xCoord + width;
		return coord;
	}

	//returns the coord for the top and bottom of the block
	std::array<float,2> getYcoords(){
		std::array<float,2> coord;
		coord[0] = yCoord - width;
		coord[1] = yCoord + width;
		return coord;
	}

	//returns the coord for the front face and backface of the block
	std::array<float,2> getZcoords(){
		std::array<float,2> coord;
		coord[0] = zCoord - width;
		coord[1] = zCoord + width;
		return coord;
	}


};

//Class storing all the data and functions for blocks. Also checks collisions with ball
class Grid{
	unsigned int numBlocksHit;
	unsigned int score;
	unsigned int highScore;
	unsigned int numBlocks;

	float blockWidth;
	std::vector<Block> grid;
public:
	Grid(){
		numBlocks = 15;
		numBlocksHit = 0;
		highScore = 0;
		blockWidth = 80.0f;
	}

	//used to initialise all the blocks in the grid
	void init(){
		float randomX, randomY, randomZ;
		float xCoord, yCoord, zCoord;
		GLfloat colors[3];
		unsigned int blockHandle;
		int randomTex;
		for (unsigned int i = 0; i < numBlocks; i++){
			Block b;
			for (int i = 0; i < 3; i++){
				colors[i] = randomColor();
			}
			randomTex = ((int)lrand48()) % (sizeof(blockTextures)/sizeof(*blockTextures));	//generates a random int in the range of the number of block textures
			b.setTexture(randomTex);
			b.setWidth(blockWidth);


			//generating random positions for the blocks
			randomX = float(drand48()) * (courtXboundary[0] - courtXboundary[1] - (blockWidth));
			randomY = float(drand48()) * (courtYboundary[0] - courtYboundary[1] - (blockWidth));
			randomZ = float(drand48()) * (0 - courtXboundary[1]);							//dont want blocks to start to close to camera

			xCoord = courtXboundary[1] + randomX + (blockWidth/2.0f);
			yCoord = courtYboundary[1] + randomY + (blockWidth/2.0f);
			zCoord = (courtZboundary[0]/2.0f) - (randomZ + blockWidth/2.0f);
			if (touchingBlock(xCoord,yCoord,zCoord)) std::cout << "Blocks touching" << '\n';
			else b.setXYZ(xCoord,yCoord,zCoord);

			blockHandle = b.make();
			b.setHandle(blockHandle);
			grid.push_back(b);
		}

	}

	bool touchingBlock(float x, float y, float z){
		bool touching = false;

		for(Block b: grid){
			if(abs(x - b.xCoord) < blockWidth){
				if(abs(y - b.yCoord) < blockWidth){
					if(abs(z - b.zCoord) < blockWidth){
						return true;
					}
				}
			}
		}
		return false;
	}

	void restart(){
		score = 0;
		numBlocksHit = 0;
		for(Block& b: grid){
			b.hit = false;
		}
	}


	//produces random number between 0 and 1
	GLfloat randomColor(){
		GLfloat color;
		color = GLfloat(drand48());
		return color;
	}


	unsigned int getScore(){
		score = numBlocksHit * 10;
		if(numBlocksHit == numBlocks) wonGame = true;
		return score;
	}

	unsigned int getHighScore(){
		unsigned int score = getScore();
		if (score > highScore) highScore = score;
		else return highScore;
	}

	//draws every block
	void draw(){
		for (Block& b : grid){
			if (!b.hit) {
				b.draw();
				b.drawShadow();
			}
		}
	}

	void updateGrid(Ball &ball){

		std::array<float,2> blockXcoords;
		std::array<float,2> blockYcoords;
		std::array<float,2> blockZcoords;

		float ballBottomEdge = ball.yOffset - ball.radius;
		float ballTopEdge = ball.yOffset + ball.radius;
		float ballLeftEdge = ball.xOffset - ball.radius;
		float ballRightEdge = ball.xOffset + ball.radius;
		float ballFrontEdge = ball.zOffset + ball.radius;
		float ballBackEdge = ball.zOffset - ball.radius;


		for (Block& b : grid){
			if (!b.hit){
				blockXcoords = b.getXcoords();
				blockYcoords = b.getYcoords();
				blockZcoords = b.getZcoords();

				if(ballBackEdge >= blockZcoords[0] && ballBackEdge <= blockZcoords[1] ){
					if(ballLeftEdge >= blockXcoords[0] && ballRightEdge <= blockXcoords[1]){
						//handles if the ball hits the top/bottom of a block
						if(ballTopEdge >= blockYcoords[0] && ballTopEdge < blockYcoords[1]){
							if (ball.getYdirection() == UP){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseYdirection();
								goto endloop;
							}
						}
						else if(ballBottomEdge <= blockYcoords[1] && ballBottomEdge > blockYcoords[0]){
							if (ball.getYdirection() == DOWN){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseYdirection();
								goto endloop;
							}
						}
					}

					//handles if the ball hits the side of a block
					if (ballTopEdge > blockYcoords[0] && ballBottomEdge < blockYcoords[1]){

						if(ballLeftEdge  <= blockXcoords[1] && ballLeftEdge > blockXcoords[0]){
							if (ball.getXdirection() == LEFT){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseXdirection();
								goto endloop;
							}
						}
						if(ballRightEdge >= blockXcoords[0] && ballRightEdge < blockXcoords[1]){
							if (ball.getXdirection() == RIGHT){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseXdirection();
								goto endloop;
							}
						}
					}
				}

				//handles if ball hits front of back of a block
				if(ballLeftEdge >= blockXcoords[0] && ballRightEdge <= blockXcoords[1]){
					if (ballTopEdge >= blockYcoords[0] && ballBottomEdge <= blockYcoords[1]){
						if(ballFrontEdge >= blockZcoords[0] && ballFrontEdge < blockZcoords[1]){
							if(ball.getZdirection() == FORWARD){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseZdirection();
								goto endloop;
							}
						}
						else if (ballBackEdge <= blockZcoords[1] && ballBackEdge > blockZcoords[1]){
							if(ball.getZdirection() == BACKWARD){
								b.hit= true;
								numBlocksHit +=1;
								ball.reverseZdirection();
								goto endloop;
							}
						}

					}
				}


			}
		}

	endloop:;

	}

};

//-----------Initialising Objects----------------//

Court court;
Paddle paddle;
Ball ball;
Grid grid;
//----------------------------------------------///


void moveObjects(){
	//sets ball to the paddle position
	if (!gameStarted) ball.init(paddle);

	if (paddle.moving) {
		if (useMouse) paddle.moveMouse();
		else paddle.move();
	}
	//checks if the ball is supposed to be moving currently. If it is the move method is called on it to change its current coordinates
	if (ball.moving) ball.move(paddle);

	//checks if the ball has hit any of the blocks
	grid.updateGrid(ball);
}


void drawObjects()
{

	//sets ball to the paddle position
	if (!gameStarted) ball.init(paddle);

	if (!(endGame) || showInstructions || showGameover){

		switchoff_light(topLight);
		switchoff_light(redSpot);
		switchoff_light(redSpot2);
		switchoff_light(redSpot3);
		switchoff_light(yellowSpot);
		switchoff_light(yellowSpot2);
		switchoff_light(yellowSpot3);
		//Puts the start menu on the screen
		if(showMenu || showInstructions || showGameover || showCongratulations){
			glPushMatrix();
				glTranslatef(0.0f,0.0f,courtZboundary[0] + 18.0f);
				glRotatef(-90.0f,1.0f,0.0f,0.0f);
				set_material(mattblack);
				drawRectangle(true,true,1000.0f,1000.0f);
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f,0.0f,courtZboundary[0] + 20.0f);
				glRotatef(-90.0f,1.0f,0.0f,0.0f);
				glEnable(GL_TEXTURE_2D);
					if (showMenu) glBindTexture(GL_TEXTURE_2D, menuTextures[0]);
					if (showGameover) glBindTexture(GL_TEXTURE_2D, menuTextures[2]);
					if (showInstructions) glBindTexture(GL_TEXTURE_2D, menuTextures[1]);

				set_material(menu);
				drawRectangle(true,true,600.0f,600.0f);

				glDisable(GL_TEXTURE_2D);

			glPopMatrix();

		}

		//checks to make sure all of the menus are off the screen before starting to draw
		if (!(showMenu || showInstructions || showCongratulations)){
			//drawing the 3d court




			//resetting the lights
			turnon_light(topLight);
			turnon_light(redSpot); turnon_light(redSpot2); turnon_light(redSpot3);
			turnon_light(yellowSpot); turnon_light(yellowSpot2); turnon_light(yellowSpot3);




			glRotatef(rotateOffset, 0, 0, 1);
			glPushMatrix();

				//draws the court to the screen

				if (drawingPrintMap) court.drawFromTop();
				else court.draw();

				//drawing grid using method in draw_shape
				if (!drawingPrintMap) glDisable(GL_LIGHTING);
					drawGridLines(courtXboundary,courtYboundary,courtZboundary);
				if (!drawingPrintMap) glEnable(GL_LIGHTING);

				//draw the ball
				glPushMatrix();
					glTranslatef(ball.xOffset, ball.yOffset,ball.zOffset);

					// if (ball.moving) ball.move(paddle);
					if (endGame) goto endRoutine;			//makes sure that nothing is done after the person has lost
					ball.draw();
				glPopMatrix();


				// grid.updateGrid(ball);
				//drawing blocks
				glPushMatrix();
					grid.draw();
				glPopMatrix();



				///////----------------------------------------------------------------////
				//----------------------Transparent Geometry---------------------------////
				//--------------------------------------------------------------------////

				// make depth buffer read - only so that transparent geometry can be drawn
				glDepthMask(GL_TRUE) ;

				//drawing shadow of ball on the floor
				if(!drawingPrintMap){
					glPushMatrix();
						glTranslatef(ball.xOffset, courtYboundary[1] + 2.0f,ball.zOffset);
						glRotatef(90.0, 1.0,0.0,0.0);
						glColor4f(1.0f,1.0f,1.0f,0.0f);
						set_material(white_shiny);
						ball.drawShadow();
					glPopMatrix();
				}

				//draw the paddle
				glPushMatrix();
					glTranslatef(paddle.xOffset, paddle.yOffset, paddle.zOffset);
					paddle.draw();
				glPopMatrix();

			glPopMatrix();

			if (!drawingPrintMap){
				glDisable(GL_LIGHTING);	//need to disable lighting for writing text
				glPushMatrix();

					glLineWidth(1.0f);

					draw_text(20,worldWidth-40,"Score:");
					draw_text(worldWidth - 250, worldWidth-40,"HighScore:");
					draw_text(20,50,"Press y/Y to change speed of the paddle");
					draw_text(20,20,"Press m to toggle mouse mode");
					unsigned int gscore = grid.getScore();
					if (gscore >= 50 && gscore < 90){
						paddle.setDifficulty(MEDIUM);
						ball.setDifficulty(MEDIUM);
					}
					else if (gscore >= 90 && gscore <120){
						paddle.setDifficulty(HARD);
						ball.setDifficulty(HARD);
					}
					else if (gscore >= 120){
						if (!court.rotating) court.startRotating();
						else court.updateRotate(1.0f);
					}else if (gscore>= 160){
						court.updateRotate(2.0f);
					}

					std::string s = std::to_string(grid.getScore());
					std::string hs = std::to_string(grid.getHighScore());
					draw_text(120,worldWidth-40, s.c_str());
					draw_text(worldWidth-90,worldWidth-40, hs.c_str());

				glPopMatrix();
				glEnable(GL_LIGHTING);
			}
		}

		if(drawingPrintMap){
			glDisable(GL_LIGHTING);
			draw_text(20,worldWidth-20,"Press R to check out rotation:");
			glEnable(GL_LIGHTING);
		}
		if (court.rotating && !drawingPrintMap) court.updateRotate(1.0f);


	}
	else if (endGame){
endRoutine:
		court.stopRotating();
		showGameover = true;
	}else if (wonGame){
		court.stopRotating();
		showCongratulations = true;
	}


	glutSwapBuffers();	//uses double buffering to render to screen
}


//used to create both the front and top views
void display(){
	moveObjects();

	glViewport(0, 0, windowWidth, windowHeight);
	glScissor(0,0,windowWidth,windowHeight);

	// windowWidth = w;
	// windowHeight = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// take FOV, ASPECT RATIO, NEAR, FAR
	gluPerspective(40.0, 1.0f, 1.0, 3000.0);

	glDepthMask(GL_TRUE) ;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// position and orient camera
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt((int) cameraX, (int) cameraY, (int) cameraZ, // eye position
			  0, 0, 0, // reference point
			  0, 1, 0  // up vector
		);

	drawObjects();

	glutSwapBuffers();

	glViewport(windowWidth, 100, mapWidth, windowHeight-200);
	glScissor(windowWidth,100,mapWidth,windowHeight-200);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// take FOV, ASPECT RATIO, NEAR, FAR
	gluPerspective(40.0, 1.0f, 1.0, 6000.0);

	glDepthMask(GL_TRUE) ;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// position and orient camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt((int) 0.0, (int) 1500, (int) 100, // eye position
			  0, 0, 0, // reference point
			  0, 1, 0  // up vector
		);

	drawingPrintMap = true;

		drawObjects();

	drawingPrintMap = false;
	glutSwapBuffers();


}

//handles when any non special keys are pressed down
void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		//exits the game
		case 'q':
			std::cout << "Leaving game" << '\n';
			exit(1);
			break;


		case 'r': if (!court.rotating) court.startRotating();
					else court.stopRotating();break;
		case ' ':
			gameStarted = true;
			ball.moving = true;
			showMenu = false;
			break;
		case 'd':
			if (paddle.currDifficulty == MEDIUM){
				paddle.setDifficulty(HARD);
				ball.setDifficulty(HARD);break;
			}else if (paddle.currDifficulty == HARD)
			{
				paddle.setDifficulty(EASY);
				ball.setDifficulty(EASY);break;
			}else{
				paddle.setDifficulty(MEDIUM);
				ball.setDifficulty(MEDIUM);break;
			}
			break;

		//restart the game
		case 's':
			grid.restart();
			paddle.restart(court);
			ball.restart(paddle);
			rotateOffset = 0.0f;
			gameStarted = false;
			endGame = false;
			wonGame = false;
			showGameover = false;
			showCongratulations = false;
			break;
		case 'i':
			if (showInstructions) showInstructions = false;
			else showInstructions = true;
				break;
		case 'm':
			if (!useMouse) useMouse = true;
			else useMouse= false;
	}

	glutPostRedisplay();
}


//Keyhandler for when any special key pressed like arrow keys
void specialDown(int key, int, int)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			paddle.currXdirection = LEFT;
			paddle.moving = true;
			break;
		case GLUT_KEY_RIGHT:
			paddle.currXdirection = RIGHT;
			paddle.moving = true;
			break;
		case GLUT_KEY_UP:
			paddle.currYdirection = UP;
			paddle.moving = true;
			break;
		case GLUT_KEY_DOWN:
			paddle.currYdirection = DOWN;
			paddle.moving = true;
			break;

	}

	glutPostRedisplay(); // force a redraw
}

//Keyhandler for when arrow keys are released
void specialUp(int key, int, int)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			paddle.currXdirection = STILL;
			paddle.moving = false;
			break;
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			paddle.currYdirection = STILL;
			paddle.moving = false;
			break;

	}
}


//Handles mouse mouse_motion
void mouseMotion(int x, int y)
{
	//normalising coordinates
	normalised_mouseX = (float(x)/(float)windowWidth) - 0.5f;
	normalised_mouseY = (float(y)/(float)windowHeight) - 0.5f;
	normalised_mouseY *= -1;	//flips the y axis
	paddle.moving = true;
	// std::cerr << "\t mouse is at (" << normalised_mouseX << ", " << normalised_mouseY << ")" << std::endl;
	// glutPostRedisplay();
}


void reshape(int w, int h)
{


}

// idle handler
void idle()
{
	usleep(5000);
	if (paddle.moving || ball.moving || court.rotating)	//checks if either the paddle or the ball are in motion
		glutPostRedisplay();
}

void visibility(int vis)
{
	if (vis==GLUT_VISIBLE) glutIdleFunc(idle);
	else glutIdleFunc(NULL);
}

void init(int argc, char* argv[])
{

	// set_light(light_0);
	set_light(topLight);
	set_light(frontLight);
	set_light(redSpot); set_light(redSpot2); set_light(redSpot3);
	set_light(yellowSpot); set_light(yellowSpot2); set_light(yellowSpot3);
	// set_light(light_2);

	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_LIGHTING);		//enable lighting
	glEnable(GL_DEPTH_TEST);	// so that hidden surfaces are removed
	glShadeModel(GL_SMOOTH);		//mode of shading

	glEnable(GL_BLEND);			// turn on blending and set a blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cameraX = 0;
	cameraY = 0;
	cameraZ = 1700;

	topCameraX = 0;
	topCameraY = 0;
	topCameraZ = 0;

	//setting up global variables
	courtXboundary = court.getXboundary();
	courtYboundary = court.getYboundary();
	courtZboundary = court.getZboundary();
	court.stopRotating();

	//setup for the paddle
	paddle.init();
	paddle.setDifficulty(EASY);
	paddle.moving = false;
	// paddle.setYoffset(court);
	paddle.make();

	//setup for the ball
	ball.init(paddle);
	ball.setDifficulty(EASY);
	ball.moving = false;
	ball.randomAngle();
	ball.make();
	ball.draw();

	//setting up the main blocks
	grid.init();
	gameStarted = false;
	endGame = wonGame = false;
	showInstructions = showGameover = showCongratulations = false;
	showMenu = true;
	drawingPrintMap = false;

	// load texture
	menuTextures[0] = load_and_bind_texture("textures/start.png");
	menuTextures[1] = load_and_bind_texture("textures/instructions.png");
	menuTextures[2] = load_and_bind_texture("textures/gameover.png");


	//block textures
	blockTextures[0] = load_and_bind_texture("textures/marble.png");
	blockTextures[1] = load_and_bind_texture("textures/concrete.png");
	blockTextures[2] = load_and_bind_texture("textures/wood.png");
	blockTextures[3] = load_and_bind_texture("textures/bricks.png");

	//ball textures
	ballTextures[0] = load_and_bind_texture("textures/sphere-map.png");
	ballTextures[1] = load_and_bind_texture("textures/gold-sphere-map.png");


}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

	glutInitWindowSize(windowWidth+ mapWidth, windowHeight);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Atari Breakout 3d");

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutPassiveMotionFunc(mouseMotion);

	//visibility function
	glutVisibilityFunc(visibility);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	init(argc, argv);

	glutMainLoop();

	return 0;
}
