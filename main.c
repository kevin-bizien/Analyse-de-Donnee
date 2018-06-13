/**
 * \file main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "donnee.h"
#include "matrice.h"

// BUFSIZE est utilisé pour le buffer, afin de minimiser le nombre d'écriture dans le fichier
#define BUFSIZE 512

// STRINGSIZE sert pour les chaînes de caractères que l'on ajoute au buffer
#define STRINGSIZE 50



int main(int argc, char **argv)
{
	if(argc < 4){
		fprintf(stderr,"./analyse <fichier de trace> <fichier de la matrice> <0 : sans graphe / 1 : avec graphe> <0 : sans suivi / 1 : avec suivi>\n");
		exit(1);
	}

	FILE *trace = fopen(argv[1],"r");
	FILE *matrice = fopen(argv[2],"r");
	int graphique = atoi(argv[3]);
	int pid = atoi(argv[4]);
	int pid_demande;
	if (pid) {
		printf("Veuillez saisir l'identifiant du paquet dont vous voulez avoir des informations\n");
		scanf("%d",&pid_demande);
	}
	FILE *graph1 = NULL;
	FILE *graph2 = NULL;
	FILE *graph3 = NULL;
	if (graphique){
		graph1 = fopen("flux_actifs.txt","w");
		graph2 = fopen("moy_attente.txt", "w");
		graph3 = fopen("paquets_perdus.txt", "w");
	}

	// Initialisations des entiers et pointeurs d'entiers
	int i = 0;
	int nb_departs = 0, nb_arrivees = 0, nb_destruction = 0, nb_depart_file = 0, nb_flux_actifs = 0;
	int nb_char1 = 0, nb_char2 = 0, nb_char3 = 0;

	// Tableau pour avoir le nombre de flux actif à la fin de chaque boucle
	int *tableau = NULL;
	int taille_tableau = 0;
	int taille = nb_noeuds(matrice);

	// Tableau comptant le nombre de paquets perdus par noeud
	int paquets_perdus[taille];
	for (i=0;i<taille;i++){
		paquets_perdus[i] = 0;
	}

	// Initialisations des chaines de caractères et des variables
	char *ligne = NULL;
	size_t len = 0;
	ssize_t read;
	char xy1[BUFSIZE+STRINGSIZE], xy2[BUFSIZE+STRINGSIZE], xy3[BUFSIZE+STRINGSIZE];
	char str_tmp1[STRINGSIZE], str_tmp2[STRINGSIZE], str_tmp3[STRINGSIZE];		

	// Initialisations des structures
	Donnees donnees;
	Liste_chainee liste_chainee = NULL;
	Liste_chainee tmp = NULL;
	Calcul_Moy moy_trajet;
	Calcul_Moy moy_attente;
	moy_trajet.moyenne = 0;
	moy_trajet.q = 0;
	moy_trajet.n = 0;
	moy_attente.moyenne = 0;
	moy_attente.q = 0;
	moy_attente.n = 0;
	

	// On lit une ligne tant qu'il y en a
	while((read=getline(&ligne, &len, trace)) != -1)
	{
		donnees = parse(ligne);

		// On fait une étude de cas en fonction de la valeur de 'code'
		switch(donnees->code)
		{
			case 0 : // départ de la source
			// Le paquet vient d'être créé, donc on l'ajoute et on incrémente nb_departs
				if (pid) {
					if (donnees->pid == pid_demande) {
						printf("----------------------------------------------\n");
						printf("Le paquet %d appartient au flux %d, part du nœud source N%d pour arriver au nœud destination N%d.\n", pid_demande, donnees->fid, donnees->source, donnees->destination);
						printf("Voici son parcours :\n-t = %lf : Départ de la source N%d\n", donnees->t, donnees->position);
					}
				}
				liste_chainee = ajout(donnees, liste_chainee);
				nb_departs++;
				// Cette condition permet d'agrandir la longeur du tableau comptant le nombre de flux actifs
				if (donnees->fid+1 > taille_tableau){
					tableau = malloc((donnees->fid+1)*sizeof(int));
					for (i = donnees->fid+1 - taille_tableau; i < donnees->fid+1; i++){
						tableau[i] = 0;
					}
					taille_tableau = donnees->fid+1;
				}
				tableau[donnees->fid]++;
				break;

			case 1 : // arrivée dans un nœud intermédiaire
				if (pid) {
					if (donnees->pid == pid_demande) {
						printf("-t = %lf : Arrivée dans le nœud intermédiaire N%d\n", donnees->t, donnees->position);
					}
				}
				liste_chainee = retrait_attente(liste_chainee, donnees->pid, donnees->t);
				break;

			case 2 : // départ d’une file d’attente
				if (pid) {
					if (donnees->pid == pid_demande) {
						printf("-t = %lf : Départ de la file d'attente\n", donnees->t);
					}
				}
				liste_chainee = ajout_attente(liste_chainee, donnees->pid, donnees->t);
				nb_depart_file++;
				break;

			case 3 : // arrivée à destination
			// On desincremente le tableau en fonction du 'fid',
			// on met a jour les moyennes et on libere l'espace alloué
				if (pid) {
					if (donnees->pid == pid_demande) {
						printf("-t = %lf : Arrivée à destination dans le nœud  N%d\n", donnees->t, donnees->position);
					}
				}
				tmp = parcours(liste_chainee, donnees->pid);
				tmp->attente_file += donnees->t;

				moy_trajet = maj_moyenne(moy_trajet, donnees->t - tmp->duree);
				moy_attente = maj_moyenne(moy_attente, tmp->attente_file);
				liste_chainee = retrait(donnees->pid, liste_chainee);
				nb_arrivees++;
				tableau[donnees->fid]--;

				if (graphique){
				// graphique de la moyenne d'attente en fonction du temps
					nb_char2 += sprintf(str_tmp2, "%lf\t%lf\n", donnees->t, moy_attente.moyenne);
					strcat(xy2, str_tmp2);
					if (nb_char2 > BUFSIZE){
						fputs(xy2,graph2);
						xy2[0] = '\0';
						nb_char2 = 0;
					}
				}				
				break;

			case 4 : // destruction d’un paquet (placement dans une file pleine)
			// On fait la même chose que pour l'arrivée, mais on incrémente la pertes de paquets
				if (pid) {
					if (donnees->pid == pid_demande) {
						printf("-t = %lf : Destruction du paquet dans le nœud N%d\n", donnees->t, donnees->position);
					}
				}
				tmp = parcours(liste_chainee, donnees->pid);
				tmp->attente_file += donnees->t;
				moy_attente = maj_moyenne(moy_attente, tmp->attente_file);
				liste_chainee = retrait(donnees->pid, liste_chainee);
				tableau[donnees->fid]--;
				nb_destruction++;
				paquets_perdus[donnees->position]++;

				if (graphique){ 
					nb_char2 += sprintf(str_tmp2, "%lf\t%lf\n", donnees->t, moy_attente.moyenne);
					strcat(xy2, str_tmp2);
					if (nb_char2 > BUFSIZE){
						fputs(xy2,graph2);
						xy2[0] = '\0';
						nb_char2 = 0;
					}
				// graphique du nombres de paquets perdus en fonction du temps
					nb_char3 += sprintf(str_tmp3, "%lf\t%d\n", donnees->t, nb_destruction);
					strcat(xy3, str_tmp3);
					if (nb_char3 > BUFSIZE){
						fputs(xy3,graph3);
						xy3[0] = '\0';
						nb_char3 = 0;
					}
				}	
				break;
				
			default : 
				break;
		}

		if(graphique) {
		// graphique du flux actifs en fonction du temps
			for (i=0;i<taille_tableau;i++){
				if(tableau[i]>0){
					nb_flux_actifs++;
				}
			}
			nb_char1 += sprintf(str_tmp1,"%lf\t%d\n",donnees->t,nb_flux_actifs);
			strcat(xy1, str_tmp1);
			if (nb_char1 > BUFSIZE){
				fputs(xy1,graph1);
				xy1[0] = '\0';
				nb_char1 = 0;
			}
		}

		free(donnees);
		nb_flux_actifs = 0;
		i++;
	}

	if(graphique){
		fputs(xy1,graph1);
		fputs(xy2,graph2);
		fputs(xy3,graph3);
	}

	// On imprime sur la console les résultats du traitement de données
	printf("----------------------------------------------\nQUELQUES NOMBRES\n");
	printf("Nombre de paquets émis de la source %d\n", nb_departs);
	printf("Nombre de paquets reçu à destination %d\n", nb_arrivees);
	printf("Nombre de paquets perdus %d\n", nb_destruction);
	printf("Nombre de départ d'une file d'attente %d\n", nb_depart_file);
	printf("----------------------------------------------\nQUELQUES POURCENTAGES\n");
	printf("%.2lf%% des paquets sont perdus\n", (double)nb_destruction/nb_departs*100);
	printf("Les pertes se trouvent au niveau des nœuds :\n");
	for (i=0;i<taille;i++){
		if(paquets_perdus[i]){
			printf("-N%d avec %.2lf%%\n", i, (double)paquets_perdus[i]/nb_destruction*100);
		}
	}
	printf("----------------------------------------------\nQUELQUES STATISTIQUES\n");
	printf("Trajet de bout en bout : moyenne = %lf; écart-type = %lf\n", moy_trajet.moyenne, ecart_type(moy_trajet));
	printf("Attente en file d'attente: moyenne = %lf; écart-type = %lf\n", moy_attente.moyenne, ecart_type(moy_attente));

	// On ouvre gnuplot pour tracer le graphe à partir des données que l'on a récupéré dans les fichiers
	if(graphique) 
	{	
		fclose(graph1);
		fclose(graph2);
		fclose(graph3);
		FILE *pipe1 = popen("gnuplot -persist","w");
		fprintf(pipe1, "plot 'flux_actifs.txt' using 1:2 with lines title \"Nombre de flux actifs en fonction du temps\" linecolor rgb \"blue\"\n");
		fflush(pipe1);
		fclose(pipe1);
		FILE *pipe2 = popen("gnuplot -persist","w");
		fprintf(pipe2, "plot 'moy_attente.txt' using 1:2 with lines title \"Moyenne d'attente dans une file en fonction du temps\" linecolor rgb \"red\"\n");
		fflush(pipe2);
		fclose(pipe2);
		FILE *pipe3 = popen("gnuplot -persist","w");
		fprintf(pipe3, "plot 'paquets_perdus.txt' using 1:2 with lines title \"Nombre de paquets perdus en fonction du temps\" linecolor rgb \"green\"\n");
		fflush(pipe3);
		fclose(pipe3);
	}

	free_Liste_chainee(liste_chainee);
	free(tableau);
	fclose(trace);
	fclose(matrice);
	return 0;
}