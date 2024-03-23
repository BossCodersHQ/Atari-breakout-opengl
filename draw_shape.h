#ifndef DRAWSHAPE_H
#define DRAWSHAPE_H
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string.h>
#include <stddef.h>

#define PI 3.1415926538
//draws the vertices for the cube structure
;
void drawCourt(bool drawTop, float width, float height, float length){
   float halfWidth = width/2.0f;
   float halfHeight = height/2.0f;
   float halfLength = length/2.0f;
   glBegin(GL_QUADS);

   //Right
   glNormal3f(-1.0f, 0.0f, 0.0f);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);

   //Back
   glNormal3f(0.0f, 0.0f, 1.0f);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);

   //Left
   glNormal3f(1.0f, 0.0f, 0.0f);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);

   //TOP
   if (drawTop){
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-halfWidth, halfHeight, halfLength);
      glVertex3f(halfWidth, halfHeight, halfLength);
      glVertex3f(halfWidth, halfHeight, -halfLength);
      glVertex3f(-halfWidth, halfHeight, -halfLength);
   }


   //Bottom
   glNormal3f(0.0f, 1.0f, 0.0f);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);

   glEnd();
}

//draws the vertices for a Cuboid structure;
void drawCuboid(float width, float height, float length){
   float halfWidth = width/2.0f;
   float halfHeight = height/2.0f;
   float halfLength = length/2.0f;
   glBegin(GL_QUADS);


   //Right
   glNormal3f(1.0f, 0.0f, 0.0f);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);

   //Back
   glNormal3f(0.0f, 0.0f, -1.0f);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);

   //Left
   glNormal3f(-1.0f, 0.0f, 0.0f);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);

   //TOP
   glNormal3f(0.0f, 1.0f, 0.0f);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);

   //Bottom
   glNormal3f(0.0f, -1.0f, 0.0f);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);

   //Front
   glNormal3f(0.0f, 0.0f, 1.0f);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, halfLength);

   glEnd();

}

//draws the vertices for a Cuboid structure;
void drawCuboidBorder(float width, float height, float length){
   float halfWidth = width/2.0f;
   float halfHeight = height/2.0f;
   float halfLength = length/2.0f;

   //Right
   glBegin(GL_LINE_LOOP);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glEnd();

   //Back
   glBegin(GL_LINE_LOOP);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glEnd();

   //Left
   glBegin(GL_LINE_LOOP);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);
   glEnd();

   //TOP
   glBegin(GL_LINE_LOOP);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, -halfLength);
   glVertex3f(-halfWidth, halfHeight, -halfLength);
   glEnd();

   //Bottom
   glBegin(GL_LINE_LOOP);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, -halfLength);
   glVertex3f(-halfWidth, -halfHeight, -halfLength);
   glEnd();

   //Front
   glBegin(GL_LINE_LOOP);
   glVertex3f(-halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, -halfHeight, halfLength);
   glVertex3f(halfWidth, halfHeight, halfLength);
   glVertex3f(-halfWidth, halfHeight, halfLength);
   glEnd();

}

//draw circle
void drawCircle(float radius){
   unsigned int numTriangles = 30; //# of triangles used to draw circle

   glBegin(GL_TRIANGLE_FAN);
   // glNormal3f(0.0f, 0.0f, 1.0f);
   glVertex3f(0, 0, 0);	//center of circle
   for(int i = 0; i <= numTriangles; i++) {
      GLfloat x = radius * cos(i * 2.0f * PI / numTriangles);
      GLfloat y = radius * sin(i * 2.0f * PI / numTriangles);
      glVertex3f(x, y , 0.0f);
   }
   glEnd();
}

void drawRectangle(bool texture,bool filled, float width, float height){
   float halfWidth = width/2.0f;
   float halfHeight = height/2.0f;

   if (filled || texture) glBegin(GL_QUADS);
   else glBegin(GL_LINE_LOOP);

      if (filled || texture) glNormal3f(0.0f, 0.0f, 1.0f);
      if(texture) glTexCoord2f(0.0f,0.0f); // lower left corner
      glVertex3f(-halfWidth, 0.0f, -halfHeight);

      if (texture) glTexCoord2f(1.0f,0.0f);
      glVertex3f(halfWidth, 0.0f, -halfHeight);

      if (texture) glTexCoord2f(1.0f,1.0f);
      glVertex3f(halfWidth, 0.0f, halfHeight);

      if (texture) glTexCoord2f(0.0f,1.0f);
      glVertex3f(-halfWidth, 0.0f, halfHeight);

   glEnd();
}

//draw cross hairs
void drawCrossHairs(float width){
   float halfWidth = width /2.0f;
   glBegin(GL_LINES);
      //vertical line
      glVertex3f(halfWidth, 0.0f, 0.0f);
      glVertex3f(-halfWidth, 0.0f, 0.0f);
      glVertex3f(0.0f,-halfWidth, 0.0f);
      glVertex3f(0.0f, halfWidth, 0.0f);
   glEnd();
}

//draws a plane with lines running in one direction along it. Used to make grid when called twice
void drawLinedPlane(float width, float height){

   float halfWidth = width/2.0f;
   float halfHeight = height/2.0f;

   float numLines=3;
   float division;
   float temp;

   drawRectangle(false,false,width,height);
   glBegin(GL_LINES);

      division = height / numLines;
      for(int i=1; i<(numLines); i++){
         temp = -halfHeight + i*division;
         glVertex3f( -halfWidth, 0.0f,temp);
         glVertex3f( halfWidth, 0.0f,temp);
      }

      division = width / numLines;
      for(int i=1; i<(numLines); i++){
         temp = -halfWidth + i*division;
         glVertex3f( temp, 0.0f,-halfHeight);
         glVertex3f( temp, 0.0f,halfHeight);
      }
   glEnd();


}

//draws a grid lines in white
void drawGridLines(
   std::array<float,2> courtXboundary,
   std::array<float,2> courtYboundary,
   std::array<float,2> courtZboundary
){

   float xSize = courtXboundary[0] - courtXboundary[1] -2.0f;
   float ySize = courtYboundary[0] - courtYboundary[1]- 2.0f;
   float zSize = courtZboundary[0] - courtZboundary[1]- 2.0f;
   glPushMatrix();
      glColor4f(1.0f,1.0f,1.0f,0.3f);


      glLineWidth(1.0f);
      //bottom side
      glPushMatrix();
         glTranslatef(0.0f,courtYboundary[1] + 2.0f,0.0f);

         drawRectangle(false,false,xSize,zSize);
         drawLinedPlane(xSize,zSize);
      glPopMatrix();
      //back side
      glPushMatrix();
         glTranslatef(0.0f,0.0,courtZboundary[1]+2.0f);
         glRotatef(90.0f,1.0,0.0,0.0);
         drawRectangle(false,false,xSize,ySize);
         drawLinedPlane(xSize,ySize);
      glPopMatrix();
      //topside
      glPushMatrix();
         glTranslatef(0.0f,courtYboundary[0] - 2.0f,0.0f);
         drawRectangle(false,false,xSize,zSize);
         drawLinedPlane(xSize,zSize);
      glPopMatrix();
      //left side
      glPushMatrix();
         glTranslatef(courtXboundary[1] + 2.0f,0.0f,0.0f);
         glRotatef(90.0f,0.0,0.0,1.0);
         drawRectangle(false,false,ySize,zSize);
         drawLinedPlane(ySize,zSize);
      glPopMatrix();

      //right side
      glPushMatrix();
         glTranslatef(courtXboundary[0] - 2.0f,0.0f,0.0f);
         glRotatef(90.0f,0.0,0.0,1.0);
         drawRectangle(false,false,ySize,zSize);
         drawLinedPlane(ySize,zSize);
      glPopMatrix();
   glPopMatrix();
}

#endif
