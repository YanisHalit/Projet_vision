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
#include "../include/swp.h"



        // FUSION SUR LIGNE
// ---------------------------------------------------------------------------------------------
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
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i-2, j-1), load2(X, i-2, j), load2(X, i-2, j+1));
        min_milieu = min3( load2(X, i-1, j-1), load2(X, i-1, j), load2(X, i-1, j+1));
        min_bas = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i-2, j), load2(X, i-2, j+1), load2(X, i-2, j+2));
        min_milieu = min3( load2(X, i-1, j), load2(X, i-1, j+1), load2(X, i-1, j+2));
        min_bas = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        max_all1 = max3( min_all1, min_all2, min_all3);

        min_haut = min3( load2(X, i-1, j-2), load2(X, i-1, j-1), load2(X, i-1, j));
        min_milieu = min3( load2(X, i, j-2), load2(X, i, j-1), load2(X, i, j));
        min_bas = min3( load2(X, i+1, j-2), load2(X, i+1, j-1), load2(X, i+1, j));
        min_all1 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i-1, j-1), load2(X, i-1, j), load2(X, i-1, j+1));
        min_milieu = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_bas = min3( load2(X, i+1, j-1), load2(X, i+1, j), load2(X, i+1, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i-1, j), load2(X, i-1, j+1), load2(X, i-1, j+2));
        min_milieu = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_bas = min3( load2(X, i+1, j), load2(X, i+1, j+1), load2(X, i+1, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        max_all2 = max3( min_all1, min_all2, min_all3);

        min_haut = min3( load2(X, i, j-2), load2(X, i, j-1), load2(X, i, j));
        min_milieu = min3( load2(X, i+1, j-2), load2(X, i+1, j-1), load2(X, i+1, j));
        min_bas = min3( load2(X, i+2, j-2), load2(X, i+2, j-1), load2(X, i+2, j));
        min_all1 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i, j-1), load2(X, i, j), load2(X, i, j+1));
        min_milieu = min3( load2(X, i+1, j-1), load2(X, i+1, j), load2(X, i+1, j+1));
        min_bas = min3( load2(X, i+2, j-1), load2(X, i+2, j), load2(X, i+2, j+1));
        min_all2 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        min_haut = min3( load2(X, i, j), load2(X, i, j+1), load2(X, i, j+2));
        min_milieu = min3( load2(X, i+1, j), load2(X, i+1, j+1), load2(X, i+1, j+2));
        min_bas = min3( load2(X, i+2, j), load2(X, i+2, j+1), load2(X, i+2, j+2));
        min_all3 = min3( min_haut, min_milieu, min_bas);
        // ---------------------------------------------------------------------------------------------
        max_all3 = max3( min_all1, min_all2, min_all3);
        store2( Y, i, j, max3( max_all1, max_all2, max_all3)  );
    }

}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
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

    uint8 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+2);
            load_im2_jp2 = load2(X, i-2, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);

            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            min_all0_2 = min3( min0_2, min0_3, min0_4);
            min_all1_2 = min3( min1_2, min1_3, min1_4);
            min_all2_2 = min3( min2_2, min2_3, min2_4);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j, max3( max_all0, max_all1, max_all2) );
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+3);
            load_im2_jp2 = load2(X, i-2, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            min_all0_0 = min3( min0_3, min0_4, min0_0);
            min_all1_0 = min3( min1_3, min1_4, min1_0);
            min_all2_0 = min3( min2_3, min2_4, min2_0);

            max_all0 = max3( min_all0_1, min_all0_2, min_all0_0);
            max_all1 = max3( min_all1_1, min_all1_2, min_all1_0);
            max_all2 = max3( min_all2_1, min_all2_2, min_all2_0);

            store2( Y, i, j+1, max3( max_all0, max_all1, max_all2) );

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+4);
            load_im2_jp2 = load2(X, i-2, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            min_all0_1 = min3( min0_4, min0_0, min0_1);
            min_all1_1 = min3( min1_4, min1_0, min1_1);
            min_all2_1 = min3( min2_4, min2_0, min2_1);

            max_all0 = max3( min_all0_2, min_all0_0, min_all0_1);
            max_all1 = max3( min_all1_2, min_all1_0, min_all1_1);
            max_all2 = max3( min_all2_2, min_all2_0, min_all2_1);

            store2( Y, i, j+2, max3( max_all0, max_all1, max_all2) );

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+5);
            load_im2_jp2 = load2(X, i-2, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            
            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            min_all0_2 = min3( min0_0, min0_1, min0_2);
            min_all1_2 = min3( min1_0, min1_1, min1_2);
            min_all2_2 = min3( min2_0, min2_1, min2_2);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);

            store2( Y, i, j+3, max3( max_all0, max_all1, max_all2) );

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+6);
            load_im2_jp2 = load2(X, i-2, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            
            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            min_all0_0 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_1, min2_2, min2_3);

            max_all0 = max3( min_all0_1, min_all0_2, min_all0_0);
            max_all1 = max3( min_all1_1, min_all1_2, min_all1_0);
            max_all2 = max3( min_all2_1, min_all2_2, min_all2_0);

            store2( Y, i, j+4, max3( max_all0, max_all1, max_all2) );

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;

    }

    int r = (j1-j0-1) % 5;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
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

    uint8 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2, load_ip3_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+2);
            load_im2_jp2 = load2(X, i-2, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            load_ip3_jp2 = load2(X, i+3, j+2);

            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_4 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

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

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+3);
            load_im2_jp2 = load2(X, i-2, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);
            load_ip3_jp2 = load2(X, i+3, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_0 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            min_all0_0 = min3( min0_3, min0_4, min0_0);
            min_all1_0 = min3( min1_3, min1_4, min1_0);
            min_all2_0 = min3( min2_3, min2_4, min2_0);
            min_all3_0 = min3( min3_3, min3_4, min3_0);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+1, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+1, max3( max_all1, max_all2, max_all3) );

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+4);
            load_im2_jp2 = load2(X, i-2, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);
            load_ip3_jp2 = load2(X, i+3, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_1 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            min_all0_1 = min3( min0_4, min0_0, min0_1);
            min_all1_1 = min3( min1_4, min1_0, min1_1);
            min_all2_1 = min3( min2_4, min2_0, min2_1);
            min_all3_1 = min3( min3_4, min3_0, min3_1);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+2, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+2, max3( max_all1, max_all2, max_all3) );

           
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+5);
            load_im2_jp2 = load2(X, i-2, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            load_ip3_jp2 = load2(X, i+3, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_2 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            min_all0_2 = min3( min0_0, min0_1, min0_2);
            min_all1_2 = min3( min1_0, min1_1, min1_2);
            min_all2_2 = min3( min2_0, min2_1, min2_2);
            min_all3_2 = min3( min3_0, min3_1, min3_2);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+3, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+3, max3( max_all1, max_all2, max_all3) );

            
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+6);
            load_im2_jp2 = load2(X, i-2, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            load_ip3_jp2 = load2(X, i+3, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_3 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            min_all0_0 = min3( min0_1, min0_2, min0_3);
            min_all1_0 = min3( min1_1, min1_2, min1_3);
            min_all2_0 = min3( min2_1, min2_2, min2_3);
            min_all3_0 = min3( min3_1, min3_2, min3_3);

            max_all0 = max3( min_all0_0, min_all0_1, min_all0_2);
            max_all1 = max3( min_all1_0, min_all1_1, min_all1_2);
            max_all2 = max3( min_all2_0, min_all2_1, min_all2_2);
            max_all3 = max3( min_all3_0, min_all3_1, min_all3_2);

            store2( Y, i, j+4, max3( max_all0, max_all1, max_all2) );
            store2( Y, i+1, j+4, max3( max_all1, max_all2, max_all3) );

            
            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;
            min_all3_1 = min_all3_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
            min_all3_0 = min_all3_2;

    }

    int r = (j1-j0-1) % 5;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_ouverture3_ui8matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
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

            min_00_10_opt, min0_1_opt,
            min_01_11_opt, min_03_13_opt,
            min_02_12_opt, min_10_20_opt,
            min_11_21_opt, min_12_22_opt,
            min_13_23_opt, min_20_30_opt,
            min_21_31_opt, min_22_32_opt,
            min_23_33_opt, min_04_14_opt,
            min_14_24_opt, min_24_opt,
            min_34_opt,

            min_all0_opt, min_all1_opt, min_all2_opt, min_all3_opt;

            uint8 min_02_03, min_12_13, min_22_23, min_32_33,
            max_all_00_01, max_all_10_11, max_all_20_21, max_all_30_31,
            // max_all_00_01, max_all_10_11, max_all_20_21, max_all_30_31,
            // max_all_00_01, max_all_10_11, max_all_20_21, max_all_30_31,
            max_all12;

    min_00_10_opt = min2(load2(X, i-1, j0-2), load2(X, i, j0-2));
    min0_0 = min2( load2(X, i-2, j0-2), min_00_10_opt);
    min1_0 = min2( min_00_10_opt, load2(X, i+1, j0-2));


    min_01_11_opt = min2(load2(X, i-1, j0-1), load2(X, i, j0-1));
    min0_1 = min2( load2(X, i-2, j0-1), min_01_11_opt);
    min1_1 = min2( min_01_11_opt, load2(X, i+1, j0-1));

    min_02_12_opt = min2(load2(X, i-1, j0), load2(X, i, j0));
    min0_2 = min2( load2(X, i-2, j0), min_02_12_opt);
    min1_2 = min2( min_02_12_opt, load2(X, i+1, j0));

    min_03_13_opt = min2(load2(X, i-1, j0+1), load2(X, i, j0+1));
    min0_3 = min2( load2(X, i-2, j0+1), min_03_13_opt);
    min1_3 = min2( min_03_13_opt, load2(X, i+1, j0+1));

    min_10_20_opt = min2(load2(X, i, j0-2), load2(X, i+1, j0-2));
    min1_0 = min2(load2(X, i-1, j0-2) , min_10_20_opt);
    min2_0 = min2(min_10_20_opt, load2(X, i+2, j0-2));

    min_11_21_opt = min2(load2(X, i, j0-1), load2(X, i+1, j0-1));
    min1_1 = min2(load2(X, i-1, j0-1) , min_11_21_opt);
    min2_1 = min2(min_11_21_opt, load2(X, i+2, j0-1));

    min_12_22_opt = min2(load2(X, i, j0), load2(X, i+1, j0));
    min1_2 = min2(load2(X, i-1, j0), min_12_22_opt);
    min2_2 = min2(min_12_22_opt, load2(X, i+2, j0));

    min_13_23_opt = min2(load2(X, i, j0+1), load2(X, i+1, j0+1));
    min1_3 = min2(load2(X, i-1, j0+1), min_13_23_opt);
    min2_3 = min2(min_13_23_opt, load2(X, i+2, j0+1));

    min_20_30_opt = min2(load2(X, i+1, j0-2), load2(X, i+2, j0-2));
    min2_0 = min2(load2(X, i, j0-2), min_20_30_opt);
    min3_0 = min2(min_20_30_opt, load2(X, i+3, j0-2));

    min_21_31_opt = min2(load2(X, i+1, j0-1), load2(X, i+2, j0-1));
    min2_1 = min2(load2(X, i, j0-1), min_21_31_opt);
    min3_1 = min2(min_21_31_opt, load2(X, i+3, j0-1));

    min_22_32_opt = min2(load2(X, i+1, j0), load2(X, i+2, j0));
    min2_2 = min2(load2(X, i, j0), min_22_32_opt);
    min3_2 = min2(min_22_32_opt, load2(X, i+3, j0));

    min_23_33_opt = min2(load2(X, i+1, j0+1), load2(X, i+2, j0+1));
    min2_3 = min2(load2(X, i, j0+1), min_23_33_opt);
    min3_3 = min2( min_23_33_opt, load2(X, i+3, j0+1));

    min_all0_opt = min2(min0_1, min0_2);
    min_all0_0 = min2( min0_0, min_all0_opt);
    min_all0_1 = min2( min_all0_opt, min0_3);

    min_all1_opt = min2(min1_1, min1_2);
    min_all1_0 = min2( min1_0, min_all1_opt);
    min_all1_1 = min2( min_all1_opt, min1_3);

    min_all2_opt = min2(min2_1, min2_2);
    min_all2_0 = min2( min2_0, min_all2_opt );
    min_all2_1 = min2( min_all2_opt , min2_3);

    min_all3_opt = min2(min3_1, min3_2);
    min_all3_0 = min2( min3_0, min_all3_opt);
    min_all3_1 = min2( min_all3_opt, min3_3);

    min_02_03 = min2(min0_2, min0_3);
    min_12_13 = min2(min1_2, min1_3);
    min_22_23 = min2(min2_2, min2_3);
    min_32_33 = min2(min3_2, min3_3);

    max_all_00_01 = max2(min_all0_0, min_all0_1);
    max_all_10_11 = max2(min_all1_0, min_all1_1);
    max_all_20_21 = max2(min_all2_0, min_all2_1);
    max_all_30_31 = max2(min_all3_0, min_all3_1);

    uint8 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2, load_ip3_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+2);
            load_im2_jp2 = load2(X, i-2, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            load_ip3_jp2 = load2(X, i+3, j+2);

            min_04_14_opt = min2(load_im1_jp2, load_i_jp2);
            min0_4 = min2(load_im2_jp2, min_04_14_opt);
            min1_4 = min2( min_04_14_opt, load_ip1_jp2);

            min_24_opt = min2(load_i_jp2, load_ip1_jp2);
            min2_4 = min2(min_24_opt, load_ip2_jp2);

            min_34_opt = min2(load_ip1_jp2, load_ip2_jp2);
            min3_4 = min2(min_34_opt, load_ip3_jp2);

            min_all0_2 = min2( min_02_03, min0_4);
            min_all1_2 = min2( min_12_13, min1_4);
            min_all2_2 = min2( min_22_23, min2_4);
            min_all3_2 = min2( min_32_33, min3_4);

            max_all0 = max2( max_all_00_01, min_all0_2);
            max_all1 = max2( max_all_10_11, min_all1_2);
            max_all2 = max2( max_all_20_21, min_all2_2);
            max_all3 = max2( max_all_30_31, min_all3_2);

            max_all12 = max2(max_all1, max_all2);

            store2( Y, i, j, max2( max_all0, max_all12) );
            store2( Y, i+1, j, max2( max_all12, max_all3) );

            // min_all0_opt = min2(min0_2, min0_3);
            // min_all0_0 = min2( min0_1, min_all0_opt);
            // min_all0_1 = min2( min_all0_opt, min0_4);

            // min_all1_opt = min2(min1_2, min1_3);
            // min_all1_0 = min2( min1_1, min_all1_opt);
            // min_all1_1 = min2( min_all1_opt, min1_4);

            // min_all2_opt = min2(min2_2, min2_3);
            // min_all2_0 = min2( min2_1, min_all2_opt);
            // min_all2_1 = min2( min_all2_opt, min2_4);

            // min_all3_opt = min2(min3_2, min3_3);
            // min_all3_0 = min2( min3_1, min_all3_opt);
            // min_all3_1 = min2( min_all3_opt, min3_4);


            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+3);
            load_im2_jp2 = load2(X, i-2, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);
            load_ip3_jp2 = load2(X, i+3, j+3);

            min_04_14_opt = min2(load_im1_jp2, load_i_jp2);
            min0_0 = min2(load_im2_jp2, min_04_14_opt);
            min1_0 = min2( min_04_14_opt, load_ip1_jp2);

            min_24_opt = min2(load_i_jp2, load_ip1_jp2);
            min2_0 = min2(min_24_opt, load_ip2_jp2);

            min_34_opt = min2(load_ip1_jp2, load_ip2_jp2);
            min3_0 = min2(min_34_opt, load_ip3_jp2);

            min_02_03 = min2(min0_3, min0_4);
            min_12_13 = min2(min1_3, min1_4);
            min_22_23 = min2(min2_3, min2_4);
            min_32_33 = min2(min3_3, min3_4);

            min_all0_0 = min2( min_02_03, min0_0);
            min_all1_0 = min2( min_12_13, min1_0);
            min_all2_0 = min2( min_22_23, min2_0);
            min_all3_0 = min2( min_32_33, min3_0);

            max_all_00_01 = max2(min_all0_1, min_all0_2);
            max_all_10_11 = max2(min_all1_1, min_all1_2);
            max_all_20_21 = max2(min_all2_1, min_all2_2);
            max_all_30_31 = max2(min_all3_1, min_all3_2);

            max_all0 = max2( max_all_00_01, min_all0_0);
            max_all1 = max2( max_all_10_11, min_all1_0);
            max_all2 = max2( max_all_20_21, min_all2_0);
            max_all3 = max2( max_all_30_31, min_all3_0);

            max_all12 = max2(max_all1, max_all2);

            store2( Y, i, j+1, max2( max_all0, max_all12) );
            store2( Y, i+1, j+1, max2( max_all12, max_all3) );

            // min_all0_opt = min2(min0_3, min0_4);
            // min_all0_0 = min2( min0_2, min_all0_opt);
            // min_all0_1 = min2( min_all0_opt, min0_0);

            // min_all1_opt = min2(min1_3, min1_4);
            // min_all1_0 = min2( min1_2, min_all1_opt);
            // min_all1_1 = min2( min_all1_opt, min1_0);

            // min_all2_opt = min2(min2_3, min2_4);
            // min_all2_0 = min2( min2_2, min_all2_opt);
            // min_all2_1 = min2( min_all2_opt, min2_0);

            // min_all3_opt = min2(min3_3, min3_4);
            // min_all3_0 = min2( min3_2, min_all3_opt);
            // min_all3_1 = min2( min_all3_opt, min3_0);

            // min_02_03 = min2(min0_4, min0_0);
            // min_12_13 = min2(min1_4, min1_0);
            // min_22_23 = min2(min2_4, min2_0);
            // min_32_33 = min2(min3_4, min3_0);

            // max_all_00_01 = max2(min_all0_0, min_all0_1);
            // max_all_10_11 = max2(min_all1_0, min_all1_1);
            // max_all_20_21 = max2(min_all2_0, min_all2_1);
            // max_all_30_31 = max2(min_all3_0, min_all3_1);

            // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+4);
            load_im2_jp2 = load2(X, i-2, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);
            load_ip3_jp2 = load2(X, i+3, j+4);

            min_04_14_opt = min2(load_im1_jp2, load_i_jp2);
            min0_1 = min2(load_im2_jp2, min_04_14_opt);
            min1_1 = min2( min_04_14_opt, load_ip1_jp2);

            min_24_opt = min2(load_i_jp2, load_ip1_jp2);
            min2_1 = min2(min_24_opt, load_ip2_jp2);

            min_34_opt = min2(load_ip1_jp2, load_ip2_jp2);
            min3_1 = min2(min_34_opt, load_ip3_jp2);
            
            min_02_03 = min2(min0_4, min0_0);
            min_12_13 = min2(min1_4, min1_0);
            min_22_23 = min2(min2_4, min2_0);
            min_32_33 = min2(min3_4, min3_0);

            min_all0_1 = min2( min_02_03, min0_1);
            min_all1_1 = min2( min_12_13, min1_1);
            min_all2_1 = min2( min_22_23, min2_1);
            min_all3_1 = min2( min_32_33, min3_1);

            max_all_00_01 = max2(min_all0_2, min_all0_0);
            max_all_10_11 = max2(min_all1_2, min_all1_0);
            max_all_20_21 = max2(min_all2_2, min_all2_0);
            max_all_30_31 = max2(min_all3_2, min_all3_0);

            max_all0 = max2( max_all_00_01, min_all0_1);
            max_all1 = max2( max_all_10_11, min_all1_1);
            max_all2 = max2( max_all_20_21, min_all2_1);
            max_all3 = max2( max_all_30_31, min_all3_1);

            max_all12 = max2(max_all1, max_all2);

            store2( Y, i, j+2, max2( max_all0, max_all12) );
            store2( Y, i+1, j+2, max2( max_all12, max_all3) );

            // min_all0_opt = min2(min0_4, min0_0);
            // min_all0_0 = min2( min0_3, min_all0_opt);
            // min_all0_1 = min2( min_all0_opt, min0_1);

            // min_all1_opt = min2(min1_4, min1_0);
            // min_all1_0 = min2( min1_3, min_all1_opt);
            // min_all1_1 = min2( min_all1_opt, min1_1);

            // min_all2_opt = min2(min2_4, min2_0);
            // min_all2_0 = min2( min2_3, min_all2_opt);
            // min_all2_1 = min2( min_all2_opt, min2_1);

            // min_all3_opt = min2(min3_4, min3_0);
            // min_all3_0 = min2( min3_3, min_all3_opt);
            // min_all3_1 = min2( min_all3_opt, min3_1);

            // min_02_03 = min2(min0_0, min0_1);
            // min_12_13 = min2(min1_0, min1_1);
            // min_22_23 = min2(min2_0, min2_1);
            // min_32_33 = min2(min3_0, min3_1);

            // max_all_00_01 = max2(min_all0_0, min_all0_1);
            // max_all_10_11 = max2(min_all1_0, min_all1_1);
            // max_all_20_21 = max2(min_all2_0, min_all2_1);
            // max_all_30_31 = max2(min_all3_0, min_all3_1);

           // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+5);
            load_im2_jp2 = load2(X, i-2, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            load_ip3_jp2 = load2(X, i+3, j+5);

            min_04_14_opt = min2(load_im1_jp2, load_i_jp2);
            min0_2 = min2(load_im2_jp2, min_04_14_opt);
            min1_2 = min2( min_04_14_opt, load_ip1_jp2);

            min_24_opt = min2(load_i_jp2, load_ip1_jp2);
            min2_2 = min2(min_24_opt, load_ip2_jp2);

            min_34_opt = min2(load_ip1_jp2, load_ip2_jp2);
            min3_2 = min2(min_34_opt, load_ip3_jp2);

            min_02_03 = min2(min0_0, min0_1);
            min_12_13 = min2(min1_0, min1_1);
            min_22_23 = min2(min2_0, min2_1);
            min_32_33 = min2(min3_0, min3_1);

            min_all0_2 = min2( min_02_03, min0_2);
            min_all1_2 = min2( min_12_13, min1_2);
            min_all2_2 = min2( min_22_23, min2_2);
            min_all3_2 = min2( min_32_33, min3_2);

            max_all_00_01 = max2(min_all0_0, min_all0_1);
            max_all_10_11 = max2(min_all1_0, min_all1_1);
            max_all_20_21 = max2(min_all2_0, min_all2_1);
            max_all_30_31 = max2(min_all3_0, min_all3_1);

            max_all0 = max2( max_all_00_01, min_all0_2);
            max_all1 = max2( max_all_10_11, min_all1_2);
            max_all2 = max2( max_all_20_21, min_all2_2);
            max_all3 = max2( max_all_30_31, min_all3_2);

            max_all12 = max2(max_all1, max_all2);

            store2( Y, i, j+3, max2( max_all0, max_all12) );
            store2( Y, i+1, j+3, max2( max_all12, max_all3) );

            // min_all0_opt = min2(min0_0, min0_1);
            // min_all0_0 = min2( min0_4, min_all0_opt);
            // min_all0_1 = min2( min_all0_opt, min0_2);

            // min_all1_opt = min2(min1_0, min1_1);
            // min_all1_0 = min2( min1_4, min_all1_opt);
            // min_all1_1 = min2( min_all1_opt, min1_2);

            // min_all2_opt = min2(min2_0, min2_1);
            // min_all2_0 = min2( min2_4, min_all2_opt);
            // min_all2_1 = min2( min_all2_opt, min2_2);

            // min_all3_opt = min2(min3_0, min3_1);
            // min_all3_0 = min2( min3_4, min_all3_opt);
            // min_all3_1 = min2( min_all3_opt, min3_2);

            // min_02_03 = min2(min0_1, min0_2);
            // min_12_13 = min2(min1_1, min1_2);
            // min_22_23 = min2(min2_1, min2_2);
            // min_32_33 = min2(min3_1, min3_2);

            // max_all_00_01 = max2(min_all0_0, min_all0_1);
            // max_all_10_11 = max2(min_all1_0, min_all1_1);
            // max_all_20_21 = max2(min_all2_0, min_all2_1);
            // max_all_30_31 = max2(min_all3_0, min_all3_1);

           // ---------------------------------------------------------------------------------------------
            load_im1_jp2 = load2(X, i-1, j+6);
            load_im2_jp2 = load2(X, i-2, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            load_ip3_jp2 = load2(X, i+3, j+6);

            min_04_14_opt = min2(load_im1_jp2, load_i_jp2);
            min0_3 = min2(load_im2_jp2, min_04_14_opt);
            min1_3 = min2( min_04_14_opt, load_ip1_jp2);

            min_24_opt = min2(load_i_jp2, load_ip1_jp2);
            min2_3 = min2(min_24_opt, load_ip2_jp2);

            min_34_opt = min2(load_ip1_jp2, load_ip2_jp2);
            min3_3 = min2(min_34_opt, load_ip3_jp2);

            min_02_03 = min2(min0_1, min0_2);
            min_12_13 = min2(min1_1, min1_2);
            min_22_23 = min2(min2_1, min2_2);
            min_32_33 = min2(min3_1, min3_2);

            min_all0_0 = min2( min_02_03, min0_3);
            min_all1_0 = min2( min_12_13, min1_3);
            min_all2_0 = min2( min_22_23, min2_3);
            min_all3_0 = min2( min_32_33, min3_3);

            max_all_00_01 = max2(min_all0_1, min_all0_2);
            max_all_10_11 = max2(min_all1_1, min_all1_2);
            max_all_20_21 = max2(min_all2_1, min_all2_2);
            max_all_30_31 = max2(min_all3_1, min_all3_2);

            max_all0 = max2( max_all_00_01, min_all0_0);
            max_all1 = max2( max_all_10_11, min_all1_0);
            max_all2 = max2( max_all_20_21, min_all2_0);
            max_all3 = max2( max_all_30_31, min_all3_0);

            max_all12 = max2(max_all1, max_all2);

            store2( Y, i, j+4, max2( max_all0, max_all12) );
            store2( Y, i+1, j+4, max2( max_all12, max_all3) );

            // min_all0_opt = min2(min0_1, min0_2);
            // min_all0_0 = min2( min0_0, min_all0_opt);
            // min_all0_1 = min2( min_all0_opt, min0_3);

            // min_all1_opt = min2(min1_1, min1_2);
            // min_all1_0 = min2( min1_0, min_all1_opt);
            // min_all1_1 = min2( min_all1_opt, min1_3);

            // min_all2_opt = min2(min2_1, min2_2);
            // min_all2_0 = min2( min2_0, min_all2_opt);
            // min_all2_1 = min2( min_all2_opt, min2_3);

            // min_all3_opt = min2(min3_1, min3_2);
            // min_all3_0 = min2( min3_0, min_all3_opt);
            // min_all3_1 = min2( min_all3_opt, min3_3);

            min_02_03 = min2(min0_2, min0_3);
            min_12_13 = min2(min1_2, min1_3);
            min_22_23 = min2(min2_2, min2_3);
            min_32_33 = min2(min3_2, min3_3);

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;
            min_all3_1 = min_all3_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
            min_all3_0 = min_all3_2;

            max_all_00_01 = max2(min_all0_0, min_all0_1);
            max_all_10_11 = max2(min_all1_0, min_all1_1);
            max_all_20_21 = max2(min_all2_0, min_all2_1);
            max_all_30_31 = max2(min_all3_0, min_all3_1);
    }
    int r = (j1-j0-1) % 5;
    line_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_ouverture3_ui8matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
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
            // ---------------------------------------------------------------------------------------------
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
            
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
            // ---------------------------------------------------------------------------------------------
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
        // FUSION COMPLETE
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y, uint8 **Z)
{
    min3_ui8matrix_basic(X, i0-1, i1+1, j0-1, j1+1, Y);
    max3_ui8matrix_basic(Y, i0,   i1,   j0,   j1,   Z);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion_ilu5_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1-2; i += 2){
        line_ouverture3_ui8matrix_fusion_ilu5_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    line_ouverture3_ui8matrix_fusion(X, i1-r-1, j0, j1, Y);
    line_ouverture3_ui8matrix_fusion(X, i1-r, j0, j1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1-2; i += 2){
        line_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    line_ouverture3_ui8matrix_fusion(X, i1-r-1, j0, j1, Y);
    line_ouverture3_ui8matrix_fusion(X, i1-r, j0, j1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_fusion_ilu15_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_ouverture3_ui8matrix_fusion_ilu15_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------


        // FUSION SUR LIGNE SWP8
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui8matrix_fusion                     (uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8   min_haut, min_milieu, min_bas,
            min_gauche, min_droit,
            min_all1_1, min_all2_1, min_all3_1,
            min_all1_2, min_all2_2, min_all3_2,
            min_all1_3, min_all2_3, min_all3_3,
            max_all1, max_all2, max_all3,

            l,  r, res;

    for(int j = j0; j < j1; j++){

        min_gauche = min3( load2(X, i-2, j-2), load2(X, i-1, j-2), load2(X, i, j-2));
        min_milieu = min3(load2(X, i-2, j-1),load2(X, i-1, j-1),load2(X, i, j-1));
        min_droit = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all1_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j-1), load2(X, i-1, j-1), load2(X, i, j-1));
        min_milieu = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));
        min_droit = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));

        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all2_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j), load2(X, i-1, j), load2(X, i, j));
        min_milieu = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));
        min_droit = min3(load2(X, i-2, j+2),load2(X, i-1, j+2),load2(X, i, j+2));

        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all3_1 = min3(l, min_milieu, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-2), load2(X, i, j-2), load2(X, i+1, j-2));
        min_milieu = min3(load2(X, i-1, j-1),load2(X, i, j-1),load2(X, i+1, j-1));
        min_droit = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));

        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all1_2 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-1), load2(X, i, j-1), load2(X, i+1, j-1));
        min_milieu = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));
        min_droit = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all2_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j), load2(X, i, j), load2(X, i+1, j));
        min_milieu = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));
        min_droit = min3(load2(X, i-1, j+2),load2(X, i, j+2),load2(X, i+1, j+2));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all3_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-2), load2(X, i+1, j-2), load2(X, i+2, j-2));
        min_milieu = min3(load2(X, i, j-1),load2(X, i+1, j-1),load2(X, i+2, j-1));
        min_droit = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all1_3 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-1), load2(X, i+1, j-1), load2(X, i+2, j-1));
        min_milieu = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));
        min_droit = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all2_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3(load2(X, i, j), load2(X, i+1, j), load2(X, i+2, j));
        min_milieu = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));
        min_droit = min3(load2(X, i, j+2),load2(X, i+1, j+2),load2(X, i+2, j+2));


        l = i8left(min_gauche, min_milieu);
        r = i8right(min_milieu, min_droit);
        min_all3_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        max_all1 = max3(min_all1_1, min_all1_2, min_all1_3);
        max_all2 = max3(min_all2_1, min_all2_2, min_all2_3);
        max_all3 = max3(min_all3_1, min_all3_2, min_all3_3);


        l = i8left(max_all1, max_all2);
        r = i8right(max_all2,max_all3);
        res = max3( l, max_all2, r);
        store2( Y, i, j,res);
    }
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui8matrix_fusion_ilu5_red            (uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            max_all0, max_all1, max_all2,

            l, r;

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

    l = i8left(min0_0, min0_1);
    r = i8right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i8left(min0_1, min0_2);
    r = i8right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i8left(min1_0, min1_1);
    r = i8right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i8left(min1_1, min1_2);
    r = i8right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i8left(min2_0, min2_1);
    r = i8right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i8left(min2_1, min2_2);
    r = i8right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    uint8 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            
            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i8left(min0_2, min0_3);
            r = i8right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i8left(min1_2, min1_3);
            r = i8right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i8left(min2_2, min2_3);
            r = i8right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);


            l = i8left(min0_3, min0_4);
            r = i8right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i8left(min1_3, min1_4);
            r = i8right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i8left(min2_3, min2_4);
            r = i8right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i8left( max_all0, max_all1);
            r = i8right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i8left(min0_4, min0_0);
            r = i8right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i8left(min1_4, min1_0);
            r = i8right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i8left(min2_4, min2_0);
            r = i8right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i8left(min0_0, min0_1);
            r = i8right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i8left(min1_0, min1_1);
            r = i8right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i8left(min2_0, min2_1);
            r = i8right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );


            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i8left(min0_1, min0_2);
            r = i8right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i8left(min1_1, min1_2);
            r = i8right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i8left(min2_1, min2_2);
            r = i8right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui8matrix_fusion_ilu5_elu2_red       (uint8 **X, int i, int j0, int j1, uint8 **Y)
{

    uint8  min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min3_0, min3_1, min3_2, min3_3, min3_4,

            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            min_all3_0, min_all3_1, min_all3_2,
            max_all0, max_all1, max_all2, max_all3,max_all4, max_all5,

            l, r;

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

    l = i8left(min0_0, min0_1);
    r = i8right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i8left(min0_1, min0_2);
    r = i8right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i8left(min1_0, min1_1);
    r = i8right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i8left(min1_1, min1_2);
    r = i8right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i8left(min2_0, min2_1);
    r = i8right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i8left(min2_1, min2_2);
    r = i8right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    l = i8left(min3_0, min3_1);
    r = i8right(min3_1, min3_2);    
    min_all3_0 = min3( l, min3_1, r);

    l = i8left(min3_1, min3_2);
    r = i8right(min3_2, min3_3);    
    min_all3_1 = min3( l , min3_2 , r);

    uint8 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2,load_ip3_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            load_ip3_jp2 = load2(X, i+3, j+2);

            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_4 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i8left(min0_2, min0_3);
            r = i8right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i8left(min1_2, min1_3);
            r = i8right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i8left(min2_2, min2_3);
            r = i8right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);

            l = i8left(min3_2, min3_3);
            r = i8right(min3_3, min3_4);
            min_all3_2 = min3( l, min3_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);
            
            max_all3 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);
            store2( Y, i, j, max3( l, max_all1, r) );

            l = i8left(max_all3, max_all4);
            r = i8right(max_all4, max_all5);
            store2( Y, i+1, j, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);
            load_ip3_jp2 = load2(X, i+3, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_0 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i8left(min0_3, min0_4);
            r = i8right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i8left(min1_3, min1_4);
            r = i8right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i8left(min2_3, min2_4);
            r = i8right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);

            l = i8left(min3_3, min3_4);
            r = i8right(min3_4, min3_0);
            min_all3_0 = min3( l, min3_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);
            
            max_all3 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3( min_all1_0, min_all2_0, min_all3_0);
            

            l = i8left( max_all0, max_all1);
            r = i8right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            l = i8left( max_all3, max_all4);
            r = i8right(max_all4, max_all5);
            store2( Y, i+1, j+1, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);
            load_ip3_jp2 = load2(X, i+3, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_1 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i8left(min0_4, min0_0);
            r = i8right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i8left(min1_4, min1_0);
            r = i8right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i8left(min2_4, min2_0);
            r = i8right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);

            l = i8left(min3_4, min3_0);
            r = i8right(min3_0, min3_1);
            min_all3_1 = min3( l, min3_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            max_all3 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all4 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all5 = max3( min_all1_1, min_all2_1, min_all3_1);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            l = i8left( max_all3, max_all4);
            r = i8right(max_all4, max_all5);
            store2( Y, i+1, j+2, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------
            
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            load_ip3_jp2 = load2(X, i+3, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_2 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i8left(min0_0, min0_1);
            r = i8right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i8left(min1_0, min1_1);
            r = i8right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i8left(min2_0, min2_1);
            r = i8right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);

            l = i8left(min3_0, min3_1);
            r = i8right(min3_1, min3_2);
            min_all3_2 = min3( l, min3_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            max_all3 = max3(min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);


            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );
         
            l = i8left( max_all3, max_all4);
            r = i8right(max_all4, max_all5);
            store2( Y, i+1, j+3, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            load_ip3_jp2 = load2(X, i+3, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_3 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i8left(min0_1, min0_2);
            r = i8right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i8left(min1_1, min1_2);
            r = i8right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i8left(min2_1, min2_2);
            r = i8right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);

            l = i8left(min3_1, min3_2);
            r = i8right(min3_2, min3_3);
            min_all3_0 = min3( l, min3_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            max_all3 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3(min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3(min_all1_0, min_all2_0, min_all3_0);

            l = i8left(max_all0, max_all1);
            r = i8right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );

            l = i8left(max_all3, max_all4);
            r = i8right(max_all4, max_all5);

            store2( Y, i+1, j+4, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;
            min_all3_1 = min_all3_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
            min_all3_0 = min_all3_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui8matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_swp_ouverture3_ui8matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);


}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
}
// ---------------------------------------------------------------------------------------------


        // FUSION SUR LIGNE SWP16
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui16matrix_fusion                     (uint16 **X, int i, int j0, int j1, uint16 **Y)
{
    uint16   min_haut, min_milieu, min_bas,
            min_gauche, min_droit,
            min_all1_1, min_all2_1, min_all3_1,
            min_all1_2, min_all2_2, min_all3_2,
            min_all1_3, min_all2_3, min_all3_3,
            max_all1, max_all2, max_all3,

            l,  r, res;

    for(int j = j0; j < j1; j++){

        min_gauche = min3( load2(X, i-2, j-2), load2(X, i-1, j-2), load2(X, i, j-2));
        min_milieu = min3(load2(X, i-2, j-1),load2(X, i-1, j-1),load2(X, i, j-1));
        min_droit = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all1_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j-1), load2(X, i-1, j-1), load2(X, i, j-1));
        min_milieu = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));
        min_droit = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));

        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all2_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j), load2(X, i-1, j), load2(X, i, j));
        min_milieu = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));
        min_droit = min3(load2(X, i-2, j+2),load2(X, i-1, j+2),load2(X, i, j+2));

        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all3_1 = min3(l, min_milieu, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-2), load2(X, i, j-2), load2(X, i+1, j-2));
        min_milieu = min3(load2(X, i-1, j-1),load2(X, i, j-1),load2(X, i+1, j-1));
        min_droit = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));

        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all1_2 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-1), load2(X, i, j-1), load2(X, i+1, j-1));
        min_milieu = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));
        min_droit = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all2_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j), load2(X, i, j), load2(X, i+1, j));
        min_milieu = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));
        min_droit = min3(load2(X, i-1, j+2),load2(X, i, j+2),load2(X, i+1, j+2));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all3_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-2), load2(X, i+1, j-2), load2(X, i+2, j-2));
        min_milieu = min3(load2(X, i, j-1),load2(X, i+1, j-1),load2(X, i+2, j-1));
        min_droit = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all1_3 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-1), load2(X, i+1, j-1), load2(X, i+2, j-1));
        min_milieu = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));
        min_droit = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all2_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3(load2(X, i, j), load2(X, i+1, j), load2(X, i+2, j));
        min_milieu = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));
        min_droit = min3(load2(X, i, j+2),load2(X, i+1, j+2),load2(X, i+2, j+2));


        l = i16left(min_gauche, min_milieu);
        r = i16right(min_milieu, min_droit);
        min_all3_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        max_all1 = max3(min_all1_1, min_all1_2, min_all1_3);
        max_all2 = max3(min_all2_1, min_all2_2, min_all2_3);
        max_all3 = max3(min_all3_1, min_all3_2, min_all3_3);


        l = i16left(max_all1, max_all2);
        r = i16right(max_all2,max_all3);
        res = max3( l, max_all2, r);
        store2( Y, i, j,res);
    }
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui16matrix_fusion_ilu5_red            (uint16 **X, int i, int j0, int j1, uint16 **Y)
{
    uint16   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            max_all0, max_all1, max_all2,

            l, r;

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

    l = i16left(min0_0, min0_1);
    r = i16right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i16left(min0_1, min0_2);
    r = i16right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i16left(min1_0, min1_1);
    r = i16right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i16left(min1_1, min1_2);
    r = i16right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i16left(min2_0, min2_1);
    r = i16right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i16left(min2_1, min2_2);
    r = i16right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    uint16 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            
            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i16left(min0_2, min0_3);
            r = i16right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i16left(min1_2, min1_3);
            r = i16right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i16left(min2_2, min2_3);
            r = i16right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);


            l = i16left(min0_3, min0_4);
            r = i16right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i16left(min1_3, min1_4);
            r = i16right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i16left(min2_3, min2_4);
            r = i16right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i16left( max_all0, max_all1);
            r = i16right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i16left(min0_4, min0_0);
            r = i16right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i16left(min1_4, min1_0);
            r = i16right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i16left(min2_4, min2_0);
            r = i16right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i16left(min0_0, min0_1);
            r = i16right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i16left(min1_0, min1_1);
            r = i16right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i16left(min2_0, min2_1);
            r = i16right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );


            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i16left(min0_1, min0_2);
            r = i16right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i16left(min1_1, min1_2);
            r = i16right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i16left(min2_1, min2_2);
            r = i16right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui16matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui16matrix_fusion_ilu5_elu2_red       (uint16 **X, int i, int j0, int j1, uint16 **Y)
{

    uint16  min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min3_0, min3_1, min3_2, min3_3, min3_4,

            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            min_all3_0, min_all3_1, min_all3_2,
            max_all0, max_all1, max_all2, max_all3,max_all4, max_all5,

            l, r;

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

    l = i16left(min0_0, min0_1);
    r = i16right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i16left(min0_1, min0_2);
    r = i16right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i16left(min1_0, min1_1);
    r = i16right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i16left(min1_1, min1_2);
    r = i16right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i16left(min2_0, min2_1);
    r = i16right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i16left(min2_1, min2_2);
    r = i16right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    l = i16left(min3_0, min3_1);
    r = i16right(min3_1, min3_2);    
    min_all3_0 = min3( l, min3_1, r);

    l = i16left(min3_1, min3_2);
    r = i16right(min3_2, min3_3);    
    min_all3_1 = min3( l , min3_2 , r);

    uint16 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2,load_ip3_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            load_ip3_jp2 = load2(X, i+3, j+2);

            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_4 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i16left(min0_2, min0_3);
            r = i16right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i16left(min1_2, min1_3);
            r = i16right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i16left(min2_2, min2_3);
            r = i16right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);

            l = i16left(min3_2, min3_3);
            r = i16right(min3_3, min3_4);
            min_all3_2 = min3( l, min3_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);
            
            max_all3 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);
            store2( Y, i, j, max3( l, max_all1, r) );

            l = i16left(max_all3, max_all4);
            r = i16right(max_all4, max_all5);
            store2( Y, i+1, j, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);
            load_ip3_jp2 = load2(X, i+3, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_0 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i16left(min0_3, min0_4);
            r = i16right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i16left(min1_3, min1_4);
            r = i16right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i16left(min2_3, min2_4);
            r = i16right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);

            l = i16left(min3_3, min3_4);
            r = i16right(min3_4, min3_0);
            min_all3_0 = min3( l, min3_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);
            
            max_all3 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3( min_all1_0, min_all2_0, min_all3_0);
            

            l = i16left( max_all0, max_all1);
            r = i16right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            l = i16left( max_all3, max_all4);
            r = i16right(max_all4, max_all5);
            store2( Y, i+1, j+1, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);
            load_ip3_jp2 = load2(X, i+3, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_1 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i16left(min0_4, min0_0);
            r = i16right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i16left(min1_4, min1_0);
            r = i16right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i16left(min2_4, min2_0);
            r = i16right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);

            l = i16left(min3_4, min3_0);
            r = i16right(min3_0, min3_1);
            min_all3_1 = min3( l, min3_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            max_all3 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all4 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all5 = max3( min_all1_1, min_all2_1, min_all3_1);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            l = i16left( max_all3, max_all4);
            r = i16right(max_all4, max_all5);
            store2( Y, i+1, j+2, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------
            
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            load_ip3_jp2 = load2(X, i+3, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_2 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i16left(min0_0, min0_1);
            r = i16right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i16left(min1_0, min1_1);
            r = i16right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i16left(min2_0, min2_1);
            r = i16right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);

            l = i16left(min3_0, min3_1);
            r = i16right(min3_1, min3_2);
            min_all3_2 = min3( l, min3_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            max_all3 = max3(min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);


            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );
         
            l = i16left( max_all3, max_all4);
            r = i16right(max_all4, max_all5);
            store2( Y, i+1, j+3, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            load_ip3_jp2 = load2(X, i+3, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_3 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i16left(min0_1, min0_2);
            r = i16right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i16left(min1_1, min1_2);
            r = i16right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i16left(min2_1, min2_2);
            r = i16right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);

            l = i16left(min3_1, min3_2);
            r = i16right(min3_2, min3_3);
            min_all3_0 = min3( l, min3_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            max_all3 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3(min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3(min_all1_0, min_all2_0, min_all3_0);

            l = i16left(max_all0, max_all1);
            r = i16right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );

            l = i16left(max_all3, max_all4);
            r = i16right(max_all4, max_all5);

            store2( Y, i+1, j+4, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;
            min_all3_1 = min_all3_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
            min_all3_0 = min_all3_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui16matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_swp_ouverture3_ui16matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);


}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui16matrix_fusion_ilu5_elu2_red_factor(uint16 **X, int i, int j0, int j1, uint16 **Y)
{
}
// ---------------------------------------------------------------------------------------------



        // FUSION SUR LIGNE SWP32
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui32matrix_fusion                     (uint32 **X, int i, int j0, int j1, uint32 **Y)
{
    uint32   min_haut, min_milieu, min_bas,
            min_gauche, min_droit,
            min_all1_1, min_all2_1, min_all3_1,
            min_all1_2, min_all2_2, min_all3_2,
            min_all1_3, min_all2_3, min_all3_3,
            max_all1, max_all2, max_all3,

            l,  r, res;

    for(int j = j0; j < j1; j++){

        min_gauche = min3( load2(X, i-2, j-2), load2(X, i-1, j-2), load2(X, i, j-2));
        min_milieu = min3(load2(X, i-2, j-1),load2(X, i-1, j-1),load2(X, i, j-1));
        min_droit = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all1_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j-1), load2(X, i-1, j-1), load2(X, i, j-1));
        min_milieu = min3(load2(X, i-2, j),load2(X, i-1, j),load2(X, i, j));
        min_droit = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));

        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all2_1 = min3(l, min_milieu, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-2, j), load2(X, i-1, j), load2(X, i, j));
        min_milieu = min3(load2(X, i-2, j+1),load2(X, i-1, j+1),load2(X, i, j+1));
        min_droit = min3(load2(X, i-2, j+2),load2(X, i-1, j+2),load2(X, i, j+2));

        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all3_1 = min3(l, min_milieu, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-2), load2(X, i, j-2), load2(X, i+1, j-2));
        min_milieu = min3(load2(X, i-1, j-1),load2(X, i, j-1),load2(X, i+1, j-1));
        min_droit = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));

        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all1_2 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j-1), load2(X, i, j-1), load2(X, i+1, j-1));
        min_milieu = min3(load2(X, i-1, j),load2(X, i, j),load2(X, i+1, j));
        min_droit = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all2_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i-1, j), load2(X, i, j), load2(X, i+1, j));
        min_milieu = min3(load2(X, i-1, j+1),load2(X, i, j+1),load2(X, i+1, j+1));
        min_droit = min3(load2(X, i-1, j+2),load2(X, i, j+2),load2(X, i+1, j+2));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all3_2 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-2), load2(X, i+1, j-2), load2(X, i+2, j-2));
        min_milieu = min3(load2(X, i, j-1),load2(X, i+1, j-1),load2(X, i+2, j-1));
        min_droit = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all1_3 = min3( min_milieu, l, r);

        // ---------------------------------------------------------------------------------------------

        min_gauche = min3( load2(X, i, j-1), load2(X, i+1, j-1), load2(X, i+2, j-1));
        min_milieu = min3(load2(X, i, j),load2(X, i+1, j),load2(X, i+2, j));
        min_droit = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all2_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        min_gauche = min3(load2(X, i, j), load2(X, i+1, j), load2(X, i+2, j));
        min_milieu = min3(load2(X, i, j+1),load2(X, i+1, j+1),load2(X, i+2, j+1));
        min_droit = min3(load2(X, i, j+2),load2(X, i+1, j+2),load2(X, i+2, j+2));


        l = i32left(min_gauche, min_milieu);
        r = i32right(min_milieu, min_droit);
        min_all3_3 = min3( min_milieu, l, r);
        // ---------------------------------------------------------------------------------------------

        max_all1 = max3(min_all1_1, min_all1_2, min_all1_3);
        max_all2 = max3(min_all2_1, min_all2_2, min_all2_3);
        max_all3 = max3(min_all3_1, min_all3_2, min_all3_3);


        l = i32left(max_all1, max_all2);
        r = i32right(max_all2,max_all3);
        res = max3( l, max_all2, r);
        store2( Y, i, j,res);
    }
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui32matrix_fusion_ilu5_red            (uint32 **X, int i, int j0, int j1, uint32 **Y)
{
    uint32   min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            max_all0, max_all1, max_all2,

            l, r;

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

    l = i32left(min0_0, min0_1);
    r = i32right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i32left(min0_1, min0_2);
    r = i32right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i32left(min1_0, min1_1);
    r = i32right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i32left(min1_1, min1_2);
    r = i32right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i32left(min2_0, min2_1);
    r = i32right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i32left(min2_1, min2_2);
    r = i32right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    uint32 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            
            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i32left(min0_2, min0_3);
            r = i32right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i32left(min1_2, min1_3);
            r = i32right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i32left(min2_2, min2_3);
            r = i32right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);


            l = i32left(min0_3, min0_4);
            r = i32right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i32left(min1_3, min1_4);
            r = i32right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i32left(min2_3, min2_4);
            r = i32right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i32left( max_all0, max_all1);
            r = i32right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i32left(min0_4, min0_0);
            r = i32right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i32left(min1_4, min1_0);
            r = i32right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i32left(min2_4, min2_0);
            r = i32right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i32left(min0_0, min0_1);
            r = i32right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i32left(min1_0, min1_1);
            r = i32right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i32left(min2_0, min2_1);
            r = i32right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );


            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);

            l = i32left(min0_1, min0_2);
            r = i32right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i32left(min1_1, min1_2);
            r = i32right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i32left(min2_1, min2_2);
            r = i32right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui32matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui32matrix_fusion_ilu5_elu2_red       (uint32 **X, int i, int j0, int j1, uint32 **Y)
{

    uint32  min0_0, min0_1, min0_2, min0_3, min0_4,
            min1_0, min1_1, min1_2, min1_3, min1_4,
            min2_0, min2_1, min2_2, min2_3, min2_4,
            min3_0, min3_1, min3_2, min3_3, min3_4,

            min_all0_0, min_all0_1, min_all0_2,
            min_all1_0, min_all1_1, min_all1_2,
            min_all2_0, min_all2_1, min_all2_2,
            min_all3_0, min_all3_1, min_all3_2,
            max_all0, max_all1, max_all2, max_all3,max_all4, max_all5,

            l, r;

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

    l = i32left(min0_0, min0_1);
    r = i32right(min0_1, min0_2);
    min_all0_0 = min3( l, min0_1, r);

    l = i32left(min0_1, min0_2);
    r = i32right(min0_2, min0_3);
    min_all0_1 = min3( l, min0_2, r);

    l = i32left(min1_0, min1_1);
    r = i32right(min1_1, min1_2);
    min_all1_0 = min3( l, min1_1, r);

    l = i32left(min1_1, min1_2);
    r = i32right(min1_2, min1_3);
    min_all1_1 = min3( l, min1_2, r);

    l = i32left(min2_0, min2_1);
    r = i32right(min2_1, min2_2);
    min_all2_0 = min3( l, min2_1, r);

    l = i32left(min2_1, min2_2);
    r = i32right(min2_2, min2_3);
    min_all2_1 = min3( l, min2_2, r);

    l = i32left(min3_0, min3_1);
    r = i32right(min3_1, min3_2);    
    min_all3_0 = min3( l, min3_1, r);

    l = i32left(min3_1, min3_2);
    r = i32right(min3_2, min3_3);    
    min_all3_1 = min3( l , min3_2 , r);

    uint32 load_im2_jp2,load_im1_jp2, load_i_jp2, load_ip1_jp2, load_ip2_jp2,load_ip3_jp2;

    for(int j = j0; j< j1-5; j+=5){
            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+2);
            load_im1_jp2 = load2(X, i-1, j+2);
            load_i_jp2 = load2(X, i, j+2);
            load_ip1_jp2 = load2(X, i+1, j+2);
            load_ip2_jp2 = load2(X, i+2, j+2);
            load_ip3_jp2 = load2(X, i+3, j+2);

            min0_4 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_4 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_4 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_4 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i32left(min0_2, min0_3);
            r = i32right(min0_3, min0_4);
            min_all0_2 = min3( l, min0_3, r);

            l = i32left(min1_2, min1_3);
            r = i32right(min1_3, min1_4);
            min_all1_2 = min3( l, min1_3, r);

            l = i32left(min2_2, min2_3);
            r = i32right(min2_3, min2_4);
            min_all2_2 = min3( l, min2_3, r);

            l = i32left(min3_2, min3_3);
            r = i32right(min3_3, min3_4);
            min_all3_2 = min3( l, min3_3, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);
            
            max_all3 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);
            store2( Y, i, j, max3( l, max_all1, r) );

            l = i32left(max_all3, max_all4);
            r = i32right(max_all4, max_all5);
            store2( Y, i+1, j, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+3);
            load_im1_jp2 = load2(X, i-1, j+3);
            load_i_jp2 = load2(X, i, j+3);
            load_ip1_jp2 = load2(X, i+1, j+3);
            load_ip2_jp2 = load2(X, i+2, j+3);
            load_ip3_jp2 = load2(X, i+3, j+3);

            min0_0 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_0 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_0 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_0 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);


            l = i32left(min0_3, min0_4);
            r = i32right(min0_4, min0_0);
            min_all0_0 = min3( l, min0_4, r);

            l = i32left(min1_3, min1_4);
            r = i32right(min1_4, min1_0);
            min_all1_0 = min3( l, min1_4, r);

            l = i32left(min2_3, min2_4);
            r = i32right(min2_4, min2_0);
            min_all2_0 = min3( l, min2_4, r);

            l = i32left(min3_3, min3_4);
            r = i32right(min3_4, min3_0);
            min_all3_0 = min3( l, min3_4, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);
            
            max_all3 = max3( min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3( min_all1_0, min_all2_0, min_all3_0);
            

            l = i32left( max_all0, max_all1);
            r = i32right(max_all1, max_all2);
            store2( Y, i, j+1, max3( l, max_all1, r) );

            l = i32left( max_all3, max_all4);
            r = i32right(max_all4, max_all5);
            store2( Y, i+1, j+1, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+4);
            load_im1_jp2 = load2(X, i-1, j+4);
            load_i_jp2 = load2(X, i, j+4);
            load_ip1_jp2 = load2(X, i+1, j+4);
            load_ip2_jp2 = load2(X, i+2, j+4);
            load_ip3_jp2 = load2(X, i+3, j+4);

            min0_1 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_1 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_1 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_1 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i32left(min0_4, min0_0);
            r = i32right(min0_0, min0_1);
            min_all0_1 = min3( l, min0_0, r);

            l = i32left(min1_4, min1_0);
            r = i32right(min1_0, min1_1);
            min_all1_1 = min3( l, min1_0, r);

            l = i32left(min2_4, min2_0);
            r = i32right(min2_0, min2_1);
            min_all2_1 = min3( l, min2_0, r);

            l = i32left(min3_4, min3_0);
            r = i32right(min3_0, min3_1);
            min_all3_1 = min3( l, min3_0, r);


            max_all0 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all1 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all2 = max3(min_all0_1, min_all1_1, min_all2_1);

            max_all3 = max3( min_all1_2, min_all2_2, min_all3_2);
            max_all4 = max3( min_all1_0, min_all2_0, min_all3_0);
            max_all5 = max3( min_all1_1, min_all2_1, min_all3_1);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+2, max3( l, max_all1, r) );

            l = i32left( max_all3, max_all4);
            r = i32right(max_all4, max_all5);
            store2( Y, i+1, j+2, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------
            
            load_im2_jp2 = load2(X, i-2, j+5);
            load_im1_jp2 = load2(X, i-1, j+5);
            load_i_jp2 = load2(X, i, j+5);
            load_ip1_jp2 = load2(X, i+1, j+5);
            load_ip2_jp2 = load2(X, i+2, j+5);
            load_ip3_jp2 = load2(X, i+3, j+5);

            min0_2 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_2 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_2 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_2 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i32left(min0_0, min0_1);
            r = i32right(min0_1, min0_2);
            min_all0_2 = min3( l, min0_1, r);

            l = i32left(min1_0, min1_1);
            r = i32right(min1_1, min1_2);
            min_all1_2 = min3( l, min1_1, r);

            l = i32left(min2_0, min2_1);
            r = i32right(min2_1, min2_2);
            min_all2_2 = min3( l, min2_1, r);

            l = i32left(min3_0, min3_1);
            r = i32right(min3_1, min3_2);
            min_all3_2 = min3( l, min3_1, r);


            max_all0 = max3(min_all0_0, min_all1_0, min_all2_0);
            max_all1 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all2 = max3(min_all0_2, min_all1_2, min_all2_2);

            max_all3 = max3(min_all1_0, min_all2_0, min_all3_0);
            max_all4 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all5 = max3( min_all1_2, min_all2_2, min_all3_2);


            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+3, max3( l, max_all1, r) );
         
            l = i32left( max_all3, max_all4);
            r = i32right(max_all4, max_all5);
            store2( Y, i+1, j+3, max3( l, max_all4, r) );

            // ---------------------------------------------------------------------------------------------
            load_im2_jp2 = load2(X, i-2, j+6);
            load_im1_jp2 = load2(X, i-1, j+6);
            load_i_jp2 = load2(X, i, j+6);
            load_ip1_jp2 = load2(X, i+1, j+6);
            load_ip2_jp2 = load2(X, i+2, j+6);
            load_ip3_jp2 = load2(X, i+3, j+6);

            min0_3 = min3( load_im2_jp2, load_im1_jp2, load_i_jp2);
            min1_3 = min3( load_im1_jp2, load_i_jp2, load_ip1_jp2);
            min2_3 = min3( load_i_jp2, load_ip1_jp2, load_ip2_jp2);
            min3_3 = min3( load_ip1_jp2, load_ip2_jp2, load_ip3_jp2);

            l = i32left(min0_1, min0_2);
            r = i32right(min0_2, min0_3);
            min_all0_0 = min3( l, min0_2, r);

            l = i32left(min1_1, min1_2);
            r = i32right(min1_2, min1_3);
            min_all1_0 = min3( l, min1_2, r);

            l = i32left(min2_1, min2_2);
            r = i32right(min2_2, min2_3);
            min_all2_0 = min3( l, min2_2, r);

            l = i32left(min3_1, min3_2);
            r = i32right(min3_2, min3_3);
            min_all3_0 = min3( l, min3_2, r);


            max_all0 = max3(min_all0_1, min_all1_1, min_all2_1);
            max_all1 = max3(min_all0_2, min_all1_2, min_all2_2);
            max_all2 = max3(min_all0_0, min_all1_0, min_all2_0);

            max_all3 = max3(min_all1_1, min_all2_1, min_all3_1);
            max_all4 = max3(min_all1_2, min_all2_2, min_all3_2);
            max_all5 = max3(min_all1_0, min_all2_0, min_all3_0);

            l = i32left(max_all0, max_all1);
            r = i32right(max_all1, max_all2);

            store2( Y, i, j+4, max3( l, max_all1, r) );

            l = i32left(max_all3, max_all4);
            r = i32right(max_all4, max_all5);

            store2( Y, i+1, j+4, max3( l, max_all4, r) );
            // ---------------------------------------------------------------------------------------------

            min_all0_1 = min_all0_0;
            min_all1_1 = min_all1_0;
            min_all2_1 = min_all2_0;
            min_all3_1 = min_all3_0;

            min_all0_0 = min_all0_2;
            min_all1_0 = min_all1_2;
            min_all2_0 = min_all2_2;
            min_all3_0 = min_all3_2;
    }
    r = (j1-j0-1) % 5;
    line_swp_ouverture3_ui32matrix_fusion(X, i, (j1-j0-r-1), (j1-j0), Y);
    line_swp_ouverture3_ui32matrix_fusion(X, i+1, (j1-j0-r-1), (j1-j0), Y);


}
// ---------------------------------------------------------------------------------------------
void line_swp_ouverture3_ui32matrix_fusion_ilu5_elu2_red_factor(uint32 **X, int i, int j0, int j1, uint32 **Y)
{
}
// ---------------------------------------------------------------------------------------------




        // FUSION COMPLETE SWP8
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_fusion                     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui8matrix_fusion(X_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, j1/8, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_fusion_ilu5_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui8matrix_fusion_ilu5_red(X_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, j1/8, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_fusion_ilu5_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P
    for( int i = i0; i < i1-2; i+=2){
        line_swp_ouverture3_ui8matrix_fusion_ilu5_elu2_red(X_P, i, j0, j1, Y_P);
    }

    int r = (i1-i0-1) % 2;
    line_swp_ouverture3_ui8matrix_fusion(X, i1-r-1, j0, j1, Y);
    //line_swp_ouverture3_ui8matrix_fusion(X, i1-r, j0, j1, Y);

    unpack_ui8matrix(Y_P, i1, j1/8, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **Y_P, uint8 **Y)
{}
// ---------------------------------------------------------------------------------------------


        // FUSION COMPLETE SWP16
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_fusion                     (uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui16matrix_fusion(X_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, j1/16, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_fusion_ilu5_red            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui16matrix_fusion_ilu5_red(X_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, j1/16, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_fusion_ilu5_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P
    for( int i = i0; i < i1-2; i+=2){
        line_swp_ouverture3_ui16matrix_fusion_ilu5_elu2_red(X_P, i, j0, j1, Y_P);
    }

    int r = (i1-i0-1) % 2;
    line_swp_ouverture3_ui16matrix_fusion(X_P, i1-r-1, j0, j1, Y_P);
    //line_swp_ouverture3_ui16matrix_fusion(X, i1-r, j0, j1, Y);

    unpack_ui16matrix(Y_P, i1, j1/16, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **Y_P, uint8 **Y)
{}
// ---------------------------------------------------------------------------------------------



        // FUSION COMPLETE SWP32
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_fusion                     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui32matrix_fusion(X_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, j1/32, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_fusion_ilu5_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    for( int i = i0; i < i1; i++){
        line_swp_ouverture3_ui32matrix_fusion_ilu5_red(X_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, j1/32, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_fusion_ilu5_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P
    for( int i = i0; i < i1-2; i+=2){
        line_swp_ouverture3_ui32matrix_fusion_ilu5_elu2_red(X_P, i, j0, j1, Y_P);
    }

    int r = (i1-i0-1) % 2;
    line_swp_ouverture3_ui32matrix_fusion(X_P, i1-r-1, j0, j1, Y_P);
    line_swp_ouverture3_ui32matrix_fusion(X_P, i1-r, j0, j1, Y_P);
    unpack_ui32matrix(Y_P, i1, j1/32, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_fusion_ilu5_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **Y_P, uint8 **Y)
{}
// ---------------------------------------------------------------------------------------------





        // PIPELINE
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_basic(X, i0-1, j0, j1, T);
    line_min3_ui8matrix_basic(X, i0, j0, j1, T);
    for( int i = i0; i < i1; i ++){
        line_min3_ui8matrix_basic(X, i+1, j0, j1, T);
        line_max3_ui8matrix_basic(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_red(X, i0-1, j0, j1, T);
    line_min3_ui8matrix_red(X, i0, j0, j1, T);
    for( int i = i0; i < i1; i ++){
        line_min3_ui8matrix_red(X, i+1, j0, j1, T);
        line_max3_ui8matrix_red(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_ilu3_red(X, i0-1, j0, j1, T);
    line_min3_ui8matrix_ilu3_red(X, i0, j0, j1, T);
    for( int i = i0; i < i1; i ++){
        line_min3_ui8matrix_ilu3_red(X, i+1, j0, j1, T);
        line_max3_ui8matrix_ilu3_red(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_elu2_red(X, i0-1, j0, j1, T);
    for( int i = i0; i < i1-1; i ++){
        line_min3_ui8matrix_elu2_red(X, i+1, j0, j1, T);
        line_max3_ui8matrix_elu2_red(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_elu2_red_factor(X, i0-1, j0, j1, T);
    for( int i = i0; i < i1-1; i ++){
        line_min3_ui8matrix_elu2_red_factor(X, i+1, j0, j1, T);
        line_max3_ui8matrix_elu2_red_factor(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_ilu3_elu2_red(X, i0-1, j0, j1, T);
    for( int i = i0; i < i1-1; i ++){
        line_min3_ui8matrix_ilu3_elu2_red(X, i+1, j0, j1, T);
        line_max3_ui8matrix_ilu3_elu2_red(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------
void ouverture3_ui8matrix_pipeline_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y)
{
    line_min3_ui8matrix_ilu3_elu2_red_factor(X, i0-1, j0, j1, T);
    for( int i = i0; i < i1-1; i ++){
        line_min3_ui8matrix_ilu3_elu2_red_factor(X, i+1, j0, j1, T);
        line_max3_ui8matrix_ilu3_elu2_red_factor(T, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------------------------


        // PIPELINE SWP8
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_basic(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui8matrix_basic(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui8matrix_basic(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_basic(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui8matrix_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui8matrix_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_ilu3_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui8matrix_ilu3_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui8matrix_ilu3_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_ilu3_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_elu2_red(X_P, i0-1, j0, j1, T_P);
    int i;
    for( i = i0; i < i1-1; i ++){
        line_swp_min3_ui8matrix_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_elu2_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_min3_ui8matrix_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui8matrix_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{

    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_ilu3_elu2_red(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui8matrix_ilu3_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_ilu3_elu2_red(T_P, i, j0, j1, Y_P);
    }
    
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **X_P, uint8 **T_P, uint8 **Y_P, uint8 **Y)
{
    pack_ui8matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui8matrix_ilu3_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui8matrix_ilu3_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui8matrix_ilu3_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y);

}
// ---------------------------------------------------------------------------------------------



        // PIPELINE SWP16
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_basic(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_basic(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui16matrix_basic(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui16matrix_basic(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_basic(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_red(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui16matrix_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui16matrix_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_ilu3_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui16matrix_ilu3_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui16matrix_ilu3_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_ilu3_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_elu2_red(X_P, i0-1, j0, j1, T_P);
    int i;
    for( i = i0; i < i1-1; i ++){
        line_swp_min3_ui16matrix_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_elu2_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui16matrix_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{

    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_ilu3_elu2_red(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui16matrix_ilu3_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_ilu3_elu2_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint16 **X_P, uint16 **T_P, uint16 **Y_P, uint8 **Y)
{
    pack_ui16matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui16matrix_ilu3_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui16matrix_ilu3_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui16matrix_ilu3_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y);

}
// ---------------------------------------------------------------------------------------------



        // PIPELINE SWP32
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_basic(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_basic(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui32matrix_basic(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui32matrix_basic(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_basic(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_red(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui32matrix_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui32matrix_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_ilu3_red(X_P, i0-1, j0, j1, T_P);
    line_swp_min3_ui32matrix_ilu3_red(X_P, i0, j0, j1, T_P);
    for( int i = i0; i < i1; i ++){
        line_swp_min3_ui32matrix_ilu3_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_ilu3_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_elu2_red(X_P, i0-1, j0, j1, T_P);
    int i;
    for( i = i0; i < i1-1; i ++){
        line_swp_min3_ui32matrix_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_elu2_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);
}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui32matrix_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{

    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_ilu3_elu2_red(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui32matrix_ilu3_elu2_red(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_ilu3_elu2_red(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);

}
// ---------------------------------------------------------------------------------------------
void ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **X_P, uint32 **T_P, uint32 **Y_P, uint8 **Y)
{
    pack_ui32matrix(X, i1, j1, X_P); // package X dans X_P

    line_swp_min3_ui32matrix_ilu3_elu2_red_factor(X_P, i0-1, j0, j1, T_P);
    for( int i = i0; i < i1-1; i ++){
        line_swp_min3_ui32matrix_ilu3_elu2_red_factor(X_P, i+1, j0, j1, T_P);
        line_swp_max3_ui32matrix_ilu3_elu2_red_factor(T_P, i, j0, j1, Y_P);
    }

    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y);

}
// // ---------------------------------------------------------------------------------------------