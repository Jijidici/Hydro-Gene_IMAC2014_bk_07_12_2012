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

static const size_t WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
static const size_t BYTES_PER_PIXEL = 32;

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
    
    // Creation des Shaders
    
    //Creation des matrices
	
	 // Boucle principale
    bool done = false;
    while(!done) {
        // Nettoyage de la fenêtre
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mise à jour de l'affichage
        SDL_GL_SwapBuffers();
        
        //Dessin
        
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
    
    
    //Desallocation
    delete[] tabV;
    delete[] tabF;
    
    SDL_Quit();
	
	return EXIT_SUCCESS;
}
