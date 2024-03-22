/*
Copyright ESIEE (2017) 

j.cousty@esiee.fr

This software is an image processing library whose purpose is to be
used primarily for research and teaching.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software. You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license. You don't have to accept the License,
but note that nothing other than this License grants you permission 
to use, propagate or modify any covered work.
*/


/* auteur Imane Youkana et Jean Cousty, janvier 2017 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include<semaphore.h>
#define _ParDMaps_h


typedef struct DistanceMap{

int *E;
int **Si;
int **Sip;
 int ** Sipp;
int **Ei;

// for the size of each Si 
int *TailleSi;
int *TailleSip;
int *TailleSipp;

 // for the size of each Ei 
int *TailleEi;  

// Booleen array
//sem_t *Traversed;
pthread_mutex_t *Traversed;

// Number of vertices in each subset E 

int indice; 

int d;   // distance 
int p;

int *start;

} DistanceMap;

/************************************Les fonctions*******************************************/
void DistanceMap_initialization(struct graphe *G, int p);
void DistanceMaps(struct graphe *G,struct DistanceMap *D,int p);
  

 
