/* -------------------------- */
/* --- morpho_ouverture.c --- */
/* -------------------------- */

/*
 * Copyright (c) 2020 - 2021, Lionel Lacassagne, All rights reserved
  
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"
//#include "sequence.h"

#include "../include/swp.h"
#include "../include/morpho_min.h"
#include "../include/morpho_max.h"





// -------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion(uint8 **X, int i, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
{
    uint8   min_haut, min_milieu, min_bas,
            min_all1, min_all2, min_all3,
            max_all1, max_all2, max_all3;

    for(int j = j0; j< j1; j++){

/// ---------------
        min_haut = min3( load2(X, i-2, j-2), load2(X, i-2, j-1), load2(X, i-2, j));
        min_milieu = min3( load2(X, i-1, j-2), load2(X, i-1, j-1), load2(X, i-1, j));
        min_bas = min3( load2(X, i, j-2), load2(X, i, j-1), load2(X, i, j));
        min_all1 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i-2, j-1), load2(X, i-2, j), load2(X, i-2, j+1));
        min_milieu = min3( load2(X, i-1, j-1), load2(X, i-1, j), load2(X, i-1, j+1));
        min_bas = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i-2, j), load2(X, i-2, j+1), load2(X, i-2, j+2));
        min_milieu = min3( load2(X, i-1, j), load2(X, i-1, j+1), load2(X, i-1, j+2));
        min_bas = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // -----
        max_all1 = max3( min_all1, min_all2, min_all3);

/// ---------------
        min_haut = min3( load2(X, i-1, j-2), load2(X, i-1, j-1), load2(X, i-1, j));
        min_milieu = min3( load2(X, i, j-2), load2(X, i, j-1), load2(X, i, j));
        min_bas = min3( load2(X, i+1, j-2), load2(X, i+1, j-1), load2(X, i+1, j));
        min_all1 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i-1, j-1), load2(X, i-1, j), load2(X, i-1, j+1));
        min_milieu = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_bas = min3( load2(X, i+1, j-1), load2(X, i+1, j), load2(X, i+1, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i-1, j), load2(X, i-1, j+1), load2(X, i-1, j+2));
        min_milieu = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_bas = min3( load2(X, i+1, j), load2(X, i+1, j+1), load2(X, i+1, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // -----
        max_all2 = max3( min_all1, min_all2, min_all3);

/// ---------------
        min_haut = min3( load2(X, i, j-2), load2(X, i, j-1), load2(X, i, j));
        min_milieu = min3( load2(X, i+1, j-2), load2(X, i+1, j-1), load2(X, i+1, j));
        min_bas = min3( load2(X, i+2, j-2), load2(X, i+2, j-1), load2(X, i+2, j));
        min_all1 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_milieu = min3( load2(X, i+1, j-1), load2(X, i+1, j), load2(X, i+1, j+1));
        min_bas = min3( load2(X, i+2, j-1), load2(X, i+2, j), load2(X, i+2, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // -----
        min_haut = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_milieu = min3( load2(X, i+1, j), load2(X, i+1, j+1), load2(X, i+1, j+2));
        min_bas = min3( load2(X, i+2, j), load2(X, i+2, j+1), load2(X, i+2, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // -----
        max_all3 = max3( min_all1, min_all2, min_all3);

/// ---------------
        store2( Y, i, j, max3( max_all1, max_all2, max_all3)  );
    }

}
// ----------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------------
{

}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------------------------
{
}
// -----------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// -----------------------------------------------------------------------------------------
{
}






// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y, uint8 **Z)
// ---------------------------------------------------------------------------------------------
{
    min3_ui8matrix_basic(X, i0-1, i1+1, j0-1, j1+1, Y);
    max3_ui8matrix_basic(Y, i0,   i1,   j0,   j1,   Z);
}
// -----------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -----------------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------------------
{
}
// -------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------------------------
{
}
// --------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------------------------------
{
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------------
{
}
// ------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ------------------------------------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ----------------------------------------------------------------------------------------------------
{
}
// ---------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ---------------------------------------------------------------------------------------------------------
{
}
// ---------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ---------------------------------------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ----------------------------------------------------------------------------------------------------------------
{
}
// --------------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// --------------------------------------------------------------------------------------------------------------
{
}
// ---------------------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
// ---------------------------------------------------------------------------------------------------------------------
{
}
