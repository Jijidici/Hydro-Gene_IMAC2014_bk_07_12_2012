#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imac2gl3/shader_tools.hpp"
#include "types.hpp"


#define FRAME_RATE 60

static const Uint32 MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;
static const size_t INTERSECTION_LOCATION = 1;
static const size_t VOXPOSITION_LOCATION = 2;
static const size_t GRID_3D_SIZE = 2;

uint32_t reduceTab(uint32_t nbSub, uint32_t *tabVoxel){

	uint32_t nbIntersectionMax = 0;
	//addition des cases 2 à 2 par ligne
	uint32_t index=0;
	uint32_t* newTab = new uint32_t[4*nbSub*nbSub*nbSub];
	for(uint32_t i=0;i<8*nbSub*nbSub*nbSub;i=i+2){
		newTab[index] = tabVoxel[i]+tabVoxel[i+1];
		index++;
	}
	//addition des cases 2 à 2 par colonne
	uint32_t index2=0;
	uint32_t tailleNewTab2 = 2*nbSub*nbSub*nbSub;
	uint32_t* newTab2 = new uint32_t[tailleNewTab2];
	for(uint32_t j=0;j<4*nbSub*nbSub;j=j+2){
		for(uint32_t i=j*nbSub;i<j*nbSub+nbSub;++i){
			newTab2[index2] = newTab[i]+newTab[i+nbSub];
			index2++;
		}
	}
	delete[] newTab;
	//addition des cases 2 à 2 en profondeur	
	uint32_t index3=0;
	uint32_t tailleNewTab3 = nbSub*nbSub*nbSub;
	uint32_t* newTab3 = new uint32_t[tailleNewTab3];
	for(uint32_t j=0;j<2*nbSub;j=j+2){
		for(uint32_t i=j*nbSub*nbSub;i<j*nbSub*nbSub+nbSub*nbSub;++i){
			newTab3[index3] = newTab2[i]+newTab2[i+nbSub*nbSub];
			index3++;
		}
	}
	delete[] newTab2;
	//on change tabVoxel
	for(uint32_t i=0;i<nbSub*nbSub*nbSub;++i){
		tabVoxel[i] = newTab3[i];
		if(tabVoxel[i] > nbIntersectionMax) nbIntersectionMax = tabVoxel[i];
	}
	delete[] newTab3;
	for(uint32_t i=nbSub*nbSub*nbSub; i<8*nbSub*nbSub*nbSub; ++i){
		tabVoxel[i] = 0;
	}
	return nbIntersectionMax;
}

uint32_t increaseTab(uint32_t nbSub, uint32_t *tabVoxel, uint32_t nbSubMax, uint32_t *tabVoxelMax, uint32_t constNbIntersectionMax){
	
	uint32_t nbIntersectionMax = constNbIntersectionMax;
	
	for(uint32_t i=0;i<nbSubMax*nbSubMax*nbSubMax;++i){
		tabVoxel[i] = tabVoxelMax[i];
	}
	
	if(nbSub != nbSubMax){
		while(nbSubMax>nbSub){
			nbSubMax /= 2;
			nbIntersectionMax = reduceTab(nbSubMax,tabVoxel);
		}
	}
	return nbIntersectionMax;
}

int main(int argc, char** argv){

	// OPEN AND READ THE VOXEL-INTERSECTION FILE
	FILE* voxelFile = NULL;
	size_t test_fic = 0;
	voxelFile = fopen("voxels_data/voxel_intersec_1.data", "rb");
	if(NULL == voxelFile){
		std::cout << "[!]-> Unable to load the file voxelFile" << std::endl;
		return EXIT_FAILURE;
	}

	uint32_t nbSubMax = 1;
	
	test_fic = fread(&nbSubMax, sizeof(uint32_t), 1, voxelFile);

	uint32_t lengthTabVoxel = nbSubMax*nbSubMax*nbSubMax;
	uint32_t* tabVoxelMax = new uint32_t[lengthTabVoxel];
	test_fic = fread(tabVoxelMax, lengthTabVoxel*sizeof(uint32_t), 1, voxelFile);
	
	uint32_t nbSub = nbSubMax;
	uint32_t nbSubExpected = nbSub;
	
	uint32_t nbIntersectionMax = 0;
	uint32_t nbIntersectedVoxel = 0;
	uint32_t* tabVoxel = new uint32_t[lengthTabVoxel];
	for(uint32_t i = 0; i<lengthTabVoxel; ++i){
		tabVoxel[i] = tabVoxelMax[i];

		//get the number of intersection maximum
		if(tabVoxel[i]>nbIntersectionMax){
			nbIntersectionMax = tabVoxel[i];
		}

		//get the number of voxel with at least one intersection
		if(tabVoxel[i] != 0){
			nbIntersectedVoxel++;
		}
	}
	uint32_t constNbIntersectionMax = nbIntersectionMax;

	std::cout<<" -> Nb intersected Voxel : "<<nbIntersectedVoxel<<std::endl;

	if(argc > 1){
		if(atoi(argv[1]) <= (int) nbSubMax){
			nbSubExpected = atoi(argv[1]);
			
			uint32_t test = nbSubExpected;
			uint32_t power = 0;
			
			while(test > 1){
				test = test/2;
				++power;
			}
			
			uint32_t nbLow = pow(2,power);
			uint32_t nbUp = pow(2,power+1);
			
			if(nbSubExpected - nbLow < nbUp - nbSubExpected){
				nbSubExpected = nbLow;
			}else{
				nbSubExpected = nbUp;
			}
			
			if(nbSubExpected == 0){
				nbSubExpected = nbSubMax;
				std::cout << "-> ! nbSub = 0, nbSub initialisé à " << nbSubMax << std::endl;
			}else{
				std::cout << "-> Nombre de subdivisions arrondi à la puissance de 2 la plus proche" << std::endl;
			}
		}else{
			std::cout << "précision demandée supérieure au nb de subdivs max" << std::endl;
		}
	}
	
	std::cout << "-> nbSub : " << nbSubExpected << std::endl;
	
	
	fclose(voxelFile);	
	
	while(nbSub > nbSubExpected){
		nbSub /= 2;
		nbIntersectionMax = increaseTab(nbSub, tabVoxel, nbSubMax, tabVoxelMax, constNbIntersectionMax);
	}
	
	double cubeSize = GRID_3D_SIZE/(double)nbSub;

	//create the intersected voxel nbIntersection and voxel position data array
	GLfloat * voxInterPos = new GLfloat[4*nbIntersectedVoxel]; 
	uint32_t idxVoxInterPos = 0;
	for(uint32_t n=0;n<lengthTabVoxel;++n){
		if(tabVoxel[n] != 0){
			//get the number of intersection for this full voxel
			voxInterPos[idxVoxInterPos] = tabVoxel[n];
			//get the position of this voxel
			uint32_t squareNbSub = nbSub*nbSub;
			voxInterPos[idxVoxInterPos + 3] = (n / squareNbSub) * cubeSize;
			uint32_t reste = n % squareNbSub;
			voxInterPos[idxVoxInterPos + 2] = (reste / nbSub) * cubeSize;
			voxInterPos[idxVoxInterPos + 1] = (reste % nbSub) * cubeSize;			
			idxVoxInterPos += 4;
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
		delete[] tabVoxel;
		delete[] tabVoxelMax;
		delete[] voxInterPos;
		return EXIT_FAILURE;
	}
	
	/* *********************************** */
	/* ****** CREATION DES FORMES ******** */
	/* *********************************** */
	
	// CREATION DU CUBE 
	Cube aCube = createCube(-0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f);
	
	GLdouble cubeVertices[] = {
		aCube.left, aCube.bottom, aCube.near,
		aCube.right, aCube.bottom, aCube.near,
		aCube.right, aCube.top, aCube.near,
		aCube.left, aCube.top, aCube.near,
		aCube.left, aCube.bottom, aCube.far,
		aCube.right, aCube.bottom, aCube.far,
		aCube.right, aCube.top, aCube.far,
		aCube.left, aCube.top, aCube.far,
	};

	uint8_t cubeIndices[] = {
		0, 1, 2, 2, 3, 0,
		5, 4, 7, 7, 6, 5,
		1, 5, 6, 6, 2, 1,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		5, 4, 0, 0, 1, 5
	};
	uint8_t cubeIndicesLength = 36;

	/* ******************************** */
	/* 		Creation des VBO, VAO 		*/
	/* ******************************** */
	//Buffer de voxel pour connaitre le nombres d'intersection
	GLuint vipBuffer = 0;
	glGenBuffers(1, &vipBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vipBuffer);
		glBufferData(GL_ARRAY_BUFFER, 4*nbIntersectedVoxel*sizeof(GL_FLOAT), voxInterPos, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint cubeVBO = 0;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint cubeIBO = 0;
	glGenBuffers(1, &cubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLuint cubeVAO = 0;
	glGenVertexArrays(1, &cubeVAO);  
	glBindVertexArray(cubeVAO);  
		glEnableVertexAttribArray(POSITION_LOCATION);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glVertexAttribPointer(POSITION_LOCATION, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(INTERSECTION_LOCATION);
		glEnableVertexAttribArray(VOXPOSITION_LOCATION);
		glBindBuffer(GL_ARRAY_BUFFER, vipBuffer);
			glVertexAttribPointer(INTERSECTION_LOCATION, 1, GL_FLOAT, GL_FALSE, 4*sizeof(uint32_t), NULL);
			glVertexAttribPointer(VOXPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 4*sizeof(uint32_t), (const GLvoid*) sizeof(uint32_t));
			glVertexAttribDivisor(INTERSECTION_LOCATION, 1);
			glVertexAttribDivisor(VOXPOSITION_LOCATION, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Creation des Shaders
	GLuint program = imac2gl3::loadProgram("shaders/basic.vs.glsl", "shaders/basic.fs.glsl");
	if(!program){
		glDeleteBuffers(1, &cubeVBO);
		glDeleteBuffers(1, &cubeIBO);
		glDeleteVertexArrays(1, &cubeVAO);
		delete[] tabVoxel;
		delete[] tabVoxelMax;
		delete[] voxInterPos;
		return (EXIT_FAILURE);
	}
	glUseProgram(program);

	// Creation des Matrices
	GLint MVPLocation = glGetUniformLocation(program, "uMVPMatrix");

	glm::mat4 P = glm::perspective(90.f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.f);
	glm::mat4 V = glm::lookAt(glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f,1.f,0.f));
	glm::mat4 VP = P*V;
	
	// Recuperation des variables uniformes
	GLint NbIntersectionLocation = glGetUniformLocation(program, "uNbIntersectionMax");
	GLint CubeSizeLocation = glGetUniformLocation(program, "uCubeSize");
	glUniform1f(CubeSizeLocation, cubeSize);
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
	bool changeNbSubPlus = false;
	bool changeNbSubMinus = false;
		
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
				
		if(changeNbSubPlus){ //si on a appuyé sur +
			std::cout <<"-> Nombre de subdivisions : "<<nbSub<<std::endl;
			nbIntersectionMax = increaseTab(nbSub,tabVoxel,nbSubMax,tabVoxelMax, constNbIntersectionMax);
			changeNbSubPlus = false;
		}
		
		if(changeNbSubMinus){ //si on a appuyé sur -
			std::cout << " > Nombre de subdivisions : " << nbSub << std::endl;
			nbIntersectionMax = reduceTab(nbSub,tabVoxel);
			changeNbSubMinus = false;
		}

		glm::mat4 MVP = glm::translate(VP, glm::vec3(offsetViewX, offsetViewY, offsetViewZ)); //MOVE WITH ARROWKEYS & ZOOM WITH SCROLL
		MVP = glm::translate(MVP, glm::vec3(0.f, 0.f, -5.f)); //MOVE AWWAY FROM THE CAMERA
		MVP = glm::rotate(MVP, angleViewX + tmpAngleViewX,  glm::vec3(0.f, 1.f, 0.f)); //ROTATE WITH XCOORDS CLIC
		MVP = glm::rotate(MVP, angleViewY + tmpAngleViewY,  glm::vec3(1.f, 0.f, 0.f)); //ROTATE WITH YCOORDS CLIC
		MVP = glm::translate(MVP, glm::vec3((cubeSize-GRID_3D_SIZE)/2, (cubeSize-GRID_3D_SIZE)/2, (cubeSize-GRID_3D_SIZE)/2)); //CENTER THE GRID
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform1i(NbIntersectionLocation, nbIntersectionMax);

		// Affichage de la grille
		glBindVertexArray(cubeVAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);
				glDrawElementsInstanced(GL_TRIANGLES, cubeIndicesLength, GL_UNSIGNED_BYTE, 0, nbIntersectedVoxel);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
						
						case SDLK_KP_PLUS:
							if(nbSub != nbSubMax){
								nbSub *= 2;
								changeNbSubPlus = true;
							}else std::cout << "Nombre de subdivisions maximum atteint." << std::endl;						
						break;
						
						case SDLK_KP_MINUS:
							if(nbSub != 1){
								nbSub /= 2;
								changeNbSubMinus = true;
							}else std::cout << "Nombre de subdivisions minimum atteint." << std::endl;						
						break;
						
						case SDLK_SPACE:
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
	glDeleteBuffers(1, &cubeIBO);
	
	delete[] tabVoxel;
	delete[] tabVoxelMax;
	delete[] voxInterPos;
	
	return (EXIT_SUCCESS);
}
