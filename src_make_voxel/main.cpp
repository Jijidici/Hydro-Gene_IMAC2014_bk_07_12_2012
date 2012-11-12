#include <iostream>
#include <cstdlib>
#include <cmath>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>

#include "imac2gl3/shader_tools.hpp"
#include "types.hpp"

#define FRAME_RATE 60

static const Uint32 MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;
static const size_t GRID_3D_SIZE = 2;

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
Point * createBoxPoints(Cube& cube){
	Point * boxPoints = new Point[8];
	Point LTN; LTN.x = &(cube.left); 	LTN.y = &(cube.top); 		LTN.z = &(cube.near);
	Point RTN; RTN.x = &(cube.right); 	RTN.y = &(cube.top); 		RTN.z = &(cube.near);
	Point RBN; RBN.x = &(cube.right);	RBN.y = &(cube.bottom);		RBN.z = &(cube.near);
	Point LBN; LBN.x = &(cube.left); 	LBN.y = &(cube.bottom); 	LBN.z = &(cube.near);
	Point LTF; LTF.x = &(cube.left); 	LTF.y = &(cube.top);		LTF.z = &(cube.far);
	Point RTF; RTF.x = &(cube.right);	RTF.y = &(cube.top); 		RTF.z = &(cube.far);
	Point RBF; RBF.x = &(cube.right); 	RBF.y = &(cube.bottom); 	RBF.z = &(cube.far);
	Point LBF; LBF.x = &(cube.left); 	LBF.y = &(cube.bottom); 	LBF.z = &(cube.far);
	
	boxPoints[0] = LTN;
	boxPoints[1] = RTN;
	boxPoints[2] = RBN;
	boxPoints[3] = LBN;
	boxPoints[4] = LTF;
	boxPoints[5] = RTF;
	boxPoints[6] = RBF;
	boxPoints[7] = LBF;
	
	//std::cout << "test : " << *boxPoints[0].x << std::endl;
	
	return boxPoints;
}

Face createFace(Vertex* inS1, Vertex* inS2, Vertex* inS3){
	Face newFace;
	newFace.s1 = inS1;
	newFace.s2 = inS2;
	newFace.s3 = inS3;
	
	return newFace;
}

// Création du tableau des points du triangle envoyé en paramètre à la fonction de test d'intersection AABB
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


/** OPERATIONS **/
// produit vectoriel
glm::vec3 crossProduct(glm::vec3 v1, glm::vec3 v2){
	glm::vec3 result;
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;
	
	return result;
}

// produit scalaire vecteur*vecteur
GLdouble dotProduct(glm::vec3 v1, glm::vec3 v2){
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// produit scalaire point*vecteur
GLdouble dotProduct(Point p1, glm::vec3 v2){
	GLdouble p1X = *(p1.x); GLdouble p1Y = *(p1.y); GLdouble p1Z = *(p1.z);
	GLdouble v2X = v2.x; GLdouble v2Y = v2.y; GLdouble v2Z = v2.z;
	
	return (p1X*v2X + p1Y*v2Y + p1Z*v2Z);
}

// création d'un vecteur à partir de 2 points
glm::vec3 vecSub(Point p1, Point p2){
	glm::vec3 result;
	result.x = p1.x - p2.x;
	result.y = p1.y - p2.y;
	result.z = p1.z - p2.z;
	
	return result;
}

/** MIN/MAX **/
/*
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
*/

// Evaluation des distances des points de l'élément aux axes
// pour les boxes
// minimum
GLdouble getminBoxPoints(Point * boxPoints, glm::vec3 axis){
	GLdouble min = dotProduct(boxPoints[0], axis);
	GLdouble dotprod = 0;
	uint8_t i =0;
	for(i = 1; i < 8; ++i){
		dotprod = dotProduct(boxPoints[i], axis);
		if(dotprod < min) min = dotprod;
	}
	
	return min;
}
// maximum
GLdouble getmaxBoxPoints(Point * boxPoints, glm::vec3 axis){
	GLdouble max = dotProduct(boxPoints[0], axis);
	GLdouble dotprod = 0;
	uint8_t i =0;
	for(i = 1; i < 8; ++i){
		dotprod = dotProduct(boxPoints[i], axis);
		if(dotprod > max) max = dotprod;
	}

	return max;
}

// pour les triangles
// minimum
GLdouble getminTriPoints(Point * triPoints, glm::vec3 axis){
	GLdouble min = dotProduct(triPoints[0], axis);
	GLdouble dotprod = 0;
	uint8_t i =0;
	for(i = 1; i < 3; ++i){
		dotprod = dotProduct(triPoints[i], axis);
		if(dotprod < min) min = dotprod;
	}
	
	return min;
}
// maximum
GLdouble getmaxTriPoints(Point * triPoints, glm::vec3 axis){
	GLdouble max = dotProduct(triPoints[0], axis);
	GLdouble dotprod = 0;
	uint8_t i =0;
	for(i = 1; i < 3; ++i){
		dotprod = dotProduct(triPoints[i], axis);
		if(dotprod > max) max = dotprod;
	}
	
	return max;
}


/************************/
/******INTERSECTION******/
/************************/
// fonction d'intersection selon un axe
// on passe en paramètre : les points du cube, les points du triangle, l'axe considéré
bool minmaxTest(Point * boxPoints, Point * triPoints, glm::vec3 axis){
	// si le point du cube le plus proche de l'axe est plus éloigné que le point du triangle le plus loin de l'axe, pas de chevauchement des AABB selon cet axe. (oui, c'est tordu !)
	if(getminBoxPoints(boxPoints, axis) > getmaxTriPoints(triPoints, axis)){
		return false;
	}
	// inversement : si le point du cube le plus loin de l'axe est plus proche que le point du triangle le plus proche de l'axe, pas de chevauchement des AABB selon cet axe.
	if(getmaxBoxPoints(boxPoints, axis) < getminTriPoints(triPoints, axis)){
		return false;
	}
	// sinon : chevauchement des AABB, et on passe au test suivant.
	return true;
}


// fonction principale de test de chevauchement des AABB
bool aabbTriboxOverlapTest(Cube testedCube, Face testedFace, GLdouble altMin, GLdouble altMax){
	// pas besoin de tester les cubes au dessus de l'altitude max, et en dessous de l'altitude min.
	if(testedCube.bottom > altMax){ return false;}
	if(testedCube.top < altMin){ return false;}
	
	Point * boxPoints = createBoxPoints(testedCube);
	Point * triPoints = createTriPoints(testedFace);
	
	// selon les trois axes du repère
	glm::vec3 xAxis(1.,0.,0.);
	if(!minmaxTest(boxPoints, triPoints, xAxis)){
		delete[] boxPoints;
		delete[] triPoints;
		return false;
	}
	
	glm::vec3 yAxis(0.,1.,0.);
	if(!minmaxTest(boxPoints, triPoints, yAxis)){
		delete[] boxPoints;
		delete[] triPoints;
		return false;
	}
	
	glm::vec3 zAxis(0.,0.,1.);
	if(!minmaxTest(boxPoints, triPoints, zAxis)){
		delete[] boxPoints;
		delete[] triPoints;
		return false;
	}
	
	// edges du triangle :
	glm::vec3 edge1 = vecSub(triPoints[1], triPoints[0]);
	glm::vec3 edge2 = vecSub(triPoints[2], triPoints[1]);
	glm::vec3 edge3 = vecSub(triPoints[0], triPoints[2]);
	
	glm::vec3 normal = crossProduct(edge1, edge2);
	
	// selon la normale au triangle
	if(!minmaxTest(boxPoints, triPoints, normal)){
		delete[] boxPoints;
		delete[] triPoints;
		return false;
	}
	
	// selon les 9 produits vectoriels (directions du cube)^(edges du triangle)
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, xAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, yAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, zAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, xAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, yAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, zAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge1, xAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge2, yAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	if(!minmaxTest(boxPoints, triPoints, crossProduct(edge3, zAxis))){ delete[] boxPoints; delete[] triPoints; return false;}
	
	delete[] boxPoints;
	delete[] triPoints;
	
	// si tous les tests passent sans pouvoir trouver un axe selon lequel les deux AABB sont clairement séparées, alors on peut dire selon le SAT que les deux éléments s'intersectent.
	return true;
}


uint32_t* createTabVoxel(uint16_t nbSub){	//on alloue un tableau pr stocker les valeurs des voxels (donc en fonction du nombre de subdivisions)
	
	if(nbSub<=0){
		std::cerr << "Nombre de subdivisions incorrect. Arret du programme." << std::endl;
		exit(0);
	}
	
	size_t const tailleTabVoxel = nbSub*nbSub*nbSub;
	uint32_t* tabVoxel = new uint32_t[tailleTabVoxel];

	for(size_t i = 0 ; i<tailleTabVoxel ; ++i){
		tabVoxel[i] = 0;
	}
	
	return tabVoxel;
	
}

uint32_t gridIntersection(uint32_t* tabVoxel, uint16_t nbSub, uint32_t nbFace, Face* tabF, GLdouble altMin, GLdouble altMax){	//on calcule les intersections et on renvoit le nombre d'intersections max (utile pour mettre la couleur plus loin)

	double cubeSize = GRID_3D_SIZE/(double)nbSub;
	double halfCubeSize = cubeSize/2;
	
	//TESTS DE TOUTES LES INTERSECTIONS
	uint32_t nbIntersectionMax = 0;
	
	//Pour chaque cube
	//#pragma openmp parallel_for
	for(uint16_t k=0;k<nbSub;++k){
		for(uint16_t j=0;j<nbSub;++j){
			for(uint16_t i=0;i<nbSub;++i){
				uint32_t currentVoxel = i + nbSub*j + nbSub*nbSub*k;
				double posX =  i*cubeSize -1;
				double posY = -j*cubeSize +1;
				double posZ = -k*cubeSize +1;
				Cube currentCube = createCube(posX-halfCubeSize,posX+halfCubeSize,posY+halfCubeSize,posY-halfCubeSize,posZ-halfCubeSize,posZ+halfCubeSize);
				
				//Pour chaque face
				for(uint32_t n=0;n<nbFace;++n){
					if(aabbTriboxOverlapTest(currentCube, tabF[n], altMin, altMax)){
						tabVoxel[currentVoxel]++;
					}
				}
				if(tabVoxel[currentVoxel] > nbIntersectionMax){
					nbIntersectionMax = tabVoxel[currentVoxel];
				}
				//std::cout << "test : " << tabVoxel[currentVoxel] << std::endl;
			}
		} 
	}
	
	return nbIntersectionMax;
}



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
	if(NULL == fichier) std::cout << "impossible de charger le fichier" << std::endl;

	//lecture du nombre de vertex et de faces, puis affichage ds la console
	uint32_t nbVertice = 0, nbFace = 0;	
	fread(&nbVertice, sizeof(nbVertice), 1, fichier);
	fread(&nbFace, sizeof(nbFace), 1, fichier);
	std::cout << "Number of vertices : " << nbVertice << std::endl;
	std::cout << "Number of faces : " << nbFace << std::endl;
	
	
	// altitudes min et max de la carte
	GLdouble altMin = 0.0;
	GLdouble altMax = 0.015;
	
	GLdouble * positionsData = new GLdouble[3*nbVertice];
	fread(positionsData, sizeof(GLdouble), 3*nbVertice, fichier); // to read the positions of the vertices
	
	uint32_t * facesData = new uint32_t[3*nbFace];
	fread(facesData, sizeof(uint32_t), 3*nbFace, fichier); // to read the indexes of the vertices which compose each face
	
	Vertex * tabV = new Vertex[nbVertice];
	
	for(uint32_t n=0;n<nbVertice;++n){ // to create the vertices tab
		tabV[n].pos.x = &positionsData[3*n];
		tabV[n].pos.z = &positionsData[3*n+1];
		tabV[n].pos.y = &positionsData[3*n+2];
		
		// on récupère les altitudes extrèmes
		if(*(tabV[n].pos.y) > altMax){
			altMax = *(tabV[n].pos.y);
		}else{
			if(*(tabV[n].pos.y) < altMin){
				altMin = *(tabV[n].pos.y);
			}
		}
	}
	/*
	for(size_t n = 0; n < 10; ++n){
		std::cout << endl << "n : " << n << std::endl;
		std::cout << "x : " << *tabV[n].pos.x << " y : " << *tabV[n].pos.y << " z : " << *tabV[n].pos.z << std::endl;
	}
	*/
	std::cout << " -> altitude max : " << altMax << " - altitude min : " << altMin << std::endl;
	
	Face * tabF = new Face[nbFace];
	uint32_t vertexCoordsOffset[3];
	
	// creation of the faces
	for(uint32_t n=0;n<nbFace;++n){
		for(size_t i = 0; i < 3; ++i){
			vertexCoordsOffset[i] = facesData[3*n+i];
		}
		tabF[n].s1 = tabV + vertexCoordsOffset[0] -1;
		tabF[n].s2 = tabV + vertexCoordsOffset[1] -1;
		tabF[n].s3 = tabV + vertexCoordsOffset[2] -1;
	}
	/*
	for(size_t i = 0; i < 10; ++i){
		std::cout << endl << "i : " << i << std::endl;
		std::cout << "s1 x : " << *tabF[i].s1->pos.x << " s1 y : " << *tabF[i].s1->pos.y << " s1 z : " << *tabF[i].s1->pos.z << std::endl;
		std::cout << "s2 x : " << *tabF[i].s2->pos.x << " s2 y : " << *tabF[i].s2->pos.y << " s2 z : " << *tabF[i].s2->pos.z << std::endl;
		std::cout << "s3 x : " << *tabF[i].s3->pos.x << " s3 y : " << *tabF[i].s3->pos.y << " s3 z : " << *tabF[i].s3->pos.z << std::endl;
	}
	*/
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
	uint8_t isArrowKeyUpPressed = 0;
	uint8_t isArrowKeyDownPressed = 0;
	uint8_t isArrowKeyLeftPressed = 0;
	uint8_t isArrowKeyRightPressed = 0;
	uint8_t isLeftClicPressed = 0;
	uint16_t savedClicX = -1;
	uint16_t savedClicY = -1;
	uint32_t *tabVoxel = NULL;
	uint32_t nbSub = 1;
	bool changeNbSub = true;
	uint32_t nbIntersectionMax = 0;
		
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

		if(changeNbSub){ //si on a appuyé sur + ou -
			std::cout<<" > Nombre de subdivisions : " <<nbSub<<std::endl;
			delete[] tabVoxel;
			tabVoxel = createTabVoxel(nbSub);
			nbIntersectionMax = gridIntersection(tabVoxel, nbSub, nbFace, tabF, altMin, altMax);
			changeNbSub = false;
		}
			
		double cubeSize = GRID_3D_SIZE/(double)nbSub;

		glm::mat4 MVP = glm::translate(VP, glm::vec3(offsetViewX, offsetViewY, offsetViewZ)); //MOVE WITH ARROWKEYS & ZOOM WITH SCROLL
		MVP = glm::translate(MVP, glm::vec3(0.f, 0.f, -5.f)); //MOVE AWWAY FROM THE CAMERA
		MVP = glm::rotate(MVP, angleViewX + tmpAngleViewX,  glm::vec3(0.f, 1.f, 0.f)); //ROTATE WITH XCOORDS CLIC
		MVP = glm::rotate(MVP, angleViewY + tmpAngleViewY,  glm::vec3(1.f, 0.f, 0.f)); //ROTATE WITH YCOORDS CLIC
		
		// Affichage de la grille
		for(uint32_t k=0;k<nbSub;++k){
			for(uint32_t j=0;j<nbSub;++j){
				for(uint32_t i=0;i<nbSub;++i){
					uint32_t currentNbIntersection = tabVoxel[k*nbSub*nbSub + j*nbSub + i];
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
		glUniform2i(NbIntersectionLocation, 1, nbIntersectionMax);
		/*
		glBindVertexArray(faceVAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		*/
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
						
						case SDLK_KP_PLUS:
							changeNbSub = true;
							nbSub *= 2;
						break;
						
						case SDLK_KP_MINUS:
							changeNbSub = true;
							nbSub /= 2;
						break;
						
						case SDLK_SPACE:
							changeNbSub = true;
							nbSub = 32;
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

	delete[] positionsData;
	delete[] facesData;

	delete[] tabV;
	delete[] tabF;
	delete[] tabVoxel;

	SDL_Quit();

	return EXIT_SUCCESS;
}
