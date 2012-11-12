#include <iostream>
#include <cstdlib>
#include <cmath>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <stdint.h>

#include "types.hpp"

static const size_t GRID_3D_SIZE = 2;

/******************************************/
/*          FUNCTIONS                     */
/******************************************/

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
	fichier = fopen("terrain_data/page_1.data", "rb");
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
	
	//VOXELS ARRAY CREATION	
	uint32_t nbSub = 8; // <----------------------------------------------------------------------///****************************** TO REPLACE
	size_t const tailleTabVoxel = nbSub*nbSub*nbSub;
	uint32_t* tabVoxel = new uint32_t[tailleTabVoxel];
	
	double cubeSize = GRID_3D_SIZE/(double)nbSub;
	double halfCubeSize = cubeSize/2;
	
	//INTERSECTION PROCESSING
	uint32_t nbIntersectionMax = 0;
	
	//For each cube
	//#pragma openmp parallel_for
	for(uint16_t k=0;k<nbSub;++k){
		for(uint16_t j=0;j<nbSub;++j){
			for(uint16_t i=0;i<nbSub;++i){
				uint32_t currentVoxel = i + nbSub*j + nbSub*nbSub*k;
				double posX =  i*cubeSize -1;
				double posY = -j*cubeSize +1;
				double posZ = -k*cubeSize +1;
				Cube currentCube = createCube(posX-halfCubeSize,posX+halfCubeSize,posY+halfCubeSize,posY-halfCubeSize,posZ-halfCubeSize,posZ+halfCubeSize);
				
				//For each Face
				for(uint32_t n=0;n<nbFace;++n){
					if(aabbTriboxOverlapTest(currentCube, tabF[n], altMin, altMax)){
						tabVoxel[currentVoxel]++;
					}
				}
				if(tabVoxel[currentVoxel] > nbIntersectionMax){
					nbIntersectionMax = tabVoxel[currentVoxel];
				}
			}
		} 
	}
	
	//ECRITURE DANS LE FICHIER DE VOXEL |||| A FAIRE
	
	
	delete[] positionsData;
	delete[] facesData;

	delete[] tabV;
	delete[] tabF;
	delete[] tabVoxel;

	return EXIT_SUCCESS;
}
