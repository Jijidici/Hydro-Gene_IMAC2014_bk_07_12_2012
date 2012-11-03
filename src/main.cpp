#include <iostream>
#include <cstdlib>
#include <cmath>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imac2gl3/shader_tools.hpp"

#define FRAME_RATE 60

using namespace std;

typedef GLdouble Point[3];

typedef struct{
	GLdouble left;
	GLdouble right;
	GLdouble top;
	GLdouble bottom;
	GLdouble far;
	GLdouble near;
	int nbVertices;
}Cube;

typedef struct{
	GLdouble s1X;
	GLdouble s1Y;
	GLdouble s1Z;
	
	GLdouble s2X;
	GLdouble s2Y;
	GLdouble s2Z;
	
	GLdouble s3X;
	GLdouble s3Y;
	GLdouble s3Z;
}Face;

typedef struct{
	GLdouble x;
	GLdouble y;
	GLdouble z;
}Axis;

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

Face createFace(GLdouble inS1X, GLdouble inS1Y, GLdouble inS1Z, GLdouble inS2X, GLdouble inS2Y, GLdouble inS2Z, GLdouble inS3X, GLdouble inS3Y, GLdouble inS3Z){
	Face newFace;
	newFace.s1X = inS1X; newFace.s1Y = inS1Y; newFace.s1Z = inS1Z;
	newFace.s2X = inS2X; newFace.s2Y = inS2Y; newFace.s2Z = inS2Z;
	newFace.s3X = inS3X; newFace.s3Y = inS3Y; newFace.s3Z = inS3Z;
	
	return newFace;
}

Axis createAxis(GLdouble inX, GLdouble inY, GLdouble inZ){
	Axis newAxis;
	newAxis.x = inX;
	newAxis.y = inY;
	newAxis.z = inZ;
	
	return newAxis;
}

Face projection(Face face, Axis axis){
	
	Axis projectionAxis = createAxis(0.,0.,0.);
	
	if(axis.x == 1.) projectionAxis = createAxis(0.,1.,1.);
	if(axis.y == 1.) projectionAxis = createAxis(1.,0.,1.);
	if(axis.z == 1.) projectionAxis = createAxis(1.,1.,0.);
	
	
	Face projectedFace = createFace(
		face.s1X * projectionAxis.x, face.s1Y * projectionAxis.y, face.s1Z * projectionAxis.z,
		face.s2X * projectionAxis.x, face.s2Y * projectionAxis.y, face.s2Z * projectionAxis.z,
		face.s3X * projectionAxis.x, face.s3Y * projectionAxis.y, face.s3Z * projectionAxis.z
	);
	
	return projectedFace;
}

bool insideVertexTest(Cube cube, Face face){
	//cout << "cube.left : " << cube.left << " sommet.x : " << face.s1X << " cube.right : " << cube.right << endl;
	//cout << "cube.bottom : " << cube.bottom << " sommet.y : " << face.s1Y << " cube.top : " << cube.top << endl;
	if(face.s1X >= cube.left 	&& face.s1X <= cube.right
	&& face.s1Y >= cube.bottom 	&& face.s1Y <= cube.top
	&& face.s1Z >= cube.far 	&& face.s1Z <= cube.near)
	{return true;}
	if(face.s2X >= cube.left 	&& face.s2X <= cube.right
	&& face.s2Y >= cube.bottom 	&& face.s2Y <= cube.top
	&& face.s2Z >= cube.far 	&& face.s2Z <= cube.near)
	{return true;}
	if(face.s3X >= cube.left 	&& face.s3X <= cube.right
	&& face.s3Y >= cube.bottom 	&& face.s3Y <= cube.top
	&& face.s3Z >= cube.far 	&& face.s3Z <= cube.near)
	{return true;}
	
	return false;
}

static const Uint32 MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;
static const size_t GRID_3D_SIZE = 2;

int main(int argc, char** argv) {

	/* ************************************************************* */
	/* **************PRE - TRAITEMENT DES VOXELS******************** */
	/* ************************************************************* */
	
	//CHARGEMENT FICHIERS .DATA
	//on charge le fichier en mode "read binary"
	FILE *fichier = NULL;
	fichier = fopen("hg_petit/page_1.data", "rb");
	if(NULL == fichier) cout << "impossible de charger le fichier" << endl;

	//lecture du nombre de vertex et de faces, puis affichage ds la console
	int nbVertice = 0, nbFace = 0;	
	fread(&nbVertice, sizeof(nbVertice), 1, fichier);
	fread(&nbFace, sizeof(nbFace), 1, fichier);
	cout << "Number of vertices : " << nbVertice << endl;
	cout << "Number of faces : " << nbFace << endl;
	
	//lecture des coordonnées de chaque vertex (tableau contenant les trois coordonnées de chaque vertex mises à la suite)
	int const sizeTabVertice(nbVertice*3); //on fixe d'abord la taille que le tableau va prendre en fonction du nombre de vertex
	GLdouble* tabV = new GLdouble[sizeTabVertice]; //on crée le tableau (j'ai voulu l'initialiser mais ça buggait alors tant pis)
	//cout<< <<
	fread(tabV, sizeTabVertice*sizeof(GLdouble), 1, fichier); //on remplit le tableau
	
	
	//lecture des index des points qui constituent chaque face (idem)
	int const sizeTabFace(nbFace*3);
	GLuint* tabF = new GLuint[sizeTabFace];
	fread(tabF, sizeTabFace*sizeof(GLuint), 1, fichier);

	fclose(fichier);
	
	/* ************************************************************* */
	/* *************INITIALISATION OPENGL/SDL*********************** */
	/* ************************************************************* */

	// Initialisation de la SDL
	SDL_Init(SDL_INIT_VIDEO);

	// Creation de la fenêtre et d'un contexte OpenGL
	SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BYTES_PER_PIXEL, SDL_OPENGL);

	// Initialisation de GLEW
	GLenum error;
	if(GLEW_OK != (error = glewInit())) {
		std::cerr << "Impossible d'initialiser GLEW: " << glewGetErrorString(error) << std::endl;
		return EXIT_FAILURE;
	}
	
	GLdouble* vertices = new GLdouble[sizeTabVertice];
	for(int n=0; n<sizeTabVertice; ++n){
			vertices[n] = tabV[n];
	}
	
	
	// Creation des VBO, VAO
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeTabVertice*sizeof(GLdouble), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);  
	glBindVertexArray(vao);  
		glEnableVertexAttribArray(POSITION_LOCATION);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(POSITION_LOCATION, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Creation des Shaders
	GLuint program = imac2gl3::loadProgram("shaders/basic.vs.glsl", "shaders/basic.fs.glsl");
	if(!program){
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		return (EXIT_FAILURE);
	}
	glUseProgram(program);

	// Creation des Matrices
	GLint MVPLocation = glGetUniformLocation(program, "uMVPMatrix");

	glm::mat4 P = glm::perspective(90.f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.f);
	glm::mat4 V = glm::lookAt(glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f,1.f,0.f));
	glm::mat4 VP = P*V;
	
	// Creation des ressources OpenGL
	glEnable(GL_DEPTH_TEST);
	
	//Creation des ressources d'evenements
	float offsetViewX = 0.;
	float offsetViewY = 0.;
	float offsetViewZ = 0.;
	float angleViewY = 0.;
	float tmpAngleViewY = 0.;
	float angleViewX = 0.;
	float tmpAngleViewX = 0.;
	int isArrowKeyUpPressed = 0;
	int isArrowKeyDownPressed = 0;
	int isArrowKeyLeftPressed = 0;
	int isArrowKeyRightPressed = 0;
	int isLeftClicPressed = 0;
	int savedClicX = -1;
	int savedClicY = -1;
	
	/* ************************************************************* */
	/* ********************DISPLAY LOOP***************************** */
	/* ************************************************************* */
	bool done = false;
	while(!done) {
		// Initilisation compteur
		Uint32 start = 0;
		Uint32 end = 0;
		Uint32 ellapsedTime = 0;
		start = SDL_GetTicks();

		// Nettoyage de la fenêtre
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		glm::mat4 MVP = glm::translate(VP, glm::vec3(offsetViewX, offsetViewY, offsetViewZ)); //MOVE WITH ARROWKEYS & ZOOM WITH SCROLL
		MVP = glm::translate(MVP, glm::vec3(0.f, 0.f, -5.f)); //MOVE AWWAY FROM THE CAMERA
		MVP = glm::rotate(MVP, angleViewX + tmpAngleViewX,  glm::vec3(0.f, 1.f, 0.f)); //ROTATE WITH XCOORDS CLIC
		MVP = glm::rotate(MVP, angleViewY + tmpAngleViewY,  glm::vec3(1.f, 0.f, 0.f)); //ROTATE WITH YCOORDS CLIC
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
		
		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, nbVertice);
		glBindVertexArray(0);

		// Mise à jour de l'affichage
		SDL_GL_SwapBuffers();

		// Boucle de gestion des évenements
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			switch(e.type){
				case SDL_QUIT:
					done=true;
				break;
				
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym){
						case SDLK_q:
							done=true;
						break;
						
						case SDLK_LEFT:
							isArrowKeyLeftPressed = 1;
						break;
						
						case SDLK_RIGHT:
							isArrowKeyRightPressed = 1;
						break;
						
						case SDLK_UP:
							isArrowKeyUpPressed = 1;
						break;
						
						case SDLK_DOWN:
							isArrowKeyDownPressed = 1;
						break;
						
						default:
						break;
					}
				break;
				
				case SDL_KEYUP:
					switch(e.key.keysym.sym){
						case SDLK_LEFT:
							isArrowKeyLeftPressed = 0;
						break;
						
						case SDLK_RIGHT:
							isArrowKeyRightPressed = 0;
						break;
						
						case SDLK_UP:
							isArrowKeyUpPressed = 0;
						break;
						
						case SDLK_DOWN:
							isArrowKeyDownPressed = 0;
						break;
						
						default:
						break;
					}
				break;
				
				case SDL_MOUSEBUTTONDOWN:
					switch(e.button.button){
						case SDL_BUTTON_WHEELUP:
							offsetViewZ += 0.5;
						break;
						
						case SDL_BUTTON_WHEELDOWN:
							offsetViewZ -= 0.5;
						break;
						
						case SDL_BUTTON_LEFT:
							isLeftClicPressed = 1;
							savedClicX = e.button.x;
							savedClicY = e.button.y;
						break;
						
						default:
						break;
					}
				break;
				
				case SDL_MOUSEBUTTONUP:
					isLeftClicPressed = 0;
					savedClicX = -1;
					savedClicY = -1;
					angleViewX += tmpAngleViewX;
					angleViewY += tmpAngleViewY;
					tmpAngleViewX=0;
					tmpAngleViewY=0;
				break;
				
				case SDL_MOUSEMOTION:
					if(isLeftClicPressed){
						tmpAngleViewX =  0.25f*(e.motion.x - savedClicX);
						tmpAngleViewY =  0.25f*(e.motion.y - savedClicY);
					}
				break;
				
				default:
				break;
			}
		}

		//Idle
		if(isArrowKeyLeftPressed){
			offsetViewX += 0.1;
		}
		
		if(isArrowKeyRightPressed){
			offsetViewX -= 0.1;
		}
		
		if(isArrowKeyUpPressed){
			offsetViewY -= 0.1;
		}
		
		if(isArrowKeyDownPressed){
			offsetViewY += 0.1;
		}
		
		// Gestion compteur
		end = SDL_GetTicks();
		ellapsedTime = end - start;
		if(ellapsedTime < MIN_LOOP_TIME){
			SDL_Delay(MIN_LOOP_TIME - ellapsedTime);
		}
	}

	// Destruction des ressources OpenGL
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	
	delete[] tabV;
	delete[] tabF;
	delete[] vertices;

	SDL_Quit();

	return EXIT_SUCCESS;
}
