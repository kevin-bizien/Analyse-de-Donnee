/**
 * \file process_dat.h
 * \brief Permet de traiter les données d'un fichier de trace
 */

#ifndef _PROCESS_DATA_H
#define __PROCESS_DATA_H


/*
 * Cette structure parse une ligne du ficher de trace
 */
typedef struct Donnees
{
	double t;
	int code;
	int pid;
	int fid;
	int tos;
	int bif;
	int source;
	int destination;
	int position;
} *Donnees;


/*
 * Cette structure permet de faire des moyennes (duree, attente_file)
 * et de se débarasser d'un paquet une fois arrivé à destination ou détruit
 */
typedef struct Liste Liste;
struct Liste
{
	int pid;
	int source;
	int destination;
	double duree;
	double attente_file;
	Liste *suite;
};
typedef Liste* Liste_chainee;


/*
 * Structure pour calculer l'écart type et la moyenne
 */
typedef struct
{
	double moyenne;
	double q;
	double n;
} Calcul_Moy;


Donnees parse(char *l);

Liste_chainee ajout(Donnees l, Liste_chainee liste_chainee);

Liste_chainee retrait(int pid, Liste_chainee liste_chainee);

void free_Liste_chainee(Liste_chainee liste_chainee);

Liste_chainee parcours(Liste_chainee liste_chainee, int pid);

Liste_chainee ajout_attente(Liste_chainee liste_chainee, int pid, double t);

Liste_chainee retrait_attente(Liste_chainee liste_chainee, int pid, double t);

Calcul_Moy maj_moyenne(Calcul_Moy m, double x);

double ecart_type(Calcul_Moy m);


#endif