#include <iostream>
#include <cstdlib>
#include <cmath>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <stdint.h>

#include <omp.h>

#include "types.hpp"

static const size_t GRID_3D_SIZE = 2;

/******************************************/
/*          FUNCTIONS                     */
/******************************************/

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
	//std::cout << "x : " << *p1.x << " y : " << *p1.y << " z : " << *p1.z <<  std::endl;
	
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

// Evaluation des distances des points de l'élément aux axes
// pour les boxes
// minimum
GLdouble getminCube(Cube testedCube, glm::vec3 axis){
	GLdouble min = dotProduct(	createPoint(testedCube.left, testedCube.top, testedCube.near), axis);
	
	GLdouble dotprod = 0;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.top, testedCube.near)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.bottom, testedCube.near)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.bottom, testedCube.near)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.top, testedCube.far)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.top, testedCube.far)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.bottom, testedCube.far)	, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.bottom, testedCube.far)	, axis);
	if(dotprod < min) min = dotprod;
	
	
	return min;
}
// maximum
GLdouble getmaxCube(Cube testedCube, glm::vec3 axis){
	GLdouble max = dotProduct(	createPoint(testedCube.left, testedCube.top, testedCube.near), axis);
	
	GLdouble dotprod = 0;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.top, testedCube.near)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.bottom, testedCube.near)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.bottom, testedCube.near)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.top, testedCube.far)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.top, testedCube.far)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.right, testedCube.bottom, testedCube.far)	, axis);
	if(dotprod > max) max = dotprod;
	
	dotprod = dotProduct(	createPoint(testedCube.left, testedCube.bottom, testedCube.far)	, axis);
	if(dotprod > max) max = dotprod;
	
	
	return max;
}

// pour les triangles
// minimum
GLdouble getminFace(Face testedFace, glm::vec3 axis){
	GLdouble min = dotProduct(testedFace.s1->pos, axis);
	GLdouble dotprod = 0;
	
	dotprod = dotProduct(testedFace.s2->pos, axis);
	if(dotprod < min) min = dotprod;
	
	dotprod = dotProduct(testedFace.s3->pos, axis);
	if(dotprod < min) min = dotprod;
	
	
	return min;
}
// maximum
GLdouble getmaxFace(Face testedFace, glm::vec3 axis){
	GLdouble max = dotProduct(testedFace.s1->pos, axis);
	GLdouble dotprod = 0;
	
	dotprod = dotProduct(testedFace.s2->pos, axis);
	if(dotprod > max) max = dotprod;

	dotprod = dotProduct(testedFace.s3->pos, axis);
	if(dotprod > max) max = dotprod;

	return max;
}

GLdouble getmaxX(Face testedFace){
	GLdouble maxX = *testedFace.s1->pos.x;
	if(*testedFace.s2->pos.x > maxX) maxX = *testedFace.s2->pos.x;
	if(*testedFace.s3->pos.x > maxX) maxX = *testedFace.s3->pos.x;
	
	return maxX;
}

GLdouble getminX(Face testedFace){
	GLdouble minX = *testedFace.s1->pos.x;
	if(*testedFace.s2->pos.x < minX) minX = *testedFace.s2->pos.x;
	if(*testedFace.s3->pos.x < minX) minX = *testedFace.s3->pos.x;
	
	return minX;
}

GLdouble getmaxY(Face testedFace){
	GLdouble maxY = *testedFace.s1->pos.y;
	if(*testedFace.s2->pos.y > maxY) maxY = *testedFace.s2->pos.y;
	if(*testedFace.s3->pos.y > maxY) maxY = *testedFace.s3->pos.y;
	
	return maxY;
}

GLdouble getminY(Face testedFace){
	GLdouble minY = *testedFace.s1->pos.y;
	if(*testedFace.s2->pos.y < minY) minY = *testedFace.s2->pos.y;
	if(*testedFace.s3->pos.y < minY) minY = *testedFace.s3->pos.y;
	
	return minY;
}

GLdouble getmaxZ(Face testedFace){
	GLdouble maxZ = *testedFace.s1->pos.z;
	if(*testedFace.s2->pos.z > maxZ) maxZ = *testedFace.s2->pos.z;
	if(*testedFace.s3->pos.z > maxZ) maxZ = *testedFace.s3->pos.z;
	
	return maxZ;
}

GLdouble getminZ(Face testedFace){
	GLdouble minZ = *testedFace.s1->pos.z;
	if(*testedFace.s2->pos.z < minZ) minZ = *testedFace.s2->pos.z;
	if(*testedFace.s3->pos.z < minZ) minZ = *testedFace.s3->pos.z;
	
	return minZ;
}


/************************/
/******INTERSECTION******/
/************************/
// fonction d'intersection selon un axe
// on passe en paramètre : les points du cube, les points du triangle, l'axe considéré
bool minmaxTest(glm::vec3 axis, Face testedFace, Cube testedCube){
	// si le point du cube le plus proche de l'axe est plus éloigné que le point du triangle le plus loin de l'axe, pas de chevauchement des AABB selon cet axe. (oui, c'est tordu !)
	if(getminCube(testedCube, axis) > getmaxFace(testedFace, axis)){
		return false;
	}
	// inversement : si le point du cube le plus loin de l'axe est plus proche que le point du triangle le plus proche de l'axe, pas de chevauchement des AABB selon cet axe.
	if(getmaxCube(testedCube, axis) < getminFace(testedFace, axis)){
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
	
	// on teste les voxels par rapport aux sommets du triangle
	if(testedCube.left > getmaxX(testedFace)){ return false;}
	if(testedCube.right < getminX(testedFace)){ return false;}
	
	if(testedCube.far > getmaxZ(testedFace)){ return false;}
	if(testedCube.near < getminZ(testedFace)){ return false;}
	
	if(testedCube.top < getmaxY(testedFace)){ return false;}
	if(testedCube.bottom > getminY(testedFace)){ return false;}
	
	// selon les trois axes du repère
	glm::vec3 xAxis(1.,0.,0.);
	if(!minmaxTest(xAxis, testedFace, testedCube)){
		
		return false;
	}
	
	glm::vec3 yAxis(0.,1.,0.);
	if(!minmaxTest(yAxis, testedFace, testedCube)){
		
		return false;
	}
	
	glm::vec3 zAxis(0.,0.,1.);
	if(!minmaxTest(zAxis, testedFace, testedCube)){
		
		return false;
	}
	
	// edges du triangle :
	glm::vec3 edge1 = vecSub(testedFace.s2->pos, testedFace.s1->pos);
	glm::vec3 edge2 = vecSub(testedFace.s3->pos, testedFace.s2->pos);
	glm::vec3 edge3 = vecSub(testedFace.s1->pos, testedFace.s3->pos);
	
	glm::vec3 normal = crossProduct(edge1, edge2);
	
	// selon la normale au triangle
	if(!minmaxTest(normal, testedFace, testedCube)){
		
		return false;
	}
	
	// selon les 9 produits vectoriels (directions du cube)^(edges du triangle)
	if(!minmaxTest(crossProduct(edge1, xAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge1, yAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge1, zAxis), testedFace, testedCube)){  return false;}
	
	if(!minmaxTest(crossProduct(edge2, xAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge2, yAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge2, zAxis), testedFace, testedCube)){  return false;}
	
	if(!minmaxTest(crossProduct(edge1, xAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge2, yAxis), testedFace, testedCube)){  return false;}
	if(!minmaxTest(crossProduct(edge3, zAxis), testedFace, testedCube)){  return false;}
	
	
	
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
	FILE *dataFile = NULL;
	dataFile = fopen("terrain_data/page_1.data", "rb");
	if(NULL == dataFile){
		std::cout << "[!] > Impossible to load the file dataFile" << std::endl;
		return EXIT_FAILURE;
	}

	//lecture du nombre de vertex et de faces, puis affichage ds la console
	uint32_t nbVertice = 0, nbFace = 0;	
	fread(&nbVertice, sizeof(nbVertice), 1, dataFile);
	fread(&nbFace, sizeof(nbFace), 1, dataFile);
	std::cout << "Number of vertices : " << nbVertice << std::endl;
	std::cout << "Number of faces : " << nbFace << std::endl;
	
	
	// altitudes min et max de la carte
	GLdouble altMin = 0.0;
	GLdouble altMax = 0.015;
	
	GLdouble * positionsData = new GLdouble[3*nbVertice];
	fread(positionsData, sizeof(GLdouble), 3*nbVertice, dataFile); // to read the positions of the vertices
	
	uint32_t * facesData = new uint32_t[3*nbFace];
	fread(facesData, sizeof(uint32_t), 3*nbFace, dataFile); // to read the indexes of the vertices which compose each face
	
	fclose(dataFile);

	Vertex * tabV = new Vertex[nbVertice];
	
	for(uint32_t n=0;n<nbVertice;++n){ // to create the vertices tab
		tabV[n].pos.x = &positionsData[3*n];
		tabV[n].pos.z = &positionsData[3*n+1];
		tabV[n].pos.y = &positionsData[3*n+2];
		
		// on récupère les altitudes extremes
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
	
	//valeur par défaut
	uint32_t nbSub = 0; // <----------------------------------------------------------------------///****************************** TO REPLACE
	
	if(argc > 1){
		nbSub = atoi(argv[1]);
	}
	
	uint32_t test = nbSub;
	uint32_t power = 0;
	
	while(test > 1){
		test = test/2;
		++power;
	}
	
	uint32_t nbLow = pow(2,power);
	uint32_t nbUp = pow(2,power+1);
	
	if(nbSub - nbLow < nbUp - nbSub){
		nbSub = nbLow;
	}else{
		nbSub = nbUp;
	}
	
	if(nbSub == 0){
		nbSub = 16;
		std::cout << "-> ! nbSub = 0, nbSub initialisé à 16" << std::endl;
	}else{
		std::cout << "-> Nombre de subdivisions arrondi à la puissance de 2 la plus proche" << std::endl;
	}
	
	std::cout << "-> nbSub : " << nbSub << std::endl;
	
	size_t const tailleTabVoxel = nbSub*nbSub*nbSub;
	uint32_t* tabVoxel = new uint32_t[tailleTabVoxel];
	
	for(uint32_t n=0;n<tailleTabVoxel;++n){
		tabVoxel[n]=0;
	}

	double cubeSize = GRID_3D_SIZE/(double)nbSub;
	double halfCubeSize = cubeSize/2;
	
	//INTERSECTION PROCESSING
	
	//For each cube
	#pragma omp parallel for
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
						tabVoxel[currentVoxel] += 1;
					}
				}
			}
		} 
	}
	
	//WRITTING THE VOXEL-INTERSECTION FILE
	FILE* voxelFile = NULL;
	voxelFile = fopen("voxels_data/voxel_intersec_1.data", "wb");
	if(NULL == voxelFile){
		std::cout << "[!] > Impossible to load the file voxelFile" << std::endl;
		return EXIT_FAILURE;
	}

	fwrite(&nbSub, sizeof(uint32_t), 1, voxelFile);
	fwrite(tabVoxel, tailleTabVoxel*sizeof(uint32_t), 1, voxelFile);

	fclose(voxelFile);
	
	delete[] positionsData;
	delete[] facesData;

	delete[] tabV;
	delete[] tabF;
	delete[] tabVoxel;

	return EXIT_SUCCESS;
}
