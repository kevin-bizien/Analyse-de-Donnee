/**
 * \file process.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "donnee.h"

/**
 *return Donnees (voir struct)
 */
Donnees parse(char *l)
{
	char *token = NULL;
	Donnees donnees = malloc(sizeof(struct Donnees));;
	donnees->t = atof(strtok(l," "));         //t

	int temp = atoi(strtok(NULL," "));        //code
	switch (temp){
		case 0 : 
			donnees->code = 0; 
			break;
		case 1 : 
			donnees->code = 1; 
			break;
		case 2 : 
			donnees->code = 2; 
			break;
		case 3 : 
			donnees->code = 3; 
			break;
		case 4 : 
			donnees->code = 4; 
			break;
		default : 
			donnees->code = 0; 
			break;
	}
	donnees->pid = atoi(strtok(NULL," "));    //pid
	donnees->fid = atoi(strtok(NULL," "));    //fid
	donnees->tos = atoi(strtok(NULL," "));    //tos

	if (donnees->code != 4) {                 //bif
		donnees->bif = atoi(strtok(NULL," "));
	}
	else {
		donnees->bif = -1;
	}

	token = strtok(NULL," ");                 //s
	token++;
	donnees->source = atoi(token);

	token = strtok(NULL," ");                 //d
	token++;
	donnees->destination = atoi(token);

	token = strtok(NULL," ");                 //pos
	token++;
	donnees->position = atoi(token);

	return donnees;
}

/**
 *return Liste_chainee : La Liste_chainee en param mis à jour, avec le nouveau paquet en tête de Liste_chainee
 */
Liste_chainee ajout(Donnees d, Liste_chainee liste_chainee)
{
	Liste* parcours = malloc(sizeof(Liste));
	parcours->pid = d->pid;
	parcours->source = d->source;
	parcours->destination = d->destination;
	parcours->duree = d->t;
	parcours->attente_file = -d->t;
	parcours->suite = liste_chainee;
	return parcours;
}

/**
 *return Liste_chainee : La Liste_chainee en param mis à jour, avec le paquet retiré s'il était présent
 */
Liste_chainee retrait(int pid, Liste_chainee liste_chainee)
{
	if (liste_chainee == NULL)
		return NULL;

	else if(liste_chainee->pid == pid){
		Liste *tmp = liste_chainee->suite;
		free(liste_chainee);
		return tmp;
	}
	else {
		liste_chainee->suite = retrait(pid,liste_chainee->suite);
		return liste_chainee;
	}
}

/**
 * \brief libération d'espace mémoire
 */
void free_Liste_chainee(Liste_chainee liste_chainee)
{
	Liste *tmp;
	while(liste_chainee!=NULL){
		tmp = liste_chainee->suite;
		free(liste_chainee);
		liste_chainee = tmp;
	}
}

/**
 *return Liste_chainee : On renvoie le maillon identifié par le pid
 */
Liste_chainee parcours(Liste_chainee liste_chainee, int pid)
{
	if (liste_chainee == NULL)
		return NULL;
	if(liste_chainee->pid == pid)
		return liste_chainee;
	else 
		return parcours(liste_chainee->suite,pid);
}

/**
 *return Liste_chainee : La Liste_chainee mis à jour
 */
Liste_chainee ajout_attente(Liste_chainee liste_chainee, int pid, double t)
{
	if (liste_chainee == NULL)
		return NULL;

	else if(liste_chainee->pid == pid){
		liste_chainee->attente_file += t;
		return liste_chainee;
	}
	else {
		liste_chainee->suite = ajout_attente(liste_chainee->suite, pid, t);
		return liste_chainee;
	}
}

/**
 *return Liste_chainee : La Liste_chainee mis à jour
 */
Liste_chainee retrait_attente(Liste_chainee liste_chainee, int pid, double t)
{
	if (liste_chainee == NULL)
		return NULL;

	else if(liste_chainee->pid == pid){
		liste_chainee->attente_file -= t;
		return liste_chainee;
	}
	else {
		liste_chainee->suite = retrait_attente(liste_chainee->suite, pid, t);
		return liste_chainee;
	}
}

/**
 *return Calcul_Moy : mise à jour pour la moyenne, écart type
 */
Calcul_Moy maj_moyenne(Calcul_Moy m, double x)
{
	m.n++;
	double temp = m.moyenne;
	if(m.n==1){
		m.moyenne=x;
		m.q=0;
		return(m);
	}
	m.moyenne=m.moyenne+((x-m.moyenne)/m.n);
	m.q=m.q+((x-temp)*(x-m.moyenne));
	return(m);
}

/**
 *return double : ecart_type correspondant a la moyenne
 */
double ecart_type(Calcul_Moy m)
{
	return(sqrt(m.q/(m.n)));
}
