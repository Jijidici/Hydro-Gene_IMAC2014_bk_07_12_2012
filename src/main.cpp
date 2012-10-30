#include <iostream>
#include <cstdlib>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imac2gl3/shader_tools.hpp"

#define FRAME_RATE 60

using namespace std;

typedef struct{
	GLdouble left;
	GLdouble right;
	GLdouble bottom;
	GLdouble top;
	GLdouble far;
	GLdouble near;
}Cube;

static const Uint32 MIN_LOOP_TIME = 1000/FRAME_RATE;
static const size_t WINDOW_WIDTH = 512, WINDOW_HEIGHT = 512;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;
static const size_t NORMAL_LOCATION = 1;
static const size_t TEXTCOORD_LOCATION = 2;

int main(int argc, char** argv) {
    /********************************************************************
     * INITIALISATION DU PROGRAMME
     ********************************************************************/
    
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
    
    // Creation des ressources OpenGL
    
    /** PLACEZ VOTRE CODE DE CREATION DES VBOS/VAOS/SHADERS/... ICI **/
    
    Cube aCube;
    aCube.left = -0.5;
    aCube.right = 0.5;
    aCube.bottom = -0.5;
    aCube.top = 0.5;
    aCube.far = -0.5;
    aCube.near = 0.5;
    
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
    GLuint nbVertices = 36;
    
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
    
  	 //CREATION DES SHADERS
    GLuint program = imac2gl3::loadProgram("shaders/basic.vs.glsl", "shaders/basic.fs.glsl");
    if(!program){
    	return (EXIT_FAILURE);
    }
    glUseProgram(program);
    
    //MATRIX
    GLint MVPLocation = glGetUniformLocation(program, "uMVPMatrix");
    
    glm::mat4 P = glm::perspective(70.f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 1000.f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f,1.f,0.f));
    glm::mat4 VP = P*V;
    
    
    glEnable(GL_DEPTH_TEST);
    
    // Boucle principale
    bool done = false;
    while(!done) {
    		// Initilisation compteur
    		Uint32 start = 0;
			Uint32 end = 0;
			Uint32 ellapsedTime = 0;
     	  	start = SDL_GetTicks();
    
        // Nettoyage de la fenêtre
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Pre-idle
        glm::mat4 MVP = glm::translate(VP, glm::vec3(1.2f, -0.8f, -4.f));
        
        // Dessin
        
        /** PLACEZ VOTRE CODE DE DESSIN ICI **/
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
        
        glBindVertexArray(cubeVAO);
        	glDrawArrays(GL_TRIANGLES, 0, nbVertices);
        glBindVertexArray(0);
        
        // Mise à jour de l'affichage
        SDL_GL_SwapBuffers();
        
        // Boucle de gestion des évenements
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            // Traitement de l'évenement fermeture de fenêtre
            if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_q) {
                done = true;
                break;
            }
            
            // Traitement des autres évenements:
            
            /** PLACEZ VOTRE CODE DE TRAITEMENT DES EVENTS ICI **/
        }
        
        //idle
        
        	// Gestion compteur
        	end = SDL_GetTicks();
        	ellapsedTime = end - start;
        	if(ellapsedTime < MIN_LOOP_TIME){
				SDL_Delay(MIN_LOOP_TIME - ellapsedTime);
			}
    }
    
    // Destruction des ressources OpenGL
    
    /** PLACEZ VOTRE CODE DE DESTRUCTION DES VBOS/VAOS/SHADERS/... ICI **/
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
