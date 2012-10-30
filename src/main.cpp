#include <iostream>
#include <cstdlib>
#include <fstream>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imac2gl3/shader_tools.hpp"

using namespace std;

typedef struct{
	double left;
	double right;
	double bottom;
	double top;
	double far;
	double near;
}Cube;

static const size_t WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;
static const size_t POSITION_LOCATION = 0;

int main(int argc, char** argv){
	
	/* ************************************************************* */
	/* **********PRE - TRAITEMENT DES VOXELS************************ */
	/* ************************************************************* */
	
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
	int const sizeTabVertice(nbVertice*3); //on fixe d'abord la taille que le tableau va prendre en fonction du nombre de vertex
	double* tabV = new double[sizeTabVertice]; //on crée le tableau (j'ai voulu l'initialiser mais ça buggait alors tant pis)
	fread(tabV, sizeTabVertice*sizeof(double), 1, fichier); //on remplit le tableau
	
	for (int i =0 ; i < 20 ; ++i) cout << tabV[i] << " "; //mini affichage pour voir si ça fonctionne
	cout << endl;
	//(j'ai testé d'une autre manière pr voir si j'obtenais la même chose, ça marchait)
	
	//lecture des index des points qui constituent chaque face (idem)
	int const sizeTabFace(nbFace*3);
	uint* tabF = new uint[sizeTabFace]; //il a pas reconnu le type uint, je sais pas pourquoi
	fread(tabF, sizeTabFace*sizeof(uint), 1, fichier);
	for (int j =0 ; j < 20 ; ++j) cout << tabF[j] << " ";
	cout << endl;

	fclose(fichier);
	
	/* ************************************************************* */
	/* *************INITIALISATION DE LA SDL************************ */
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
    
    // Creation des ressources OpenGL
    glEnable(GL_DEPTH_TEST);
    
    // Creation des VBO, VAO
    Cube aCube;
    aCube.left = -0.5;
    aCube.right = 0.5;
    aCube.bottom = -0.5;
    aCube.top = 0.5;
    aCube.far = -0.5;
    aCube.near = 0.5;
    
    double cubeVertices[] = {//
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
    		aCube.right, aCube.top, aCube.near,
    		aCube.right, aCube.bottom, aCube.far,
    		
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
    		aCube.left, aCube.top, aCube.near,
    		aCube.left, aCube.bottom, aCube.far,
    		
    		//
    		aCube.left, aCube.bottom, aCube.near,
    		aCube.right, aCube.bottom, aCube.near,
    		aCube.right, aCube.bottom, aCube.far,
    		
    		aCube.left, aCube.bottom, aCube.near,
    		aCube.right, aCube.bottom, aCube.far,
    		aCube.left, aCube.bottom, aCube.far,
    };
    GLuint nbVertices = 36;
    
    GLuint cubeVBO = 0;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    	glBufferData(GL_ARRAY_BUFFER, nbVertices*3*sizeof(GL_DOUBLE), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    GLuint cubeVAO = 0;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    	glEnableVertexAttribArray(POSITION_LOCATION);
    	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    		glVertexAttribPointer(
    			POSITION_LOCATION,
    			3,
    			GL_DOUBLE,
    			GL_FALSE,
    			3*sizeof(GL_DOUBLE),
    			0
    		);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Creation des Shaders
    GLuint program = imac2gl3::loadProgram("shaders/transform.vs.glsl", "shaders/basic.fs.glsl");
    if(!program){
    	glDeleteBuffers(1, &cubeVBO);
   	glDeleteVertexArrays(1, &cubeVAO);
    	return EXIT_FAILURE;
    }
    glUseProgram(program);
    
    //Creation des matrices
	GLuint MVPLocation = glGetUniformLocation(program, "uMVPMatrix");
    
    glm::mat4 P = glm::perspective(120.f, WINDOW_WIDTH / (float) WINDOW_HEIGHT, -0.1f, -1000.f);
    glm::mat4 V = glm::lookAt(glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f,1.f,0.f));
    glm::mat4 MVP = glm::translate(P*V, glm::vec3(0.f, 0.f, -2.f));
	
	
	 // Boucle principale
    bool done = false;
    while(!done) {
        // Nettoyage de la fenêtre
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mise à jour de l'affichage
        SDL_GL_SwapBuffers();
        
        //Dessin
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
        
        glBindVertexArray(cubeVAO);
        	glDrawArrays(GL_TRIANGLES, 0, nbVertices);
        glBindVertexArray(0);
        
        // Boucle de gestion des évenements
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type){
            	case SDL_QUIT:
            		done = true;
            	break;
            	
            	case SDL_KEYDOWN:
            		switch(e.key.keysym.sym){
            			case SDLK_q:
            				done = true;
            			break;
            			
            			default:
            			break;
            		}
            	break;
            	
            	default:
            	break;
            }
            
            //Idle
        }
    }
    
    // Destruction des ressources OpenGL
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    
    //Desallocation
    delete[] tabV;
    delete[] tabF;
    
    SDL_Quit();
	
	return EXIT_SUCCESS;
}
