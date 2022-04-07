/* --------------- */
/* --- nrdef.h --- */
/* --------------- */

/*
 * Copyright (c) 2000 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#ifndef __NRDEF_H__
#define __NRDEF_H__

#define load(X, i) X[i]
#define load2(X, i, j) X[i][j]
#define store1(X, i, x) X[i] = x
#define store2(X, i, j, x) X[i][j] = x
#define min3(x,y,z) x&y&z
#define min2(x,y) x&y
#define max3(x,y,z) x|y|z
#define max2(x,y) x|y

#define TRUE 1
#define FALSE 0

#define IMAGE_EXPORT(X) X

#endif // __NRDEF_H__
