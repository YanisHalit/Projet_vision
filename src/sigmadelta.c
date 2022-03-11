/* -------------------- */
/* --- sigmadelta.c --- */
/* -------------------- */

/*
* Copyright (c) 2004 - 2021, Lionel Lacassagne, All rights reserved
* University of Paris Sud, Laboratoire de Recherche en Informatique
* Sorbonne University - Laboratoire d'Informatique de Paris 6
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"

#include "../include/sigmadelta.h"

// -----------------------------------------------------------------------------------------
void SigmaDelta_Step0_line(uint8 *I, uint8 *M, uint8 *O, uint8 *V, uint8 *E, int j0, int j1)
// -----------------------------------------------------------------------------------------
{
    for( int x = j0; x < j1; x ++){
        store1(M, x, load(I, x));
        store1(V, x, SD_VMIN);
    }
}

// ------------------------------------------------------------------------------------------------
void SigmaDelta_1Step_line(uint8 *I, uint8 *M, uint8 *O, uint8 *V, uint8 *E, int k, int j0, int j1)
// ------------------------------------------------------------------------------------------------
{
    uint8 mt_1_x, it_x, vt_1_x, ot_x;

    // step 1
    for( int x = j0; x < j1; x++){
        mt_1_x = load(M, x);
        it_x = load(I, x);
        
        if( mt_1_x < it_x){
            store1(M, x, mt_1_x + 1);
        }
        else if( mt_1_x > it_x ){
            store1(M, x, mt_1_x - 1);
        }
    }

    // step 2
    for( int x = j0; x < j1; x++){
        store1(O, x, abs(load(M,x) - load(I, x)));
    }

    // step 3
    for( int x = j0; x < j1; x++){
        vt_1_x = load(V, x);
        ot_x = load(O, x) * N;
        
        if( vt_1_x < ot_x ){
            store1(V, x, vt_1_x + 1);
        }
        else if( vt_1_x > ot_x ){
            store1(V, x, vt_1_x - 1);
        }
        store1(V, x, max2(  min2( load(V, x), SD_VMAX) , SD_VMIN));
    }

    // step 4
    for( int x = j0; x < j1; x++){
        ot_x = load(O, x);
        vt_1_x = load(V, x);
        if( ot_x < vt_1_x ){
            store1(E, x, 0);
        }
        else{
            store1(E, x, 1);
        }
    }


}
// ---------------------------------------------------------------------------------------------------------
void SigmaDelta_Step0(uint8 **I, uint8 **M, uint8 **O, uint8 **V, uint8 **E, int i0, int i1, int j0, int j1)
// ---------------------------------------------------------------------------------------------------------
{
    for( int x = i0; x < i1; x++){
        SigmaDelta_Step0_line( I[x], M[x], O[x], V[x], E[x], j0, j1);
    }
}
// ----------------------------------------------------------------------------------------------------------------
void SigmaDelta_1Step(uint8 **I, uint8 **M, uint8 **O, uint8 **V, uint8 **E, int k, int i0, int i1, int j0, int j1)
// ----------------------------------------------------------------------------------------------------------------
{
    for( int x = i0; x < i1; x++){
        SigmaDelta_1Step_line( I[x], M[x], O[x], V[x], E[x], k, j0, j1);
    }
}
