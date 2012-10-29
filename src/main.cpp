#include <iostream>
#include <fstream>

using namespace std;

int main(){

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
	double tabV[sizeTabVertice]; //on crée le tableau (j'ai voulu l'initialiser mais ça buggait alors tant pis)
	fread(tabV, sizeof(tabV), 1, fichier); //on remplit le tableau
	
	for (int i =0 ; i < 20 ; ++i) cout << tabV[i] << " "; //mini affichage pour voir si ça fonctionne
	cout << endl;
	//(j'ai testé d'une autre manière pr voir si j'obtenais la même chose, ça marchait)
	
	//lecture des index des points qui constituent chaque face (idem)
	int const sizeTabFace(nbFace*3);
	int tabF[sizeTabFace]; //il a pas reconnu le type uint, je sais pas pourquoi
	fread(tabF, sizeof(tabF), 1, fichier);
	for (int j =0 ; j < 20 ; ++j) cout << tabF[j] << " ";
	cout << endl;

	fclose(fichier);

	return 0;
}
