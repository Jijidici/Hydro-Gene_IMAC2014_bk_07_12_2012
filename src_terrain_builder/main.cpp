#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>

//ENUMS
//differentes scenes
enum SceneType{
	HORIZ_PLAN = 0,
	VERTI_PLAN = 1
};

/* writing the vertices and faces file */
bool writeVerticesAndFacesFile(uint32_t nbV, uint32_t nbF, double* v, uint32_t* f, std::string filename){
	if(nbV == 0 || nbF == 0 || v == NULL || f == NULL){
		throw std::invalid_argument("null value");
	}

	std::string output = filename + "_1.data";
	size_t test_file = 0;

	FILE* file = NULL;
	file = fopen(output.c_str(), "wb");
	if(NULL == file){
		throw std::runtime_error("unable to open the file");
	}

	test_file = fwrite(&nbV, sizeof(uint32_t), 1, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }
	test_file = fwrite(&nbF, sizeof(uint32_t), 1, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }
	test_file = fwrite(v, sizeof(double), 3*nbV, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }
	test_file = fwrite(f, sizeof(uint32_t), 3*nbF, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }

	fclose(file);

	std::cout<<"-> Vertices and Faces file generated : "<<output<<std::endl;

	return true;
}

/* writing the normals file */
bool writeNormalFile(uint32_t nbV, uint32_t nbF, double* normalV, double* normalF, std::string filename){
	if(nbV == 0 || nbF == 0 || normalV == NULL || normalF == NULL){
		throw std::invalid_argument("null value");
	}

	std::string output = filename + "_2.data";
	size_t test_file = 0;

	FILE* file = NULL;
	file = fopen(output.c_str(), "wb");
	if(NULL == file){
		throw std::runtime_error("unable to open the file");
	}

	test_file = fwrite(normalV, sizeof(double), 3*nbV, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }
	test_file = fwrite(normalF, sizeof(double), 3*nbF, file);
	if(test_file <= 0){ throw std::runtime_error("unable to write in the file"); }

	fclose(file);

	std::cout<<"-> Normal file generated : "<<output<<std::endl;

	return true;
}

/*****************************************************************
							MAIN
*******************************************************************/

int main(int argc, char** argv){
	bool displayHelp = true;
	SceneType input = HORIZ_PLAN;
	std::string outputFileName = "terrain_data/page";

	/* Reading the input arguments */
	/* Choose the terrain */
	if(argc > 1){
		std::string arg1 = argv[1];
		if(arg1 == "horp"){
			input = HORIZ_PLAN;
			displayHelp = false;
		}else if(arg1 == "verp"){
			input = VERTI_PLAN;
			displayHelp = false;
		}
	}

	/* Output files's name */
	if(argc > 2){
		std::string arg2 = argv[2];
		outputFileName = "terrain_data/" + arg2;
	}

	/* Display the help if there is no input arguments or they are incorrects */
	if(displayHelp){
		std::cout<<"//// ARGUMENTS PROBLEM ////"<<std::endl;
		std::cout<<"-> You don't put argument or put a wrong argument to choose the terrain will be generate."<<std::endl;
		std::cout<<"-> Syntaxe : ./bin/hg_terrain_builder <scene> <output_filename>"<<std::endl;
		std::cout<<std::endl;
		std::cout<<"\tType of <scene> :"<<std::endl;
		std::cout<<"\t\t - horp : single horizontal plan"<<std::endl;
		std::cout<<"\t\t - verp : single vertical plan"<<std::endl;
		std::cout<<std::endl;
		return (EXIT_FAILURE);
	}

	/* GENERATOR */
	uint32_t nbVertices;
	uint32_t nbFaces;
	double* v = NULL;
	uint32_t* f = NULL;
	double* nV = NULL;
	double* nF = NULL;

	switch(input){
		case HORIZ_PLAN:
			std::cout<<"Generating 'horp' terrain :"<<std::endl;
			nbVertices = 4;
			nbFaces = 2;

			v = new double[3*nbVertices];
			v[0] = -0.9; v[1] = 0.9; v[2] = 0.1;		//1
			v[3] = 0.9; v[4] = 0.9; v[5] = 0.1;		//2
			v[6] = 0.9; v[7] = -0.9; v[8] = 0.1;		//3
			v[9] = -0.9; v[10] = -0.9; v[11] = 0.1;	//4

			f = new uint32_t[3*nbFaces];
			f[0] = 1; f[1] = 2; f[2] = 3;
			f[3] = 3; f[4] = 4; f[5] = 1;

			writeVerticesAndFacesFile(nbVertices, nbFaces, v, f, outputFileName);

			nV = new double[3*nbVertices];
			nV[0] = 0.; nV[1] = 0.; nV[2] = 1.;		//1
			nV[3] = 0.; nV[4] = 0.; nV[5] = 1.;		//2
			nV[6] = 0.; nV[7] = 0.; nV[8] = 1.;		//3
			nV[9] = 0.; nV[10] = 0.; nV[11] = 1.;	//4

			nF = new double[3*nbFaces];
			nF[0] = 0.; nF[1] = 0.; nF[2] = 1.;
			nF[3] = 0.; nF[4] = 0.; nF[5] = 1.;	

			writeNormalFile(nbVertices, nbFaces, nV, nF, outputFileName);

		break;

		case VERTI_PLAN:
			std::cout<<"YUUUPER!"<<std::endl;
		break;

		default:
		break;
	}

	/* Free the resources*/
	if(v != NULL){ delete[] v; }
	if(f != NULL){ delete[] f; }
	if(nV != NULL){ delete[] nV; }
	if(nF != NULL){ delete[] nF; }

	std::cout<<"[--FINISHED--]"<<std::endl;

	return (EXIT_SUCCESS);
}