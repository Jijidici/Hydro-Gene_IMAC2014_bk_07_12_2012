#include <iostream>
#include <cstdlib>
#include <cmath>

#include <glm/glm.hpp>

#include <stdint.h>

#include <omp.h>

#include "types.hpp"

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


/************************/
/******INTERSECTION******/
/************************/

bool processIntersectionFaceVoxel(Face testedFace, Voxel& currentVoxel){

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
	size_t test_fic = 0;
	dataFile = fopen("terrain_data/page_1.data", "rb");
	if(NULL == dataFile){
		std::cout << "[!] > Unable to load the file dataFile" << std::endl;
		return EXIT_FAILURE;
	}

	//lecture du nombre de vertex et de faces, puis affichage ds la console
	uint32_t nbVertice = 0, nbFace = 0;	
	test_fic = fread(&nbVertice, sizeof(nbVertice), 1, dataFile);
	test_fic =fread(&nbFace, sizeof(nbFace), 1, dataFile);
	std::cout << "Number of vertices : " << nbVertice << std::endl;
	std::cout << "Number of faces : " << nbFace << std::endl;
	
	
	// altitudes min et max de la carte
	double altMin = 0;
	double altMax = 0;
	
	double * positionsData = new double[3*nbVertice];
	test_fic = fread(positionsData, sizeof(double), 3*nbVertice, dataFile); // to read the positions of the vertices
	
	uint32_t * facesData = new uint32_t[3*nbFace];
	test_fic = fread(facesData, sizeof(uint32_t), 3*nbFace, dataFile); // to read the indexes of the vertices which compose each face
	
	fclose(dataFile);

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

	double voxelSize = GRID_3D_SIZE/(double)nbSub;
	
	//INTERSECTION PROCESSING
	
	//For each Face
	//#pragma omp parallel for
	for(uint32_t n=0; n<nbFace;++n){
		uint32_t minVoxelX =  (getminX(tabF[n]) - voxelSize + 1.)/voxelSize;
		uint32_t maxVoxelX =  (getmaxX(tabF[n]) + voxelSize + 1.)/voxelSize;
		uint32_t minVoxelY =  (getminY(tabF[n]) - voxelSize + 1.)/voxelSize;
		uint32_t maxVoxelY =  (getmaxY(tabF[n]) + voxelSize + 1.)/voxelSize;
		uint32_t minVoxelZ =  (getminZ(tabF[n]) - voxelSize + 1.)/voxelSize;
		uint32_t maxVoxelZ =  (getmaxZ(tabF[n]) + voxelSize + 1.)/voxelSize;

		//For each cube of the face bounding box
		for(uint32_t k=minVoxelZ; k<=maxVoxelZ; ++k){
			for(uint32_t j=minVoxelY;j<=maxVoxelY; ++j){
				for(uint32_t i=minVoxelX;i<=maxVoxelX;++i){
					Voxel vox = createVoxel(i*voxelSize -1, j*voxelSize -1, k*voxelSize -1, voxelSize);
					if(processIntersectionFaceVoxel(tabF[n], vox)){
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
