/**
 * \file matrice.h
 * \brief Permet de traiter la matrice d'adjacence
 */

#ifndef _MATRICE_H
#define __MATRICE_H

/*
 * Cette structure permet de créer une matrice
 */
typedef int **matrix;


int nb_noeuds(FILE *fichier);

/**
 *return matrix : celle du fichier de la matrice d'adjacence
 */
//matrix creation_matrice(FILE *fichier);

/**
 *return matrix : Rend la matrice symétrique si elle ne l'est pas déjà (dû à ses propriétes)
 */
//matrix symetrie_matrice(FILE *fichier, matrix M);

/**
 *return int : Somme de tous les éléments de la matrice (correspondant au flux total)
 */
//int flux_total(FILE *fichier, matrix M);


#endif