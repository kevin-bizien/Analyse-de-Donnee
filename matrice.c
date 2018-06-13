/**
 * \file matrice.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "matrice.h"


/**
 *return int : Le nombre de noeuds qui correspond
 */
int nb_noeuds(FILE *fichier)
{
	int n = 0, i = 0;
	char *ligne = NULL;
	size_t len = 0;
	getline(&ligne,&len,fichier);
	while (ligne[i]){
		if (ligne[i] == '\t'){
			n++;
		}
		else {

		}
		i++;
	}
	return n+1;
}

/*
matrix creation_matrice(FILE *fichier)
{
	int i, n, p;

	matrix matrice = NULL;
	int m = nb_noeuds(fichier);

	matrice= (int **)calloc(m, sizeof(int*));
	for(i=0; i<m; i++)
	{
		matrice[i]=(int *)calloc(m, sizeof(int));
	}

	char *ligne = NULL;
	size_t len = 0;

	for (n=0; n<m; n++) {
		getline(&ligne, &len, fichier);
		matrice [n][0] = atoi(strtok(ligne,"\t"));
		for (p=1; p<m; p++) {
			matrice[n][p] = atoi(strtok(ligne,"\t"));
		}
	}
	return matrice;
}


matrix symetrie_matrice(FILE *fichier, matrix M)
{

	int n,p,m=nb_noeuds(fichier);
	matrix M_finale = M;
	
	for(n=0; n<m/2; n++)
	{
		for(p=0; p<m/2; p++)
		{
			if (M_finale[n][p] != 0) {
				M_finale[p][n] = M_finale[n][p];
			}
			else if (M_finale[p][n] != 0){
				M_finale[n][p] = M_finale[p][n];
			}
		}
	}
	return M_finale;
}

int flux_total(FILE *fichier, matrix M) {
	int m=nb_noeuds(fichier);
	int n, p, compteur=0;

	for(n=0; n<m/2; n++) {
		for (p=0; p<m/2; p++) {
			compteur += 2*M[n][p];           //dû à la symétrie
		}
	}
	return compteur;
}


int main(int argc, char **argv) {

	if (argc != 2) {
		printf("erreur");
	}

	FILE *matrice = fopen(argv[1],"r");
	matrix M = NULL;

	int m=nb_noeuds(matrice);
	
	M = creation_matrice(matrice);

	printf("nb noeuds : %d \n", m);

	for (int n=0; n<m; n++) {
		for (int p=0; p<m; p++) {
			printf ("%d   ", M[n][p]);
		}
		printf("\n");
	}
	
	return 0;
}
*/
