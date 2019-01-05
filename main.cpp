#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif
#ifdef __WIN32__
#include <random>
void srand48(long) {};
double drand48() {
static std::ranlux48 source(std::random_device{}());
return std::uniform_real_distribution<double>(0,1)(source);
}
#endif
#include <iostream>

#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include <vector>
#include <cmath>


int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH); 

	glutInitWindowSize(1000, 1000); 
	glutInitWindowPosition(50,50); 

	glutCreateWindow("Simple maze"); 

	glutKeyboardFunc(keyboard); 
	glutReshapeFunc(reshape); 
	glutDisplayFunc(display); 
	glutIdleFunc(idle);

	init(); 

	glutMainLoop(); 

	return 0; 
}