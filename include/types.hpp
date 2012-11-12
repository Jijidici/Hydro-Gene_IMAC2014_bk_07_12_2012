#ifndef __TYPE_HPP__
#define __TYPE_HPP__

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

#endif
