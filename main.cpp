#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
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

#include "draw_text.h"	//uses header file from labs to draw instructions on the screen
#define PI 3.1415926538
#define print(a) std::cout << #a << ": " << (a) << std::endl
const float worldWidth = 1000.0f;

std::array<float,2> courtXboundary;	//stores the 2 x boundarys in an array [left,right]
std::array<float,2> courtYboundary;	//stores the 2 y boundarys in an array 
std::array<float,2> courtZboundary; //stroes the 2 z boundarys in an array

bool gameStarted;	//set to true when the game has begun
bool endGame;		//set to true if game is lost

enum Difficulty {EASY, MEDIUM, HARD};
enum Direction {STILL, LEFT, RIGHT, UP, DOWN};
// enum Direction3d {LEFT, RIGHT, UP, DOWN, FORWARD, BACKWARD};
// enum BallDirection {TO_PADDLE, FROM_PADDLE};



class Court{
	const float border;
public:
	const float widthMult;	//stores the width of the court as a percentage of the full display
	Court() : widthMult(0.80f), border(3.0f){}
	void draw(){
		glColor3f(0.5f,0.5f,1.0f);
		glLineWidth(border);
		glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5f,-0.5f);
			glVertex2f(0.5f,-0.5f);
			glVertex2f(0.5f,0.5f);
			glVertex2f(-0.5f,0.5f);
		glEnd();
	}

	std::array<float,2> getXboundary(){
		float courtWidth = worldWidth * widthMult;
		std::array<float,2> boundaries;
		boundaries[0] = ((worldWidth - courtWidth) / 2.0f) + (border*3.0);	//gets the x position of the left side court boundary
		boundaries[1] = courtWidth + boundaries[0] -6*border;			//gets the y position of the right side court boundary
		return boundaries;
	}

	std::array<float,2> getYboundary(){
		return getXboundary();
	}
};

// Class to store data on the main paddle such as colour, width etc
class Paddle{
	float width;
	float height;
	const float yAdd;	// sets how far the centre of the paddle should be from the court bottom
public:
	float xOffset;	//initialised to middle of the screen, when moving the paddle this is altered
	float yOffset;
	float xStep;	//controls the speed of the paddle
	float scaleFac;	//The height and width of the paddle will be times by this to fit on the display
	Direction currDirection;
	Difficulty currDifficulty;	//stores the current difficulty level
	bool moving;	//used to check whether paddle is currently moving
	//first define constructor
	Paddle() : xOffset(500.0f), xStep(10.0f), height(0.2f), scaleFac(100.0f), yAdd(10.0f){}
	//sets a different level of difficulty
	void setDifficulty(Difficulty level){
		switch(level){
			case EASY:
			//set width of the paddle to be large
				width = 3.0f; currDifficulty = EASY; break;
			case MEDIUM:
				width = 2.0f; currDifficulty = MEDIUM; break;
			case HARD:
				width = 1.0f; currDifficulty = HARD; break;
		}
	}
	//draws the paddle  
	void draw(){
		glColor3f(0.5, 0.1, 0.1);
		glBegin(GL_QUADS);
			glVertex2f(-width/2.0f,height);
			glVertex2f(-width/2.0f,0.0f);
			glVertex2f(width/2.0f, 0.0f);
			glVertex2f(width/2.0f, height);
		glEnd();
	}

	void restart(Court c){
		xOffset = 500.0f;
		moving = false;
		setDifficulty(MEDIUM);
		setYoffset(c);
	}

	void move(){
		float x_rightEdge = xOffset + ((width/2.0f)*scaleFac);
		float x_leftEdge = xOffset - ((width/2.0f)*scaleFac);
		if (currDirection == LEFT){
			if (x_leftEdge > courtXboundary[0]) xOffset -= xStep;
		}else if (currDirection == RIGHT){
			if (x_rightEdge <courtXboundary[1]) xOffset += xStep;
		}
	}

	std::array<float,2> getXcoords(){
		std::array<float,2> coord;
		coord[0] = xOffset - (width * scaleFac/2.0f);
		coord[1] = xOffset + (width * scaleFac/2.0f);
		return coord;
	}

	void setYoffset(Court c){
		yOffset = c.getYboundary()[0] + yAdd;
	}

	float getHeight(){
		return height*scaleFac;
	}

};


class Ball{
	float angle;	//angle in radians
	Direction yDirection;	//controls whether the ball is travelling towards paddle(down) or away from paddle(down)
	Direction xDirection;	//controls whether the ball is moving left or right

	float xStep;	//controls the speed in the x direction
	float yStep;	//controls the speed in the y direction
public:
	float xOffset;	//x coord of the ball
	float yOffset;	//y coord of the ball
	float speed;	//controls how many pixels should be moved each time the ball is drawn
	
	bool  moving;
	float radius;
	Ball (): radius(20.0f), xOffset(500.0f), yOffset(500.0f), yDirection(DOWN){} 
	void setDifficulty(Difficulty level){
		switch (level){
			case EASY:
				speed = 1.0f;
				break;
			case MEDIUM:
				speed = 5.0f;
				break;
			case HARD:
				speed = 10.0f;
				break;
		}
	}
	void draw(){
		glColor3f(0.1, 0.5, 0.1);
		unsigned int numTriangles = 30; //# of triangles used to draw circle

		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);	//center of circle
		for(int i = 0; i <= numTriangles; i++) { 
			GLfloat x = cos(i * 2.0f * PI / numTriangles);
			GLfloat y = sin(i * 2.0f * PI / numTriangles);
			glVertex2f(x, y);
		}
		glEnd();
	}

	//restart function, called when the game is restarted
	void restart(){
		xOffset = 500.0f;
		yOffset = 500.0f;
		yDirection = DOWN;
		setDifficulty(MEDIUM);
		randomAngle();
		moving = false;
	}

	void randomAngle(){
		srand (static_cast <unsigned int> (time(0)));	//used to ensure true randomness
		for (int x =0; x<((rand() *1000)/RAND_MAX);x++)
			angle = static_cast <float> (rand()) / (static_cast <float> ((RAND_MAX)/(PI/2.0f))) + (PI/4.0f);
		std::cout << "starting angle = " << angle << std::endl;
		if(angle>(PI/2.0f)) xDirection = LEFT;
		else xDirection = RIGHT;

		xStep = speed * cos(angle);
		yStep = speed * sin(angle);
		if (yDirection == DOWN){
			yStep = -abs(yStep);
		}
	}

	void move(Paddle p){
		float paddleTopCoord = p.yOffset + (p.getHeight() / 2.0f);
		float x_rightEdge = xOffset + radius;
		float x_leftEdge = xOffset - radius;
		float y_bottomEdge = yOffset - radius;
		float y_topEdge = yOffset + radius;
		
		if (xDirection == LEFT && x_leftEdge <= courtXboundary[0]){
			xStep = -xStep;
			xDirection = RIGHT;
			std::cout << "x direction now right\n";
		}else if(xDirection == RIGHT && x_rightEdge >= courtXboundary[1]){
			xStep = -xStep;
			xDirection = LEFT;
			std::cout << "x direction now keft\n";
		}
		if (y_topEdge >= courtYboundary[1]){
			yStep = -yStep;
			yDirection = DOWN;
		}else if (y_bottomEdge-radius <= paddleTopCoord){
			std::cout << "paddle top coord ="<<paddleTopCoord<<"\n";
			std::array<float,2> xCoord = p.getXcoords();
			if ((xOffset+speed) >= xCoord[0] && (xOffset-speed) <= xCoord[1]){
				angle = this->getPaddleAngle(xOffset,p);
				xStep = speed * cos(angle);
				yStep = speed * sin(angle);
				if(angle>(PI/2.0f)) xDirection = LEFT;
				else xDirection = RIGHT;
				yDirection = UP;
			}
		}
		if (y_bottomEdge-speed <= courtYboundary[0]){
			// std::cout << xCoord[0]<<" "<<xCoord[1]  <<" "<<xOffset<<std::endl;
			moving = false;
			endGame = true;
		}
		xOffset += xStep;
		yOffset += yStep;
	}

	//divides paddle into a set number of x intervals. The angle the ball bounces off at depends on the interval it lands on
	float getPaddleAngle(float xpos, Paddle p){
		float angle = PI/4.0f;
		const int numSteps = 5;		//used
		const float angleStep = ( PI/2.0f ) / (static_cast <float> (numSteps));
		std::array<float,2> xCoord = p.getXcoords();
		std::array<float,numSteps+1> interval;	//stores
		interval[0] = xCoord[0];
		float distanceStep = (xCoord[1] - xCoord[0]) / (static_cast <float> (numSteps));
		for(int i=1; i<=numSteps; i++){
			interval[i] = interval[i-1] + distanceStep;
			if (xpos>interval[i-1] && xpos<interval[i]){
				angle += ((numSteps+1) - i)* angleStep;
				break;
			}
		}
		return angle;
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

	Direction * getDirection(){
		static Direction directions[2] = {xDirection,yDirection};
		return directions;
	}

	Direction getXdirection(){ return xDirection;}
	Direction getYdirection(){ return yDirection;}


};



class Block{
	GLfloat color[3];
	float position;
	float width;
	float height;
	
	
	
public:
	float xCoord;
	float yCoord;
	bool hit;	//set to true when the block has been hit
	Block() {
		hit=false;
	}
	void setXY(float x, float y){
		xCoord = x;
		yCoord = y;
	}

	void setHeight(float h){
		height = h;
	}
	float getHeight(){return height;}

	void setWidth(float w){
		width = w;
	}
	float getWidth(){return width;}

	void setColor(GLfloat col[3]){
		for (int i = 0; i < 3; i++) {
			color[i] = col[i];
		}
	}
	
	// void draw(){
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glPushMatrix();
	// 		glColor3f(color[0],color[1],color[2]);
	// 		glTranslatef(xCoord, yCoord,0.0f);
	// 		glScalef(width, height, 1.0f);
			
	// 	glPopMatrix();
	// }

	//uses display lists to draw blocks efficiently.
	void draw(unsigned int makeblock){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glColor3f(color[0],color[1],color[2]);
			glTranslatef(xCoord, yCoord,0.0f);
			glScalef(width, height, 1.0f);
			glCallList(makeblock);	
		glPopMatrix();
	}

	static unsigned int make(){
		unsigned int handle = glGenLists(1); 
		glNewList(handle, GL_COMPILE);
			glBegin(GL_QUADS);
				glVertex2f(-0.5f,-0.5f);
				glVertex2f(0.5f,-0.5f);
				glVertex2f(0.5f,0.5f);
				glVertex2f(-0.5f,0.5f);
			glEnd();
		glEndList();
		return handle;
	}
};

//Class storing a whole row of blocks. WHen draw is called each block will be drawn
class RowBlocks{
	unsigned int numBlocks;
	unsigned int pointsPerBlock;
	unsigned int rowLevel;		//stores the level of the row, this is used to calulate the score
	unsigned int basicBlock;		//will be used to execute a pre-compiled display list for all block	
	
	GLfloat color[3];
	
	float blockWidth;
	float height;
	float score;
	bool colorSet;				//used to stop the generation of random colors

public:
	unsigned int numHit;		//stores the number of blocks that have been hit in the row
	std::vector<Block> blocks;
	RowBlocks(float courtWidth, unsigned int rowlvl){
		numBlocks = 5;
		// print(courtWidth);
		blockWidth = courtWidth/(static_cast <float> (numBlocks));
		// print(blockWidth);
		rowLevel = rowlvl;
		pointsPerBlock = 10;
		height = 30.0f;
		colorSet = false;
	}

	void setColor(GLfloat col[3]){
		if (!colorSet){
			for (int i = 0; i < 3; i++) {
				color[i] = col[i];
			}
			colorSet = true;
		}
	}
	
	void init(float gapHeight){
		// request a single display list handle
		basicBlock = Block::make();
		 
		print(basicBlock);
		print("hello");
		for(int i =0; i<numBlocks; i++){
			Block b;
			b.setColor(color);
			b.setWidth(blockWidth);
			b.setHeight(height);
			float xCoord = courtXboundary[0] + ((i * blockWidth) + (blockWidth/2.0f));
			float yCoord = courtYboundary[1]- gapHeight - ((rowLevel * height)+(height/2.0f));
			b.setXY(xCoord,yCoord);
			blocks.push_back(b);
		}
	}

	void draw(){
		// std::cout<< " blockwidth = "<<blockWidth<< " courtWidth ="<< courtWidth<<"\n";
		for (Block b : blocks){
			// print(1);
			if (!b.hit)
				b.draw(basicBlock);
			// else
				// std::cout << "b has seen to be hit\n";
		}
	}

	unsigned int getScore(){
		numHit = 0;
		for(Block b:blocks){
			if(b.hit) numHit+=1;
		}
		return numHit * pointsPerBlock * rowLevel;
	}
};

class Grid{
	unsigned int score;
	unsigned int numRows;
	float gapHeight;
	std::vector<RowBlocks> grid;
public:
	Grid(){
		numRows = 4; 
		score = 0;
		gapHeight = 100.0;
	}
	unsigned int getScore(){
		return score;
	}

	void init(){
		GLfloat colors[3];
		for (unsigned int i = 0; i < numRows; i++){
			for (int i = 0; i < 3; i++){
				colors[i] = randomColor();
			}
			float courtWidth = courtXboundary[1] - courtXboundary[0];			
			RowBlocks r(courtWidth,i);
			r.setColor(colors);
			r.init(gapHeight);
			// std::cout<<"one row\n";
			grid.push_back(r);
		}
	}

	void restart(){
		for(RowBlocks& rb: grid){
			rb.numHit = 0;
			for(Block& b : rb.blocks){
				b.hit = false;
			}
		}
	}


	void draw(){
		for (RowBlocks r : grid){
			r.draw();
		}
	}

	GLfloat randomColor(){
		GLfloat color;
		color = static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX));
		return color;
	}

	void updateGrid(Ball& ball){
		float ballBottomEdge = ball.yOffset - ball.radius;
		float ballTopEdge = ball.yOffset + ball.radius;
		float ballLeftEdge = ball.xOffset - ball.radius;
		float ballRightEdge = ball.xOffset + ball.radius;

		Direction * ptr = ball.getDirection();			//returns pointer to float array containing the direction the ball is travelling
		Direction ballDirection[2] = {*(ptr),*(ptr+1)};
		for (RowBlocks& rb : grid){
			for (Block& b : rb.blocks){
				if (!b.hit){
					float halfHeight = b.getHeight() / 2.0f;
					float halfWidth = b.getWidth() / 2.0f;

					if(ballLeftEdge >= b.xCoord - halfWidth && ballRightEdge <= b.xCoord +halfWidth){
						//handles if the ball hits the top/bottom of a block
						if(ballTopEdge >= (b.yCoord - halfHeight) && ballTopEdge < (b.yCoord + halfHeight)){
							if (ball.getYdirection() == UP){
								b.hit= true;
								print(b.hit);
								rb.numHit +=1;
								ball.reverseYdirection();
								std::cout << "touched b\n";
								goto endloop;
							}
						}
						if(ballBottomEdge <= (b.yCoord + halfHeight) && ballBottomEdge > (b.yCoord - halfHeight)){
							if (ball.getYdirection() == DOWN){
								b.hit= true;
								print(b.hit);
								rb.numHit +=1;
								ball.reverseYdirection();
								std::cout << "touched b\n";
								goto endloop;
							}
						}
					}
					//handles if the ball hits the side of a block
					if (ball.yOffset > (b.yCoord - halfHeight) && ball.yOffset < (b.yCoord +halfHeight)){
						
						if(ballLeftEdge  <= (b.xCoord + halfWidth) && ballLeftEdge > (b.xCoord - halfWidth)){
							if (ball.getXdirection() == LEFT){
								b.hit= true;
								print(b.hit);
								rb.numHit +=1;
								ball.reverseXdirection();
								std::cout << "touched b\n";
								goto endloop;
							}
						}
						if(ballRightEdge >= (b.xCoord - halfWidth) && ballRightEdge < (b.xCoord + halfWidth)){
							if (ball.getXdirection() == RIGHT){
								b.hit= true;
								print(b.hit);
								rb.numHit +=1;
								ball.reverseXdirection();
								std::cout << "touched b\n";
								goto endloop;
							}
						}
					}
				}
			}
		}
	endloop:;
		// std::cout << "touched endloop\n";


	}

};



Court court;
Paddle paddle;
Ball ball;
Grid grid;

bool movementIdle = false;	//so we can move paddle when key down

void display()
{
	float xTranslate, yTranslate;
	float xScale, yScale;
	glClear(GL_COLOR_BUFFER_BIT); 
	if (!endGame){
		// work on MODELVIEW matrix stack
		glMatrixMode(GL_MODELVIEW);

		//drawing arena
		glPushMatrix();
			xTranslate = yTranslate = worldWidth/2.0f;
			xScale = yScale = worldWidth * court.widthMult;
			
			// std::cout << "xscale " << worldWidth*court.widthMult;
			glTranslatef(xTranslate, yTranslate,0.0f);
			glScalef(xScale, yScale, 1.0f);
			court.draw();
		glPopMatrix();

		//drawing the paddle
		glPushMatrix();
			xTranslate = paddle.xOffset;
			yTranslate = paddle.yOffset;
			xScale = yScale = paddle.scaleFac;
			glTranslatef(xTranslate, yTranslate,0.0f);
			glScalef(xScale, yScale, 1.0f);
			if (paddle.moving) paddle.move();
			paddle.draw();
		glPopMatrix();

		

		//drawing the ball
		glPushMatrix();
			xTranslate = ball.xOffset;
			yTranslate = ball.yOffset;
			xScale = yScale = ball.radius;
			glTranslatef(xTranslate, yTranslate,0.0f);
			glScalef(xScale, yScale, 1.0f);
			if (ball.moving) ball.move(paddle);
			ball.draw();
		glPopMatrix();

		//checks if the ball has hit any of the blocks
		grid.updateGrid(ball);

		//drawing blocks
		glPushMatrix();
			grid.draw();
		glPopMatrix();

		glLineWidth(1.0f);
		if (!gameStarted)
			draw_text(20, 50, "Press Space to start the game");

		draw_text(20, 20, "Use 'd' to toggle difficulty");
		draw_text(20,worldWidth-40,"Score:");
		draw_text(worldWidth - 200, worldWidth-40,"HighScore");
	}else{
		draw_text(worldWidth/4, worldWidth/2, "Press s to restart game");
	}
	//}
	glutSwapBuffers(); 
}

// will get which key was pressed and x and y positions if required
void keyboard(unsigned char key, int, int)
{
	std::cerr << "\t you pressed the " << key << " key" << std::endl;

	switch (key)
	{
		case 'q': exit(1); // quit!

		// change difficulty
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
		case ' ':
			gameStarted = true;
			ball.moving = true;
			break;
		case 's':
			grid.restart();
			ball.restart();
			paddle.restart(court);
			endGame = false;
	}

	glutPostRedisplay(); // force a redraw
}

// any special key pressed like arrow keys
void specialDown(int key, int, int)
{
	// handle special keys
	switch (key)
	{
		case GLUT_KEY_LEFT: 
			paddle.currDirection = LEFT; 
			paddle.moving = true;
			break;
		case GLUT_KEY_RIGHT: 
			paddle.currDirection = RIGHT; 
			paddle.moving = true;
			break;
		// case GLUT_KEY_UP: g_yoffset += g_offset_step; break;
		// case GLUT_KEY_DOWN: g_yoffset -= g_offset_step; break;
	}

	glutPostRedisplay(); // force a redraw
}

void specialUp(int key, int, int)
{
	paddle.moving = false;
}

// idle handler
void idle()
{
	// std::cerr << "\t idle handler called..." << std::endl;
	usleep(10000);
	if (paddle.moving || ball.moving)	//checks if either the paddle or the ball are in motion
		glutPostRedisplay();
}

void visibility(int vis)
{
	if (vis==GLUT_VISIBLE)
	{
		std::cerr << "\t visible" << std::endl;
		glutIdleFunc(idle);
	}
	else
	{
		std::cerr << "\t NOT visible" << std::endl;
		glutIdleFunc(NULL);
	}
}

void init()
{
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0, worldWidth, 0, worldWidth);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	
	courtXboundary = court.getXboundary();
	courtYboundary = court.getYboundary();

	paddle.setDifficulty(MEDIUM);
	paddle.moving = false;
	paddle.setYoffset(court);

	ball.setDifficulty(MEDIUM);
	ball.moving = false;
	ball.randomAngle();
	std::cout<< time(0);

	grid.init();
	gameStarted = false;

}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA); 
	glutInitWindowSize(500, 500); 
	glutInitWindowPosition(50, 50); 
	glutCreateWindow("Atari Breakout"); 
	glutDisplayFunc(display); 

	// handlers for keyboard input
	glutKeyboardFunc(keyboard); 
	glutSpecialFunc(specialDown); 
	glutSpecialUpFunc(specialUp); 

	//visibility function
	glutVisibilityFunc(visibility); 

	init(); 
	glutMainLoop(); 

	return 0; 
}