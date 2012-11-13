#ifndef __TYPE_HPP__
#define __TYPE_HPP__

#include <GL/glew.h>

typedef struct s_point{
	GLdouble * x;
	GLdouble * y;
	GLdouble * z;
}Point;

typedef struct s_vertex{
	Point pos;
}Vertex;

typedef struct s_face{
	Vertex *s1, *s2, *s3;
}Face;

typedef struct s_cube{
	GLdouble left;
	GLdouble right;
	GLdouble top;
	GLdouble bottom;
	GLdouble far;
	GLdouble near;
	uint8_t nbVertices;
}Cube;

/******************************************/
/*          FUNCTIONS                     */
/******************************************/

Cube createCube(GLdouble inLeft, GLdouble inRight, GLdouble inTop, GLdouble inBottom, GLdouble inFar, GLdouble inNear){
	Cube newCube;
	newCube.left = inLeft;
	newCube.right = inRight;
	newCube.top = inTop;
	newCube.bottom = inBottom;
	newCube.far = inFar;
	newCube.near = inNear;
	newCube.nbVertices = 36;
	
	return newCube;
}

// Création du tableau des points du cube envoyé en paramètre à la fonction de test d'intersection AABB
Point createPoint(GLdouble inx, GLdouble iny, GLdouble inz){
	Point p;
	p.x = &inx;
	p.y = &iny;
	p.z = &inz;
	
	return p;
}

Face createFace(Vertex* inS1, Vertex* inS2, Vertex* inS3){
	Face newFace;
	newFace.s1 = inS1;
	newFace.s2 = inS2;
	newFace.s3 = inS3;
	
	return newFace;
}

#endif
