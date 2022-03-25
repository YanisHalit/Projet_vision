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
{
    uint8   min_haut, min_milieu, min_bas,
            min_all1, min_all2, min_all3,
            max_all1, max_all2, max_all3;

    for(int j = j0; j< j1; j++){
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
        store2( Y, i, j, max3( max_all1, max_all2, max_all3)  );
    }

}
// ----------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------------
{

    uint8   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            max_all0, max_all1, max_all2;

    min0_0 = min3( load2(X, i-2, j0-2), load2(X, i-1, j0-2), load2(X, i, j0-2));
    min0_1 = min3( load2(X, i-2, j0-1), load2(X, i-1, j0-1), load2(X, i, j0-1));
    min0_2 = min3( load2(X, i-2, j0), load2(X, i-1, j0), load2(X, i, j0));
    min0_3 = min3( load2(X, i-2, j0+1), load2(X, i-1, j0+1), load2(X, i, j0+1));

    min1_0 = min3( load2(X, i-1, j0-2), load2(X, i, j0-2), load2(X, i+1, j0-2));
    min1_1 = min3( load2(X, i-1, j0-1), load2(X, i, j0-1), load2(X, i+1, j0-1));
    min1_2 = min3( load2(X, i-1, j0), load2(X, i, j0), load2(X, i+1, j0));
    min1_3 = min3( load2(X, i-1, j0+1), load2(X, i, j0+1), load2(X, i+1, j0+1));

    min2_0 = min3( load2(X, i, j0-2), load2(X, i+1, j0-2), load2(X, i+2, j0-2));
    min2_1 = min3( load2(X, i, j0-1), load2(X, i+1, j0-1), load2(X, i+2, j0-1));
    min2_2 = min3( load2(X, i, j0), load2(X, i+1, j0), load2(X, i+2, j0));
    min2_3 = min3( load2(X, i, j0+1), load2(X, i+1, j0+1), load2(X, i+2, j0+1));

    min_all0_0 = min3( min0_0, min0_1, min0_2);
    min_all0_1 = min3( min0_1, min0_2, min0_3);

    min_all1_0 = min3( min1_0, min1_1, min1_2);
    min_all1_1 = min3( min1_1, min1_2, min1_3);

    min_all2_0 = min3( min2_0, min2_1, min2_2);
    min_all2_1 = min3( min2_1, min2_2, min2_3);

    for(int j = j0; j< j1-5; j+=5){
            // ----- 1 ------
            min0_4 = min3( load2(X, i-2, j+2), load2(X, i-1, j+2), load2(X, i, j+2));
            min1_4 = min3( load2(X, i-1, j+2), load2(X, i, j+2), load2(X, i+1, j+2));
            min2_4 = min3( load2(X, i, j+2), load2(X, i+1, j+2), load2(X, i+2, j+2));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);

            // ------- 2 -------- 
            min0_4 = min3( load2(X, i-2, j+3), load2(X, i-1, j+3), load2(X, i, j+3));
            min1_4 = min3( load2(X, i-1, j+3), load2(X, i, j+3), load2(X, i+1, j+3));
            min2_4 = min3( load2(X, i, j+3), load2(X, i+1, j+3), load2(X, i+2, j+3));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+1, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 3 --------
            min0_4 = min3( load2(X, i-2, j+4), load2(X, i-1, j+4), load2(X, i, j+4));
            min1_4 = min3( load2(X, i-1, j+4), load2(X, i, j+4), load2(X, i+1, j+4));
            min2_4 = min3( load2(X, i, j+4), load2(X, i+1, j+4), load2(X, i+2, j+4));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+2, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 4 --------
            min0_4 = min3( load2(X, i-2, j+5), load2(X, i-1, j+5), load2(X, i, j+5));
            min1_4 = min3( load2(X, i-1, j+5), load2(X, i, j+5), load2(X, i+1, j+5));
            min2_4 = min3( load2(X, i, j+5), load2(X, i+1, j+5), load2(X, i+2, j+5));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+3, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 5 --------
            min0_4 = min3( load2(X, i-2, j+6), load2(X, i-1, j+6), load2(X, i, j+6));
            min1_4 = min3( load2(X, i-1, j+6), load2(X, i, j+6), load2(X, i+1, j+6));
            min2_4 = min3( load2(X, i, j+6), load2(X, i+1, j+6), load2(X, i+2, j+6));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+4, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
    }

    int r = (j1-j0-1) % 5;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------------
{
    uint8   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min3_0, min3_1, min3_2, min3_3, min3_4,

            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            min_all3_0, min_all3_1, min_all3_2,
            max_all0, max_all1, max_all2, max_all3;

    min0_0 = min3( load2(X, i-2, j0-2), load2(X, i-1, j0-2), load2(X, i, j0-2));
    min0_1 = min3( load2(X, i-2, j0-1), load2(X, i-1, j0-1), load2(X, i, j0-1));
    min0_2 = min3( load2(X, i-2, j0), load2(X, i-1, j0), load2(X, i, j0));
    min0_3 = min3( load2(X, i-2, j0+1), load2(X, i-1, j0+1), load2(X, i, j0+1));

    min1_0 = min3( load2(X, i-1, j0-2), load2(X, i, j0-2), load2(X, i+1, j0-2));
    min1_1 = min3( load2(X, i-1, j0-1), load2(X, i, j0-1), load2(X, i+1, j0-1));
    min1_2 = min3( load2(X, i-1, j0), load2(X, i, j0), load2(X, i+1, j0));
    min1_3 = min3( load2(X, i-1, j0+1), load2(X, i, j0+1), load2(X, i+1, j0+1));

    min2_0 = min3( load2(X, i, j0-2), load2(X, i+1, j0-2), load2(X, i+2, j0-2));
    min2_1 = min3( load2(X, i, j0-1), load2(X, i+1, j0-1), load2(X, i+2, j0-1));
    min2_2 = min3( load2(X, i, j0), load2(X, i+1, j0), load2(X, i+2, j0));
    min2_3 = min3( load2(X, i, j0+1), load2(X, i+1, j0+1), load2(X, i+2, j0+1));

    min3_0 = min3( load2(X, i+1, j0-2), load2(X, i+2, j0-2), load2(X, i+3, j0-2));
    min3_1 = min3( load2(X, i+1, j0-1), load2(X, i+2, j0-1), load2(X, i+3, j0-1));
    min3_2 = min3( load2(X, i+1, j0), load2(X, i+2, j0), load2(X, i+3, j0));
    min3_3 = min3( load2(X, i+1, j0+1), load2(X, i+2, j0+1), load2(X, i+3, j0+1));

    min_all0_0 = min3( min0_0, min0_1, min0_2);
    min_all0_1 = min3( min0_1, min0_2, min0_3);

    min_all1_0 = min3( min1_0, min1_1, min1_2);
    min_all1_1 = min3( min1_1, min1_2, min1_3);

    min_all2_0 = min3( min2_0, min2_1, min2_2);
    min_all2_1 = min3( min2_1, min2_2, min2_3);

    min_all3_0 = min3( min3_0, min3_1, min3_2);
    min_all3_1 = min3( min3_1, min3_2, min3_3);

    for(int j = j0; j< j1-5; j+=5){
            // ----- 1 ------
            min0_4 = min3( load2(X, i-2, j+2), load2(X, i-1, j+2), load2(X, i, j+2));
            min1_4 = min3( load2(X, i-1, j+2), load2(X, i, j+2), load2(X, i+1, j+2));
            min2_4 = min3( load2(X, i, j+2), load2(X, i+1, j+2), load2(X, i+2, j+2));
            min3_4 = min3( load2(X, i+1, j+2), load2(X, i+2, j+2), load2(X, i+3, j+2));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);

            // ----- 2 ------
            min0_4 = min3( load2(X, i-2, j+3), load2(X, i-1, j+3), load2(X, i, j+3));
            min1_4 = min3( load2(X, i-1, j+3), load2(X, i, j+3), load2(X, i+1, j+3));
            min2_4 = min3( load2(X, i, j+3), load2(X, i+1, j+3), load2(X, i+2, j+3));
            min3_4 = min3( load2(X, i+1, j+3), load2(X, i+2, j+3), load2(X, i+3, j+3));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+1, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+1, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);

            // ----- 3 ------
            min0_4 = min3( load2(X, i-2, j+4), load2(X, i-1, j+4), load2(X, i, j+4));
            min1_4 = min3( load2(X, i-1, j+4), load2(X, i, j+4), load2(X, i+1, j+4));
            min2_4 = min3( load2(X, i, j+4), load2(X, i+1, j+4), load2(X, i+2, j+4));
            min3_4 = min3( load2(X, i+1, j+4), load2(X, i+2, j+4), load2(X, i+3, j+4));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+2, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+2, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);

            // ----- 4 ------
            min0_4 = min3( load2(X, i-2, j+5), load2(X, i-1, j+5), load2(X, i, j+5));
            min1_4 = min3( load2(X, i-1, j+5), load2(X, i, j+5), load2(X, i+1, j+5));
            min2_4 = min3( load2(X, i, j+5), load2(X, i+1, j+5), load2(X, i+2, j+5));
            min3_4 = min3( load2(X, i+1, j+5), load2(X, i+2, j+5), load2(X, i+3, j+5));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+3, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+3, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);

            // ----- 5 ------
            min0_4 = min3( load2(X, i-2, j+6), load2(X, i-1, j+6), load2(X, i, j+6));
            min1_4 = min3( load2(X, i-1, j+6), load2(X, i, j+6), load2(X, i+1, j+6));
            min2_4 = min3( load2(X, i, j+6), load2(X, i+1, j+6), load2(X, i+2, j+6));
            min3_4 = min3( load2(X, i+1, j+6), load2(X, i+2, j+6), load2(X, i+3, j+6));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+4, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+4, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);
    }

    int r = (j1-j0-1) % 5;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_ouverture3_ui8matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);

}
// ----------------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------------------------
{
    uint8   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min3_0, min3_1, min3_2, min3_3, min3_4,

            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            min_all3_0, min_all3_1, min_all3_2,
            max_all0, max_all1, max_all2, max_all3,

            min0_0_opt, min0_1_opt,
            min1_0_opt, min1_1_opt,
            min2_0_opt, min2_1_opt,
            min3_0_opt, min3_1_opt,
            min4_0_opt, min4_1_opt,

            min_all0_opt, min_all1_opt, min_all2_opt, min_all3_opt;


// -------------
    min0_0_opt = min2(load2(X, i-1, j0-2), load2(X, i, j0-2));
    min0_1_opt = min2(load2(X, i+1, j0-2), load2(X, i+2, j0-2));

    min1_0_opt = min2(load2(X, i-1, j0-1), load2(X, i, j0-1));
    min1_1_opt = min2(load2(X, i+1, j0-1), load2(X, i+2, j0-1));

    min2_0_opt = min2(load2(X, i-1, j0), load2(X, i, j0));
    min2_1_opt = min2(load2(X, i+1, j0), load2(X, i+2, j0));

    min3_0_opt = min2(load2(X, i-1, j0+1), load2(X, i, j0+1));
    min3_1_opt = min2(load2(X, i+1, j0+1), load2(X, i+2, j0+1));

// --------------
    min0_0 = min2( load2(X, i-2, j0-2), min0_0_opt);
    min0_1 = min2( load2(X, i-2, j0-1), min1_0_opt);
    min0_2 = min2( load2(X, i-2, j0), min2_0_opt);
    min0_3 = min2( load2(X, i-2, j0+1), min3_0_opt);

    min1_0 = min2( min0_0_opt, load2(X, i+1, j0-2));
    min1_1 = min2( min1_0_opt, load2(X, i+1, j0-1));
    min1_2 = min2( min2_0_opt, load2(X, i+1, j0));
    min1_3 = min2( min3_0_opt, load2(X, i+1, j0+1));

    min2_0 = min2( load2(X, i, j0-2), min0_1_opt);
    min2_1 = min2( load2(X, i, j0-1), min1_1_opt);
    min2_2 = min2( load2(X, i, j0), min2_1_opt);
    min2_3 = min2( load2(X, i, j0+1), min3_1_opt);

    min3_0 = min2( min0_1_opt , load2(X, i+3, j0-2));
    min3_1 = min2( min1_1_opt, load2(X, i+3, j0-1));
    min3_2 = min2( min2_1_opt, load2(X, i+3, j0));
    min3_3 = min2( min3_1_opt, load2(X, i+3, j0+1));
// ----------

//_---------

    min_all0_opt = min2(min0_1, min0_2 );
    min_all1_opt = min2(min1_1, min1_2 );
    min_all2_opt = min2(min2_1, min2_2 );
    min_all3_opt = min2(min3_1, min3_2);

    min_all0_0 = min2( min0_0, min_all0_opt);
    min_all0_1 = min2( min_all0_opt, min0_3);

    min_all1_0 = min2( min1_0, min_all1_opt);
    min_all1_1 = min2( min_all1_opt, min1_3);

    min_all2_0 = min2( min2_0, min_all2_opt );
    min_all2_1 = min2( min_all2_opt , min2_3);

    min_all3_0 = min2( min3_0, min_all3_opt);
    min_all3_1 = min2( min_all3_opt, min3_3);

for(int j = j0; j< j1-5; j+=5){ //            min4_0_opt, min4_1_opt,
            // ----- 1 ------
            min4_0_opt = min2(load2(X, i-1, j+2), load2(X, i, j+2));
            min4_1_opt = min2(load2(X, i+1, j+2), load2(X, i+2, j+2));

            min0_4 = min2( load2(X, i-2, j+2), min4_0_opt);
            min1_4 = min2( min4_0_opt, load2(X, i+1, j+2));
            min2_4 = min2( load2(X, i, j+2), min4_1_opt);
            min3_4 = min2( min4_1_opt, load2(X, i+3, j+2));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);
            min_all3_2 = min3( min3_2, min3_3, min3_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j, max3( max_all1, max_all2, max_all3) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min3_0 = min3_1;
            min3_1 = min3_2;
            min3_2 = min3_3;
            min3_3 = min3_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_0, min3_1, min3_2);
            min_all3_1 = min3( min3_1, min3_2, min3_3);
}

}
// -----------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// -----------------------------------------------------------------------------------------
{
    uint8   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min_all0_0, min_all0_1, min_all0_2, // min_all0_2 sera chargé dans la boucle
            min_all1_0, min_all1_1, min_all1_2, // min_all0_2 sera chargé dans la boucle 
            min_all2_0, min_all2_1, min_all2_2, // min_all2_2 sera chargé dans la boucle
            max_all0, max_all1, max_all2;

    min0_0 = min3( load2(X, i-2, j0-2), load2(X, i-1, j0-2), load2(X, i, j0-2));
    min0_1 = min3( load2(X, i-2, j0-1), load2(X, i-1, j0-1), load2(X, i, j0-1));
    min0_2 = min3( load2(X, i-2, j0), load2(X, i-1, j0), load2(X, i, j0));
    min0_3 = min3( load2(X, i-2, j0+1), load2(X, i-1, j0+1), load2(X, i, j0+1));

    min1_0 = min3( load2(X, i-1, j0-2), load2(X, i, j0-2), load2(X, i+1, j0-2));
    min1_1 = min3( load2(X, i-1, j0-1), load2(X, i, j0-1), load2(X, i+1, j0-1));
    min1_2 = min3( load2(X, i-1, j0), load2(X, i, j0), load2(X, i+1, j0));
    min1_3 = min3( load2(X, i-1, j0+1), load2(X, i, j0+1), load2(X, i+1, j0+1));

    min2_0 = min3( load2(X, i, j0-2), load2(X, i+1, j0-2), load2(X, i+2, j0-2));
    min2_1 = min3( load2(X, i, j0-1), load2(X, i+1, j0-1), load2(X, i+2, j0-1));
    min2_2 = min3( load2(X, i, j0), load2(X, i+1, j0), load2(X, i+2, j0));
    min2_3 = min3( load2(X, i, j0+1), load2(X, i+1, j0+1), load2(X, i+2, j0+1));

    //1er niveau
    min_all0_0 = min3( min0_0, min0_1, min0_2);  // calcul pour le premier point
    min_all0_1 = min3( min0_1, min0_2, min0_3);  // calcule pour le deuxieme point
    //2eme niveau
    min_all1_0 = min3( min1_0, min1_1, min1_2);  // calcul pour le premier point
    min_all1_1 = min3( min1_1, min1_2, min1_3);  // calcule pour le deuxieme point
    // 3 eme niveau 
    min_all2_0 = min3( min2_0, min2_1, min2_2);  // calcul pour le premier point
    min_all2_1 = min3( min2_1, min2_2, min2_3);

    for(int j = j0; j< j1-15; j+=15){
            // ----- 1 ------
            min0_4 = min3( load2(X, i-2, j+2), load2(X, i-1, j+2), load2(X, i, j+2));
            min1_4 = min3( load2(X, i-1, j+2), load2(X, i, j+2), load2(X, i+1, j+2));
            min2_4 = min3( load2(X, i, j+2), load2(X, i+1, j+2), load2(X, i+2, j+2));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            
            // ------- 2 -------- 
            min0_4 = min3( load2(X, i-2, j+3), load2(X, i-1, j+3), load2(X, i, j+3));
            min1_4 = min3( load2(X, i-1, j+3), load2(X, i, j+3), load2(X, i+1, j+3));
            min2_4 = min3( load2(X, i, j+3), load2(X, i+1, j+3), load2(X, i+2, j+3));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+1, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 3 --------
            min0_4 = min3( load2(X, i-2, j+4), load2(X, i-1, j+4), load2(X, i, j+4));
            min1_4 = min3( load2(X, i-1, j+4), load2(X, i, j+4), load2(X, i+1, j+4));
            min2_4 = min3( load2(X, i, j+4), load2(X, i+1, j+4), load2(X, i+2, j+4));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+2, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 4 --------
            min0_4 = min3( load2(X, i-2, j+5), load2(X, i-1, j+5), load2(X, i, j+5));
            min1_4 = min3( load2(X, i-1, j+5), load2(X, i, j+5), load2(X, i+1, j+5));
            min2_4 = min3( load2(X, i, j+5), load2(X, i+1, j+5), load2(X, i+2, j+5));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+3, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 5 --------
            min0_4 = min3( load2(X, i-2, j+6), load2(X, i-1, j+6), load2(X, i, j+6));
            min1_4 = min3( load2(X, i-1, j+6), load2(X, i, j+6), load2(X, i+1, j+6));
            min2_4 = min3( load2(X, i, j+6), load2(X, i+1, j+6), load2(X, i+2, j+6));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+4, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 6 --------
            min0_4 = min3( load2(X, i-2, j+7), load2(X, i-1, j+7), load2(X, i, j+7));
            min1_4 = min3( load2(X, i-1, j+7), load2(X, i, j+7), load2(X, i+1, j+7));
            min2_4 = min3( load2(X, i, j+7), load2(X, i+1, j+7), load2(X, i+2, j+7));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+5, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 7 --------
            min0_4 = min3( load2(X, i-2, j+8), load2(X, i-1, j+8), load2(X, i, j+8));
            min1_4 = min3( load2(X, i-1, j+8), load2(X, i, j+8), load2(X, i+1, j+8));
            min2_4 = min3( load2(X, i, j+8), load2(X, i+1, j+8), load2(X, i+2, j+8));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+6, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 8 --------
            min0_4 = min3( load2(X, i-2, j+9), load2(X, i-1, j+9), load2(X, i, j+9));
            min1_4 = min3( load2(X, i-1, j+9), load2(X, i, j+9), load2(X, i+1, j+9));
            min2_4 = min3( load2(X, i, j+9), load2(X, i+1, j+9), load2(X, i+2, j+9));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+7, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 9 --------
            min0_4 = min3( load2(X, i-2, j+10), load2(X, i-1, j+10), load2(X, i, j+10));
            min1_4 = min3( load2(X, i-1, j+10), load2(X, i, j+10), load2(X, i+1, j+10));
            min2_4 = min3( load2(X, i, j+10), load2(X, i+1, j+10), load2(X, i+2, j+10));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+8, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 10 --------
            min0_4 = min3( load2(X, i-2, j+11), load2(X, i-1, j+11), load2(X, i, j+11));
            min1_4 = min3( load2(X, i-1, j+11), load2(X, i, j+11), load2(X, i+1, j+11));
            min2_4 = min3( load2(X, i, j+11), load2(X, i+1, j+11), load2(X, i+2, j+11));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+9, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 11 --------
            min0_4 = min3( load2(X, i-2, j+12), load2(X, i-1, j+12), load2(X, i, j+12));
            min1_4 = min3( load2(X, i-1, j+12), load2(X, i, j+12), load2(X, i+1, j+12));
            min2_4 = min3( load2(X, i, j+12), load2(X, i+1, j+12), load2(X, i+2, j+12));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+10, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 12 --------
            min0_4 = min3( load2(X, i-2, j+13), load2(X, i-1, j+13), load2(X, i, j+13));
            min1_4 = min3( load2(X, i-1, j+13), load2(X, i, j+13), load2(X, i+1, j+13));
            min2_4 = min3( load2(X, i, j+13), load2(X, i+1, j+13), load2(X, i+2, j+13));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+11, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 13 --------
            min0_4 = min3( load2(X, i-2, j+14), load2(X, i-1, j+14), load2(X, i, j+14));
            min1_4 = min3( load2(X, i-1, j+14), load2(X, i, j+14), load2(X, i+1, j+14));
            min2_4 = min3( load2(X, i, j+14), load2(X, i+1, j+14), load2(X, i+2, j+14));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+12, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 14 --------
            min0_4 = min3( load2(X, i-2, j+15), load2(X, i-1, j+15), load2(X, i, j+15));
            min1_4 = min3( load2(X, i-1, j+15), load2(X, i, j+15), load2(X, i+1, j+15));
            min2_4 = min3( load2(X, i, j+15), load2(X, i+1, j+15), load2(X, i+2, j+15));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+13, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
            // ------- 15 --------
            min0_4 = min3( load2(X, i-2, j+16), load2(X, i-1, j+16), load2(X, i, j+16));
            min1_4 = min3( load2(X, i-1, j+16), load2(X, i, j+16), load2(X, i+1, j+16));
            min2_4 = min3( load2(X, i, j+16), load2(X, i+1, j+16), load2(X, i+2, j+16));

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+14, max3( max_all0, max_all1, max_all2) );

            min0_0 = min0_1;
            min0_1 = min0_2;
            min0_2 = min0_3;
            min0_3 = min0_4;

            min1_0 = min1_1;
            min1_1 = min1_2;
            min1_2 = min1_3;
            min1_3 = min1_4;

            min2_0 = min2_1;
            min2_1 = min2_2;
            min2_2 = min2_3;
            min2_3 = min2_4;

            min_all0_0 = min3( min0_0, min0_1, min0_2);
            min_all0_1 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_0, min1_1, min1_2);
            min_all1_1 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_0, min2_1, min2_2);
            min_all2_1 = min3( min2_1, min2_2, min2_3);
    }

    int r = (j1-j0-1) % 15;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
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
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion_ilu5_red(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------------------------
{
    for( int i = i0; i < i1-2; i+=2){
        line_ouverture3_ui8matrix_fusion_ilu5_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    line_ouverture3_ui8matrix_fusion(X, i1-r-1, j0, j1, Y);
}
// --------------------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------------------------------
{
    for( int i = i0; i < i1-2; i+=2){
        line_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    line_ouverture3_ui8matrix_fusion(X, i1-r-1, j0, j1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion_ilu15_red(X, i, j0, j1, Y);
    }
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
