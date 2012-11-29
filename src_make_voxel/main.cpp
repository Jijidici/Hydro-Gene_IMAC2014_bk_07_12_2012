#include <iostream>
#include <cstdlib>
#include <cmath>
#include <glm/glm.hpp>
#include <stdint.h>
#include <omp.h>
#include "types.hpp"


static const double HALF_SQRT_3 = 0.866025404 *2.;
static const size_t GRID_3D_SIZE = 2;

/******************************************/
/*          FUNCTIONS                     */
/******************************************/

/** MIN/MAX of a Face**/

double getmaxX(Face testedFace){
	double maxX = testedFace.s1->pos.x;
	if(testedFace.s2->pos.x > maxX) maxX = testedFace.s2->pos.x;
	if(testedFace.s3->pos.x > maxX) maxX = testedFace.s3->pos.x;
	
	return maxX;
}

double getminX(Face testedFace){
	double minX = testedFace.s1->pos.x;
	if(testedFace.s2->pos.x < minX) minX = testedFace.s2->pos.x;
	if(testedFace.s3->pos.x < minX) minX = testedFace.s3->pos.x;
	
	return minX;
}

double getmaxY(Face testedFace){
	double maxY = testedFace.s1->pos.y;
	if(testedFace.s2->pos.y > maxY) maxY = testedFace.s2->pos.y;
	if(testedFace.s3->pos.y > maxY) maxY = testedFace.s3->pos.y;
	
	return maxY;
}

double getminY(Face testedFace){
	double minY = testedFace.s1->pos.y;
	if(testedFace.s2->pos.y < minY) minY = testedFace.s2->pos.y;
	if(testedFace.s3->pos.y < minY) minY = testedFace.s3->pos.y;
	
	return minY;
}

double getmaxZ(Face testedFace){
	double maxZ = testedFace.s1->pos.z;
	if(testedFace.s2->pos.z > maxZ) maxZ = testedFace.s2->pos.z;
	if(testedFace.s3->pos.z > maxZ) maxZ = testedFace.s3->pos.z;
	
	return maxZ;
}

double getminZ(Face testedFace){
	double minZ = testedFace.s1->pos.z;
	if(testedFace.s2->pos.z < minZ) minZ = testedFace.s2->pos.z;
	if(testedFace.s3->pos.z < minZ) minZ = testedFace.s3->pos.z;
	
	return minZ;
}

/* GEOMETRICS */
/* Determine if a point is in front of or behind a Face | >0 = in front of | <0 = behind | ==0 = on */
double relativePositionVertexFace(Face f, glm::dvec3 vx){
	glm::dvec3 referentVector = createVector(f.s1->pos, vx);
	return glm::dot(referentVector, f.normal);
}

double relativePositionVertexFace(Plane p, glm::dvec3 vx){
	glm::dvec3 referentVector = createVector(p.s1.pos, vx);
	return glm::dot(referentVector, p.normal);
}

/************************/
/******INTERSECTION******/
/************************/
/* Calculation of intersection between a vertex of the face and the voxel */
bool processIntersectionVertexVoxel(Vertex* v, Voxel vox, double threshold){
	/* if the center of the voxel is inside a bounding sphere with a radius of threshold, turn it on */
	if(glm::distance(v->pos, vox.c) <= threshold){
		return true;
	}
	return false;
}

/* Calculation of intersection between an edge of the face and the voxel */
bool processIntersectionEdgeVoxel(Vertex* v1, Vertex* v2, Voxel vox, double threshold){
	/* Projection of the voxel center on the edge */
	glm::dvec3 edgeDir = createVector(v1->pos, v2->pos);
	double edgeLength = glm::length(edgeDir);
	/* case where the segment is a point */
	if(edgeLength <= 0){
		return false;
	}

	glm::dvec3 edgeDiff = createVector(v1->pos, vox.c);
	float t = glm::dot(edgeDiff, edgeDir)/glm::dot(edgeDir, edgeDir);
	/* If the projected isn't on the segment */
	if(t<0. || t>edgeLength){
		return false;
	}
	glm::dvec3 voxCProjected = glm::dvec3(v1->pos.x + t*edgeDir.x, v1->pos.y + t*edgeDir.y, v1->pos.z + t*edgeDir.z);

	/* if the center of the voxel is inside a bounding cylinder with a radius of threshold, turn it on */
	if(glm::distance(vox.c, voxCProjected) <= threshold){
		return true;
	}
	return false;
}

/* Calculation of intersections between the main plane and the voxel */
bool processIntersectionMainPlaneVoxel(Face testedFace, Voxel currentVoxel){
	/* Choose the good diagonal to calculate alpha angle */
	double cRelativity = relativePositionVertexFace(testedFace, currentVoxel.c);
	double halfVoxelSize = currentVoxel.size/2.;

	/* to calculate voxel vertices */
	int8_t coef1 = -1;
	int8_t coef2 = -1;
	int8_t coef3 = -1;
	glm::dvec3 v;
	int8_t i;
	for(i=0;i<8;++i){
		v = glm::dvec3(currentVoxel.c.x + coef1*halfVoxelSize, currentVoxel.c.y + coef2*halfVoxelSize, currentVoxel.c.z + coef3*halfVoxelSize);
		/* check if the current vertex is on the other side of the plan that the center */
		if(cRelativity * relativePositionVertexFace(testedFace, v)  >= 0){
			break;
		}
		coef1 *= -1;
		if(coef1 == -1){ 
			coef2 *= -1;
			if(coef2 == -1){ coef3 *= -1; }
		}
	}

	/* if no vertex are on the other side, don't put the cube on */
	if(i == 8){ return false; }

	//construction of the diagonal
	glm::dvec3 diagonal = createVector(v, currentVoxel.c);
	double cosAlpha = glm::dot(testedFace.normal, diagonal) / (glm::length(testedFace.normal)*glm::length(diagonal));

	//calculate the plane ordonate d
	double d = - testedFace.normal.x*testedFace.s1->pos.x - testedFace.normal.y*testedFace.s1->pos.y - testedFace.normal.z*testedFace.s1->pos.z; 

	//real test
	double threshold = currentVoxel.size*HALF_SQRT_3*cosAlpha;
	double testedValue = testedFace.normal.x*currentVoxel.c.x + testedFace.normal.y*currentVoxel.c.y + testedFace.normal.z*currentVoxel.c.z + d;
	if(testedValue <= threshold && testedValue >= -threshold){return true;}
	return false;
}

/* Calculate if the voxel center is in the Ei prism */
bool processIntersectionOtherPlanesVoxel(Face testedFace, Voxel currentVoxel){
	/* Define the three perpendicular planes to the trangle Face passing by each edge */
	Plane e1 = createPlane(testedFace.s1->pos, testedFace.s2->pos, testedFace.s1->pos + testedFace.normal);
	Plane e2 = createPlane(testedFace.s1->pos, testedFace.s3->pos, testedFace.s1->pos + testedFace.normal);
	Plane e3 = createPlane(testedFace.s2->pos, testedFace.s3->pos, testedFace.s2->pos + testedFace.normal);

	/* Test if the center of the voxel is on the same side of the tree plan */
	double cRelativityE1 = relativePositionVertexFace(e1, currentVoxel.c);
	double cRelativityE2 = relativePositionVertexFace(e2, currentVoxel.c);
	double cRelativityE3 = relativePositionVertexFace(e3, currentVoxel.c);

	/* If it's the case, the voxel center is in the Ei prism */
	if((cRelativityE1 <=0 && cRelativityE2 <=0 && cRelativityE3 <=0) ||
	   (cRelativityE1 >=0 && cRelativityE2 >=0 && cRelativityE3 >=0)){
		return true;
	}

	return false;
}

/* Main calculation of the intersection between the face and a voxel */
bool processIntersectionPolygonVoxel(Face testedFace, Voxel currentVoxel){
	/* vertex Bounding sphere radius and edge bounding cylinder radius */
	double Rc = currentVoxel.size * HALF_SQRT_3;

	/* Vertices tests */
	if(processIntersectionVertexVoxel(testedFace.s1, currentVoxel, Rc)){ return true;}
	if(processIntersectionVertexVoxel(testedFace.s2, currentVoxel, Rc)){ return true;}
	if(processIntersectionVertexVoxel(testedFace.s3, currentVoxel, Rc)){ return true;}

	/* Edges tests */
	if(processIntersectionEdgeVoxel(testedFace.s1, testedFace.s2, currentVoxel, Rc)){return true;}
	if(processIntersectionEdgeVoxel(testedFace.s1, testedFace.s3, currentVoxel, Rc)){return true;}
	if(processIntersectionEdgeVoxel(testedFace.s2, testedFace.s3, currentVoxel, Rc)){return true;}

	/* Face test */
	if(processIntersectionMainPlaneVoxel(testedFace, currentVoxel) && processIntersectionOtherPlanesVoxel(testedFace, currentVoxel)){
		return true;
	}

	return false;
}

/*************************************/
/*             MAIN                  */
/*************************************/
int main(int argc, char** argv) {

	/* ************************************************************* */
	/* **************PRE - TRAITEMENT DES VOXELS******************** */
	/* ************************************************************* */
	
	//CHARGEMENT FICHIERS .DATA

	size_t test_fic = 0;
	// FILES 1 - BEGIN
	//on charge le fichier en mode "read binary"
	FILE *dataFile = NULL;
	dataFile = fopen("terrain_data/page_1.data", "rb");
	if(NULL == dataFile){
		std::cout << "[!] > Unable to load the file dataFile" << std::endl;
		return EXIT_FAILURE;
	}

	//lecture du nombre de vertex et de faces, puis affichage ds la console
	uint32_t nbVertice = 0, nbFace = 0;	
	test_fic = fread(&nbVertice, sizeof(nbVertice), 1, dataFile);
	test_fic =fread(&nbFace, sizeof(nbFace), 1, dataFile);
	std::cout << "-> Number of vertices : " << nbVertice << std::endl;
	std::cout << "-> Number of faces : " << nbFace << std::endl;
	
	
	// altitudes min et max de la carte
	double altMin = 0;
	double altMax = 0;
	
	double * positionsData = new double[3*nbVertice];
	test_fic = fread(positionsData, sizeof(double), 3*nbVertice, dataFile); // to read the positions of the vertices
	
	uint32_t * facesData = new uint32_t[3*nbFace];
	test_fic = fread(facesData, sizeof(uint32_t), 3*nbFace, dataFile); // to read the indexes of the vertices which compose each face
	
	fclose(dataFile);
	//FILE 1 -END

	//FILE 2 - BEGIN
	FILE* normalFile = NULL;
	normalFile = fopen("terrain_data/page_2.data", "rb");
	if(NULL == normalFile){
		std::cout << "[!]-> Unable to load the file normalFile" << std::endl;
		return EXIT_FAILURE;
	}

	//moving to the beginning of face normals in the file
	test_fic = fseek(normalFile, nbVertice*3*sizeof(double), SEEK_SET);
	if(test_fic != 0){
		std::cout<<"[!]-> Problem in moving on the normal file"<<std::endl;
		return EXIT_FAILURE;
	}

	double * normalData = new double[3*nbFace];
	test_fic = fread(normalData, sizeof(double), 3*nbFace, normalFile);
	std::cout << "-> Number of face normals : " << test_fic << std::endl;

	fclose(normalFile);
	//FILE 2 - END

	//CONSTRUCTION OF THE DATA STRUCTURES
	Vertex * tabV = new Vertex[nbVertice];
	
	for(uint32_t n=0;n<nbVertice;++n){ // to create the vertices tab
		tabV[n].pos.x = positionsData[3*n];
		tabV[n].pos.z = positionsData[3*n+1];
		tabV[n].pos.y = positionsData[3*n+2];
		
		// on récupère les altitudes extremes
		if(tabV[n].pos.y > altMax){
			altMax = tabV[n].pos.y;
		}else{
			if(tabV[n].pos.y < altMin){
				altMin = tabV[n].pos.y;
			}
		}
	}

	std::cout<<"-> altitude max : "<<altMax<<" - altitude min : "<<altMin<<std::endl;
	
	
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
		tabF[n].normal = glm::vec3(normalData[3*n], normalData[3*n+1], normalData[3*n+2]);
	}
	delete[] normalData;

	//VOXELS ARRAY CREATION
	
	//valeur par défaut
	uint32_t nbSub = 0;
	
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
		std::cout << "-> ! nbSub = 0, Nombre de subdivisions initialisé à 16" << std::endl;
	}else{
		std::cout << "-> Nombre de subdivisions arrondi à la puissance de 2 la plus proche" << std::endl;
	}
	
	std::cout << "-> Nombre de subdivisions : " << nbSub << std::endl;

	size_t const tailleTabVoxel = nbSub*nbSub*nbSub;
	uint32_t* tabVoxel = NULL;
	tabVoxel = new uint32_t[tailleTabVoxel];
	if(NULL == tabVoxel){
		std::cout<<"[!] -> Allocation failure for tabVoxel"<<std::endl;
		return EXIT_FAILURE;
	}
	
	for(uint32_t n=0;n<tailleTabVoxel;++n){
		tabVoxel[n]=0;
	}

	double voxelSize = GRID_3D_SIZE/(double)nbSub;
	
	//INTERSECTION PROCESSING
	
	//For each Face
	#pragma omp parallel for
	for(uint32_t n=0; n<nbFace;++n){

		uint32_t minVoxelX = glm::min(uint32_t(getminX(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);
		uint32_t maxVoxelX = glm::min(uint32_t(getmaxX(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);
		uint32_t minVoxelY = glm::min(uint32_t(getminY(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);
		uint32_t maxVoxelY = glm::min(uint32_t(getmaxY(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);
		uint32_t minVoxelZ = glm::min(uint32_t(getminZ(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);
		uint32_t maxVoxelZ = glm::min(uint32_t(getmaxZ(tabF[n])/voxelSize + nbSub*0.5), nbSub-1);

		//For each cube of the face bounding box
		for(uint32_t k=minVoxelZ; k<=maxVoxelZ; ++k){
			for(uint32_t j=minVoxelY;j<=maxVoxelY; ++j){
				for(uint32_t i=minVoxelX;i<=maxVoxelX;++i){
					Voxel vox = createVoxel(i*voxelSize -1, j*voxelSize -1, k*voxelSize -1, voxelSize);
					if(processIntersectionPolygonVoxel(tabF[n], vox)){
						tabVoxel[i + nbSub*j + nbSub*nbSub*k]++;
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

	test_fic = fwrite(&nbSub, sizeof(uint32_t), 1, voxelFile);
	test_fic = fwrite(tabVoxel, tailleTabVoxel*sizeof(uint32_t), 1, voxelFile);

	fclose(voxelFile);
	
	delete[] positionsData;
	delete[] facesData;

	delete[] tabV;
	delete[] tabF;
	delete[] tabVoxel;

	return EXIT_SUCCESS;
}
