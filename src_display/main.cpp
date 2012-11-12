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
static const size_t GRID_3D_SIZE = 2;

int main(int argc, char** argv){
	
	// LECTURE DU FICHIER DATA DE VOXELS  ||||| A FAIRE 
	//TMP
	uint32_t nbSub = 50;
	uint32_t* tabVoxel = new uint32_t[nbSub*nbSub*nbSub];
	
	uint32_t nbIntersectionMax = 23;
	for(uint32_t n=0;n<nbSub*nbSub*nbSub;++n){
		tabVoxel[n] = n%23;
	} 
	//TMP END
	
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
						break;
						
						case SDLK_KP_MINUS:
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
	
	delete[] tabVoxel;
	
	return (EXIT_SUCCESS);
}
