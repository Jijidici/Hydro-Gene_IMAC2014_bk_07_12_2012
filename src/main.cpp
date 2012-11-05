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

typedef struct{
	GLdouble x;
	GLdouble y;
	GLdouble z;
}Point;

typedef struct{
	Point pos;
}Vertex;

typedef struct{
	Vertex *s1, *s2, *s3;
}Face;

typedef struct{
	GLdouble left;
	GLdouble right;
	GLdouble top;
	GLdouble bottom;
	GLdouble far;
	GLdouble near;
	int nbVertices;
}Cube;
/*
typedef struct{
	GLdouble x;
	GLdouble y;
	GLdouble z;
}Axis;
*/

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

Point * createBoxPoints(Cube cube){
	Point * boxPoints = new Point[8];
	Point LTN; LTN.x = cube.left; LTN.y = cube.top; LTN.z = cube.near;
	Point RTN; RTN.x = cube.right; RTN.y = cube.top; RTN.z = cube.near;
	Point RBN; RBN.x = cube.right; RBN.y = cube.bottom; RBN.z = cube.near;
	Point LBN; LBN.x = cube.left; LBN.y = cube.bottom; LBN.z = cube.near;
	Point LTF; LTF.x = cube.left; LTF.y = cube.top; LTF.z = cube.far;
	Point RTF; RTF.x = cube.right; RTF.y = cube.top; RTF.z = cube.far;
	Point RBF; RBF.x = cube.right; RBF.y = cube.bottom; RBF.z = cube.far;
	Point LBF; LBF.x = cube.left; LBF.y = cube.bottom; LBF.z = cube.far;
	
	boxPoints[0] = LTN;
	boxPoints[1] = RTN;
	boxPoints[2] = RBN;
	boxPoints[3] = LBN;
	boxPoints[4] = LTF;
	boxPoints[5] = RTF;
	boxPoints[6] = RBF;
	boxPoints[7] = LBF;
	
	return boxPoints;
}

Face createFace(Vertex* inS1, Vertex* inS2, Vertex* inS3){
	Face newFace;
	newFace.s1 = inS1;
	newFace.s2 = inS2;
	newFace.s3 = inS3;
	
	return newFace;
}

Point * createTriPoints(Face face){
	Point * triPoints = new Point[3];
	Point A,B,C;
	A.x = face.s1->pos.x;
	A.y = face.s1->pos.y;
	A.z = face.s1->pos.z;
	
	B.x = face.s2->pos.x;
	B.y = face.s2->pos.y;
	B.z = face.s2->pos.z;
	
	C.x = face.s3->pos.x;
	C.y = face.s3->pos.y;
	C.z = face.s3->pos.z;
	
	triPoints[0] = A;
	triPoints[1] = B;
	triPoints[2] = C;
	
	return triPoints;
}

/*
Axis createAxis(GLdouble inX, GLdouble inY, GLdouble inZ){
	Axis newAxis;
	newAxis.x = inX;
	newAxis.y = inY;
	newAxis.z = inZ;
	
	return newAxis;
}
*/

/*
bool insideVertexTest(Cube cube, Face face){ // IT WORKS !
	//cout << "cube.left : " << cube.left << " sommet.x : " << face.s1X << " cube.right : " << cube.right << endl;
	//cout << "cube.bottom : " << cube.bottom << " sommet.y : " << face.s1Y << " cube.top : " << cube.top << endl;
	if(face.s1->pos.x >= cube.left 		&& face.s1->pos.x <= cube.right
	&& face.s1->pos.y >= cube.bottom 	&& face.s1->pos.y <= cube.top
	&& face.s1->pos.z >= cube.far 		&& face.s1->pos.z <= cube.near)
	{return true;}
	if(face.s2->pos.x >= cube.left 		&& face.s2->pos.x <= cube.right
	&& face.s2->pos.y >= cube.bottom 	&& face.s2->pos.y <= cube.top
	&& face.s2->pos.z >= cube.far	 	&& face.s2->pos.z <= cube.near)
	{return true;}
	if(face.s3->pos.x >= cube.left 		&& face.s3->pos.x <= cube.right
	&& face.s3->pos.y >= cube.bottom 	&& face.s3->pos.y <= cube.top
	&& face.s3->pos.z >= cube.far 		&& face.s3->pos.z <= cube.near)
	{return true;}
	
	return false;
}
*/
/** OPERATIONS **/
glm::vec3 crossProduct(glm::vec3 v1, glm::vec3 v2){
	glm::vec3 result;
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;
	
	return result;
}

GLdouble dotProduct(glm::vec3 v1, glm::vec3 v2){
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

GLdouble dotProduct(Point p1, glm::vec3 v2){
	/*
	cout << "point.x : " << p1.x << " axe.x : " << v2.x << endl;
	cout << "point.y : " << p1.y << " axe.y : " << v2.y << endl;
	cout << "point.z : " << p1.z << " axe.z : " << v2.z << endl;
	cout << endl;
	*/
	return (p1.x*v2.x + p1.y*v2.y + p1.z*v2.z);
}

glm::vec3 vecSub(Point v1, Point v2){
	glm::vec3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	
	return result;
}

/** MIN/MAX **/
GLdouble min(GLdouble a, GLdouble b, GLdouble c){
	GLdouble min = a;
	if(b<min) min = b;
	if(c<min) min = c;
	
	return min;
}

GLdouble max(GLdouble a, GLdouble b, GLdouble c){
	GLdouble max = a;
	if(b>max) max = b;
	if(c>max) max = c;
	
	return max;
}

// version avec axe
// pour les boxes
GLdouble getminBoxPoints(Point * boxPoints, glm::vec3 axis){
	GLdouble min = dotProduct(boxPoints[0], axis);
	GLdouble dotprod = 0;
	int i =0;
	for(i = 1; i < 8; ++i){
		dotprod = dotProduct(boxPoints[i], axis);
		if(dotprod < min) min = dotprod;
	}
	
	return min;
}

GLdouble getmaxBoxPoints(Point * boxPoints, glm::vec3 axis){
	GLdouble max = dotProduct(boxPoints[0], axis);
	GLdouble dotprod = 0;
	int i =0;
	for(i = 1; i < 8; ++i){
		dotprod = dotProduct(boxPoints[i], axis);
		if(dotprod > max) max = dotprod;
	}
	
	return max;
}

// pour les triangles
GLdouble getminTriPoints(Point * triPoints, glm::vec3 axis){
	GLdouble min = dotProduct(triPoints[0], axis);
	GLdouble dotprod = 0;
	int i =0;
	for(i = 1; i < 3; ++i){
		dotprod = dotProduct(triPoints[i], axis);
		if(dotprod < min) min = dotprod;
	}
	
	return min;
}

GLdouble getmaxTriPoints(Point * triPoints, glm::vec3 axis){
	GLdouble max = dotProduct(triPoints[0], axis);
	GLdouble dotprod = 0;
	int i =0;
	for(i = 1; i < 3; ++i){
		dotprod = dotProduct(triPoints[i], axis);
		if(dotprod > max) max = dotprod;
	}
	
	return max;
}


/************************/
/******INTERSECTION******/
/************************/

bool minmaxTest(Point * boxPoints, Point * triPoints, glm::vec3 axis){
	if(getminBoxPoints(boxPoints, axis) > getmaxTriPoints(triPoints, axis)){
		return false;
	}
	if(getmaxBoxPoints(boxPoints, axis) < getminTriPoints(triPoints, axis)){
		return false;
	}

	return true;
}



bool aabbTriboxOverlapTest(Cube testedCube, Face testedFace){
	Point * boxPoints = createBoxPoints(testedCube);
	Point * triPoints = createTriPoints(testedFace);
	
	glm::vec3 xAxis(1.,0.,0.);
	if(!minmaxTest(boxPoints, triPoints, xAxis)){
		return false;
	}
	
	glm::vec3 yAxis(0.,1.,0.);
	if(!minmaxTest(boxPoints, triPoints, yAxis)){
		return false;
	}
	
	glm::vec3 zAxis(0.,0.,1.);
	if(!minmaxTest(boxPoints, triPoints, zAxis)){
		return false;
	}
	
	// edges du triangle :
	glm::vec3 edge1 = vecSub(triPoints[1], triPoints[0]);
	glm::vec3 edge2 = vecSub(triPoints[2], triPoints[1]);
	glm::vec3 edge3 = vecSub(triPoints[0], triPoints[2]);
	
	glm::vec3 normal = crossProduct(edge1, edge2);
	
	if(!minmaxTest(boxPoints, triPoints, normal)){
		return false;
	}
	
	// 9 edges cross products
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, xAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, yAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, zAxis))){ return false;}
	
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, xAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, yAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, zAxis))){ return false;}
	
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, xAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, yAxis))){ return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge3, zAxis))){ return false;}
	
	return true;
}


static const Uint32 MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;
static const size_t GRID_3D_SIZE = 2;

/*************************************/
/*             MAIN                  */
/*************************************/

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
	Vertex* tabV = new Vertex[nbVertice]; //on crée le tableau 
	for(int n=0;n<nbVertice;++n){
		fread(&(tabV[n].pos.x), sizeof(GLdouble), 1, fichier);
		fread(&(tabV[n].pos.z), sizeof(GLdouble), 1, fichier);
		fread(&(tabV[n].pos.y), sizeof(GLdouble), 1, fichier); //on remplit le tableau
	}
	
	
	//lecture des index des points qui constituent chaque face (idem)
	Face* tabF = new Face[nbFace];
	GLuint vertexCoordsOffset[3];
	for(int n=0;n<nbFace;++n){
		fread(vertexCoordsOffset, 3*sizeof(GLuint), 1, fichier);
		tabF[n].s1 = tabV + vertexCoordsOffset[0] -1;
		tabF[n].s2 = tabV + vertexCoordsOffset[1] -1;
		tabF[n].s3 = tabV + vertexCoordsOffset[2] -1;
	}

	fclose(fichier);
	
	//CREATION DE LA GRILLE 3D DE VOXELS
	int nbSub = -1;
	
	cout << endl <<" > Entrer un nombre de subdivisions : ";
	cin >>nbSub;
	
	while(!(nbSub) || (nbSub<=0)){
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); //efface la mauvaise ligne du buffer
		cout << endl <<" > Entrer un nombre de subdivisions : ";
		cin >> nbSub;
	}
		
	size_t const tailleTabVoxel = nbSub*nbSub*nbSub;
	int* tabVoxel = new int[tailleTabVoxel];

	for(size_t i = 0 ; i<tailleTabVoxel ; ++i){
		tabVoxel[i] = 0;
	}
	
	double cubeSize = GRID_3D_SIZE/(double)nbSub;
	double halfCubeSize = cubeSize/2;
	
	//TESTS DE TOUTES LES INTERSECTIONS
	int nbIntersectionMax = 0;
	
	//Pour chaque cube
	for(int k=0;k<nbSub;++k){
		for(int j=0;j<nbSub;++j){
			for(int i=0;i<nbSub;++i){
				int currentVoxel = i + nbSub*j + nbSub*nbSub*k;
				double posX =  i*cubeSize -1;
				double posY = -j*cubeSize +1;
				double posZ = -k*cubeSize +1;
				Cube currentCube = createCube(posX-halfCubeSize,posX+halfCubeSize,posY+halfCubeSize,posY-halfCubeSize,posZ-halfCubeSize,posZ+halfCubeSize);
				
				//Pour chaque face
				for(int n=0;n<nbFace;++n){
					if(aabbTriboxOverlapTest(currentCube, tabF[n])){
						tabVoxel[currentVoxel]++;
					}
				}
				if(tabVoxel[currentVoxel] > nbIntersectionMax){
					nbIntersectionMax = tabVoxel[currentVoxel];
				}
			}
		} 
	}
	
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
	
	/* *********************************** */
	/* ****** CREATION DES FORMES ******** */
	/* *********************************** */
	
	// CREATION DU CUBE 
	Cube aCube = createCube(-0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f);
	
	GLdouble cubeVertices[] = {//
		aCube.left, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.near,
		aCube.left, aCube.top, aCube.near,

		aCube.right, aCube.bottom, aCube.near,
		aCube.right, aCube.top, aCube.near,
		aCube.left, aCube.top, aCube.near,

		//
		aCube.right, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.far,
		aCube.right, aCube.top, aCube.far,

		aCube.right, aCube.bottom, aCube.near,
		aCube.right, aCube.top, aCube.far,
		aCube.right, aCube.top, aCube.near,

		//
		aCube.left, aCube.top, aCube.near,
		aCube.right, aCube.top, aCube.near,
		aCube.right, aCube.top, aCube.far,

		aCube.left, aCube.top, aCube.near,
		aCube.right, aCube.top, aCube.far,
		aCube.left, aCube.top, aCube.far,

		////
		aCube.left, aCube.bottom, aCube.far,
		aCube.right, aCube.bottom, aCube.far,
		aCube.left, aCube.top, aCube.far,

		aCube.right, aCube.bottom, aCube.far,
		aCube.right, aCube.top, aCube.far,
		aCube.left, aCube.top, aCube.far,

		//
		aCube.left, aCube.bottom, aCube.near,
		aCube.left, aCube.bottom, aCube.far,
		aCube.left, aCube.top, aCube.far,

		aCube.left, aCube.bottom, aCube.near,
		aCube.left, aCube.top, aCube.far,
		aCube.left, aCube.top, aCube.near,

		//
		aCube.left, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.far,

		aCube.left, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.far,
		aCube.left, aCube.bottom, aCube.far
	};
	
	/* ******************************** */
	/* 		Creation des VBO, VAO 		*/
	/* ******************************** */
	
	GLuint cubeVBO = 0;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint cubeVAO = 0;
	glGenVertexArrays(1, &cubeVAO);  
	glBindVertexArray(cubeVAO);  
		glEnableVertexAttribArray(POSITION_LOCATION);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glVertexAttribPointer(POSITION_LOCATION, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	


	// Creation des Shaders
	GLuint program = imac2gl3::loadProgram("shaders/basic.vs.glsl", "shaders/basic.fs.glsl");
	if(!program){
		glDeleteBuffers(1, &cubeVBO);
		glDeleteVertexArrays(1, &cubeVAO);
		delete[] tabV;
		delete[] tabF;
		delete[] tabVoxel;
		return (EXIT_FAILURE);
	}
	glUseProgram(program);

	// Creation des Matrices
	GLint MVPLocation = glGetUniformLocation(program, "uMVPMatrix");

	glm::mat4 P = glm::perspective(90.f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.f);
	glm::mat4 V = glm::lookAt(glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f,1.f,0.f));
	glm::mat4 VP = P*V;
	
	// Recuperation des variables uniformes
	GLint NbIntersectionLocation = glGetUniformLocation(program, "uNbIntersection");
	
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
		
		// Affichage de la grille
		for(int k=0;k<nbSub;++k){
			for(int j=0;j<nbSub;++j){
				for(int i=0;i<nbSub;++i){
						int currentNbIntersection = tabVoxel[k*nbSub*nbSub + j*nbSub + i];
						if(currentNbIntersection != 0){
							glm::mat4 aCubeMVP = glm::translate(MVP, glm::vec3(i*cubeSize-(GRID_3D_SIZE-cubeSize)/2, -(j*cubeSize-(GRID_3D_SIZE-cubeSize)/2), -(k*cubeSize-(GRID_3D_SIZE-cubeSize)/2))); //PLACEMENT OF EACH GRID CUBE
							aCubeMVP = glm::scale(aCubeMVP, glm::vec3(cubeSize)); // RE-SCALE EACH GRID CUBE
							glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(aCubeMVP));
						
							glUniform2i(NbIntersectionLocation, currentNbIntersection, nbIntersectionMax);
						
							glBindVertexArray(cubeVAO);
								glDrawArrays(GL_TRIANGLES, 0, aCube.nbVertices);
							glBindVertexArray(0);
						}
				}
			}
		}

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
							offsetViewZ += 0.3;
						break;
						
						case SDL_BUTTON_WHEELDOWN:
							offsetViewZ -= 0.3;
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
			offsetViewX += 0.05;
		}
		
		if(isArrowKeyRightPressed){
			offsetViewX -= 0.05;
		}
		
		if(isArrowKeyUpPressed){
			offsetViewY -= 0.05;
		}
		
		if(isArrowKeyDownPressed){
			offsetViewY += 0.05;
		}
		
		// Gestion compteur
		end = SDL_GetTicks();
		ellapsedTime = end - start;
		if(ellapsedTime < MIN_LOOP_TIME){
			SDL_Delay(MIN_LOOP_TIME - ellapsedTime);
		}
	}

	// Destruction des ressources OpenGL
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &cubeVAO);
	delete[] tabV;
	delete[] tabF;
	delete[] tabVoxel;

	SDL_Quit();

	return EXIT_SUCCESS;
}
