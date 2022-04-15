/* -------------------- */
/* --- morpho_max.c --- */
/* -------------------- */

/*
 * Copyright (c) 2004 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"
#include "../include/swp.h"

// ------------------------------------------------------------------------
void line_max3_ui8matrix_basic(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    for( int j = j0; j < j1; j++){
        store2(Y, i, j, max3( max3( (load2(X, i-1, j-1)) , load2(X, i-1, j), load2(X, i-1, j+1 ) ),
                              max3( (load2(X, i, j-1)) , load2(X, i, j), load2(X, i, j+1 ) ),
                              max3( (load2(X, i+1, j-1)) , load2(X, i+1, j), load2(X, i+1, j+1 ) )
                            ));
    }
}
// ----------------------------------------------------------------------
void line_max3_ui8matrix_reg(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_haut, max_milieu, max_bas, max_all;
    for( int j = j0; j < j1; j++){
        haut_gauche = load2(X, i-1, j-1);
        haut_milieu = load2(X, i-1, j);
        haut_droit = load2(X, i-1, j+1);

        milieu_gauche = (load2(X, i, j-1));
        milieu_milieu = load2(X, i, j);
        milieu_droit = load2(X, i, j+1 );

        bas_gauche = load2(X, i+1, j-1);
        bas_milieu = load2(X, i+1, j);
        bas_droit = load2(X, i+1, j+1);

        max_haut = max3(haut_gauche, haut_milieu, haut_droit);
        max_milieu = max3(milieu_gauche, milieu_milieu, milieu_droit);
        max_bas = max3(bas_milieu, bas_droit, bas_gauche);

        max_all = max3(max_haut, max_milieu, max_bas);
        store2(Y, i, j, max_all);
    }
}
// ----------------------------------------------------------------------
void line_max3_ui8matrix_rot(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_haut, max_milieu, max_bas, max_all;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        max_haut = max3( haut_gauche, haut_milieu, haut_droit);
        max_milieu = max3( milieu_gauche, milieu_milieu, milieu_droit);
        max_bas = max3( bas_milieu, bas_droit, bas_gauche);

        max_all = max3( max_haut, max_milieu, max_bas);
        store2(Y, i, j, max_all);

        haut_gauche = haut_milieu;
        milieu_gauche = milieu_milieu;
        bas_gauche = bas_milieu;

        haut_milieu = haut_droit;
        milieu_milieu = milieu_droit;
        bas_milieu = bas_droit;
    }
}
// ----------------------------------------------------------------------
void line_max3_ui8matrix_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);

    max_gauche = max3(haut_gauche, milieu_gauche, bas_gauche);
    max_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        max_actu = max3(haut_droit, milieu_droit, bas_droit);


        max_all = max3(max_gauche, max_milieu, max_actu);
        store2(Y, i, j, max_all);

        max_gauche = max_milieu;
        max_milieu = max_actu;
    }
}
// -----------------------------------------------------------------------
void line_max3_ui8matrix_ilu3(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu;

    for( int j = j0; j < j1-3; j+=3 ){
        haut_gauche = load2(X, i-1, j-1);
        haut_milieu = load2(X, i-1, j);
        haut_droit = load2(X, i-1, j+1);

        milieu_gauche = load2(X, i, j-1);
        milieu_milieu = load2(X, i, j);
        milieu_droit = load2(X, i, j+1);

        bas_gauche = load2(X, i+1, j-1) ;
        bas_milieu = load2(X, i+1, j);
        bas_droit = load2(X, i+1, j+1);

        store2(Y, i, j, max3( max3(haut_gauche , haut_milieu, haut_droit),
                              max3(milieu_gauche, milieu_milieu, milieu_droit),
                              max3(bas_gauche, bas_milieu, bas_droit)
                            ));

        haut_gauche = load2(X, i-1, j+2);
        milieu_gauche = load2(X, i, j+2);
        bas_gauche = load2(X, i+1, j+2);

        store2(Y, i, j+1, max3( max3(haut_milieu , haut_droit, haut_gauche),
                              max3(milieu_milieu , milieu_droit, milieu_gauche),
                              max3(bas_milieu , bas_droit, bas_gauche)
                            ));

        haut_milieu =load2(X, i-1, j+3);
        milieu_milieu = load2(X, i, j+3);
        bas_milieu = load2(X, i+1, j+3);

        store2(Y, i, j+2, max3( max3(haut_droit, haut_gauche, haut_milieu),
                              max3(milieu_droit, milieu_gauche, milieu_milieu),
                              max3(bas_droit, bas_gauche, bas_milieu)
                            ));
    }

    int r = (j1-j0-1) % 3;
    for(int y = (j1-j0-r-1); y < (j1-j0); y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
    }
}
// ---------------------------------------------------------------------------
void line_max3_ui8matrix_ilu3_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);

    max_gauche = max3(haut_gauche, milieu_gauche, bas_gauche);
    max_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

    for( int j = j0; j < j1-3; j+=3){
        // -------- 1 --------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        max_actu = max3(haut_droit, milieu_droit, bas_droit);

        max_all = max3(max_gauche, max_milieu, max_actu);
        store2(Y, i, j, max_all);
        // -------- 2 --------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        max_gauche = max3(haut_droit, milieu_droit, bas_droit);

        max_all = max3(max_milieu, max_actu, max_gauche);
        store2(Y, i, j+1, max_all);
        // -------- 3 --------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        max_milieu = max3(haut_droit, milieu_droit, bas_droit);

        max_all = max3(max_actu, max_gauche, max_milieu);
        store2(Y, i, j+2, max_all);
    }

    int r = (j1-j0-1) % 3;
    for(int y = (j1-j0-r-1); y < (j1-j0); y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
    }
}
// ---------------------------------------------------------------------------
void line_max3_ui8matrix_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
       uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    max_gauche = max3(haut_gauche, milieu_gauche, bas_gauche);
    max_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
    max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
    max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        max_actu = max3(haut_droit, milieu_droit, bas_droit);
        max_all = max3(max_gauche, max_milieu, max_actu);
        store2(Y, i, j, max_all);


        max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);
        max_all_elu = max3(max_gauche_elu, max_milieu_elu, max_droite_elu);
        store2(Y, i+1, j, max_all_elu);

        max_gauche = max_milieu;
        max_milieu = max_actu;

        max_gauche_elu = max_milieu_elu;
        max_milieu_elu = max_droite_elu;
    }
}
// ----------------------------------------------------------------------------------
void line_max3_ui8matrix_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

          max_mg_bg, max_mm_bm, max_md_bd, max_g_m, max_g_m_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    max_mg_bg = max2(milieu_gauche, bas_gauche);

    max_gauche = max2(haut_gauche, max_mg_bg);
    max_gauche_elu = max2(max_mg_bg, gauche_elu);

    max_mm_bm = max2(milieu_milieu, bas_milieu);
    max_milieu = max2(haut_milieu, max_mm_bm);
    max_milieu_elu = max2(max_mm_bm, milieu_elu);

    max_g_m_elu = max2(max_gauche_elu, max_milieu_elu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        max_md_bd = max2(milieu_droit, bas_droit);
        max_g_m = max2(max_gauche, max_milieu);

        max_actu = max2(haut_droit, max_md_bd);
        max_all = max2(max_g_m, max_actu);
        store2(Y, i, j, max_all);

        max_droite_elu = max2(max_md_bd, droite_elu);

        max_all_elu = max2(max_g_m_elu, max_droite_elu);
        store2(Y, i+1, j, max_all_elu);

        max_gauche = max_milieu;
        max_milieu = max_actu;

        max_g_m_elu = max2(max_milieu_elu, max_droite_elu);
        max_milieu_elu = max_droite_elu;
    }
}
// --------------------------------------------------------------------------------
void line_max3_ui8matrix_ilu3_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
       uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    max_gauche = max3(haut_gauche, milieu_gauche, bas_gauche);
    max_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
    max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
    max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

    for( int j = j0; j < j1 - 3; j += 3){
        // ----- 1 ------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        max_actu = max3(haut_droit, milieu_droit, bas_droit);
        max_all = max3(max_gauche, max_milieu, max_actu);
        store2(Y, i, j, max_all);


        max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);
        max_all_elu = max3(max_gauche_elu, max_milieu_elu, max_droite_elu);
        store2(Y, i+1, j, max_all_elu);
        // ----- 2 ------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        droite_elu = load2(X, i+2, j+2);

        max_gauche = max3(haut_droit, milieu_droit, bas_droit);
        max_all = max3(max_milieu, max_actu, max_gauche);
        store2(Y, i, j+1, max_all);


        max_gauche_elu = max3(milieu_droit, bas_droit, droite_elu);
        max_all_elu = max3(max_milieu_elu, max_droite_elu, max_gauche_elu);
        store2(Y, i+1, j+1, max_all_elu);
        // ----- 3 ------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        droite_elu = load2(X, i+2, j+3);

        max_milieu = max3(haut_droit, milieu_droit, bas_droit);
        max_all = max3(max_actu, max_gauche, max_milieu);
        store2(Y, i, j+2, max_all);


        max_milieu_elu = max3(milieu_droit, bas_droit, droite_elu);
        max_all_elu = max3(max_droite_elu, max_gauche_elu, max_milieu_elu);
        store2(Y, i+1, j+2, max_all_elu);
    }
    int r = (j1-j0-1) % 3;
    line_max3_ui8matrix_basic(X, i, j1-r-1, j1, Y);
    line_max3_ui8matrix_basic(X, i+1, j1-r-1, j1, Y);
}
// ---------------------------------------------------------------------------------------
void line_max3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

          max_mg_bg, max_mm_bm, max_md_bd, max_g_m, max_g_m_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    max_mg_bg = max2(milieu_gauche, bas_gauche);
    
    max_gauche = max2(haut_gauche, max_mg_bg);
    max_gauche_elu = max2(max_mg_bg, gauche_elu);

    max_mm_bm = max2(milieu_milieu, bas_milieu);
    max_milieu = max2(haut_milieu, max_mm_bm);
    max_milieu_elu = max2(max_mm_bm, milieu_elu);

    max_g_m_elu = max2(max_gauche_elu, max_milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        max_md_bd = max2(milieu_droit, bas_droit);
        max_g_m = max2(max_gauche, max_milieu);

        max_actu = max2(haut_droit, max_md_bd);
        max_all = max2(max_g_m, max_actu);
        store2(Y, i, j, max_all);

        max_droite_elu = max2(max_md_bd, droite_elu);

        max_all_elu = max2(max_g_m_elu, max_droite_elu);
        store2(Y, i+1, j, max_all_elu);

        max_g_m_elu = max2(max_milieu_elu, max_droite_elu);

        // ------ 2 ------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        droite_elu = load2(X, i+2, j+2);

        max_md_bd = max2(milieu_droit, bas_droit);
        max_g_m = max2(max_milieu, max_actu);

        max_gauche = max2(haut_droit, max_md_bd);
        max_all = max2(max_g_m, max_gauche);
        store2(Y, i, j+1, max_all);

        max_gauche_elu = max2(max_md_bd, droite_elu);

        max_all_elu = max2(max_g_m_elu, max_gauche_elu);
        store2(Y, i+1, j+1, max_all_elu);

        max_g_m_elu = max2(max_droite_elu, max_gauche_elu);
        // ------ 3 ------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        droite_elu = load2(X, i+2, j+3);

        max_md_bd = max2(milieu_droit, bas_droit);
        max_g_m = max2(max_actu, max_gauche);

        max_milieu = max2(haut_droit, max_md_bd);
        max_all = max2(max_g_m, max_milieu);
        store2(Y, i, j+2, max_all);

        max_milieu_elu = max2(max_md_bd, droite_elu);

        max_all_elu = max2(max_g_m_elu, max_milieu_elu);
        store2(Y, i+1, j+2, max_all_elu);

        max_g_m_elu = max2(max_gauche_elu, max_milieu_elu);
    }

    int r = (j1-j0-1) % 3;
    line_max3_ui8matrix_basic(X, i, j1-r-1, j1, Y);
    line_max3_ui8matrix_basic(X, i+1, j1-r-1, j1, Y);
}
// ----------------------------------------------------------------------------
void max3_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{   
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_basic(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_reg(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_reg(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_rot(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------
void max3_ui8matrix_ilu3(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_ilu3(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void max3_ui8matrix_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_ilu3_red(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void max3_ui8matrix_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_max3_ui8matrix_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// --------------------------------------------------------------------------------------
void max3_ui8matrix_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1-2; i += 2){
        line_max3_ui8matrix_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// ------------------------------------------------------------------------------------
void max3_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_max3_ui8matrix_ilu3_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// -------------------------------------------------------------------------------------------
void max3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_max3_ui8matrix_ilu3_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, max3( max3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              max3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              max3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}

// ---------------- SWP ----------------

// ---------------- SWP8 ----------------
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_basic(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        for(int j = j0; j <= j1;   j++){

            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);


            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_rot(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1);
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            haut_gauche = haut_milieu;
            milieu_gauche = milieu_milieu;
            bas_gauche = bas_milieu;

            haut_milieu = haut_droit;
            milieu_milieu = milieu_droit;
            bas_milieu = bas_droit;
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_red                 (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);


            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_ilu3                (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;
        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  max3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_ilu3_red            (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_elu2_red            (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i8left(max_gauche_elu, max_milieu_elu);
            r = i8right(max_milieu_elu, max_droite_elu);
            res =  max3(max_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_elu2_red_factor     (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        max_mg_bg, max_bm_mm, max_md_bd, max_g_m, max_g_m_elu, max_l_orm, max_l_mm_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res, l_elu;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_bm_mm = max2(bas_milieu, milieu_milieu);
        or_milieu = max2(max_bm_mm, haut_milieu);
        max_milieu_elu = max2(max_bm_mm, milieu_elu);

        l = i8left(b0_g, or_milieu);
        max_l_orm = max2(l, or_milieu);

        l_elu = i8left(max_gauche_elu, max_milieu_elu);
        max_l_mm_elu = max2(l_elu, max_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            max_md_bd = max2(milieu_droit, bas_droit);
            b0_d = max2(haut_droit, max_md_bd);

            r = i8right(or_milieu, b0_d);
            res =  max2(max_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            max_droite_elu = max2(max_md_bd, droite_elu);

            r = i8right(max_milieu_elu, max_droite_elu);

            res =  max2(max_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;

            l = i8left(b0_g, or_milieu);
            max_l_orm = max2(l, or_milieu);

            l_elu = i8left(max_gauche_elu, max_milieu_elu);
            max_l_mm_elu = max2(l_elu, max_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_ilu3_elu2_red(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i8left(max_gauche_elu, max_milieu_elu);
            r = i8right(max_milieu_elu, max_droite_elu);
            res =  max3(l, max_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);

            l = i8left(max_milieu_elu, max_droite_elu);
            r = i8right(max_droite_elu, max_gauche_elu);
            res =  max3(l, max_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

            l = i8left(max_droite_elu, max_gauche_elu);
            r = i8right(max_gauche_elu, max_milieu_elu);
            res =  max3(l, max_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_max3_ui8matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_ilu3_elu2_red_factor(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu, max_l_elu,

          max_mg_bg, max_mm_bm, max_md_bd, max_g_m, max_g_m_elu,

          b0_g, b0_d, or_milieu,

          l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_mm_bm = max2(milieu_milieu, bas_milieu);
        or_milieu = max2(haut_milieu, max_mm_bm);
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        max_md_bd = max2(milieu_droit, bas_droit);

        b0_d = max2(haut_droit, max_md_bd);

        l = i8left(b0_g, or_milieu);
        r = i8right(or_milieu, b0_d);

        max_l_elu = max2(l, or_milieu);
        res =  max2(max_l_elu, r);
        store2(Y, i, j, res);
        // --
        max_droite_elu = max2(max_md_bd, droite_elu);

        l = i8left(max_gauche_elu, max_milieu_elu);
        r = i8right(max_milieu_elu, max_droite_elu);

        max_l_elu = max2(l, max_milieu_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        max_mg_bg = max2(milieu_gauche, bas_gauche);

        b0_g  = max2(haut_gauche, max_mg_bg);

        l = i8left(or_milieu, b0_d);
        r = i8right(b0_d, b0_g);

        max_l_elu = max2(l, b0_d);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        l = i8left(max_milieu_elu, max_droite_elu);
        r = i8right(max_droite_elu, max_gauche_elu);

        max_l_elu = max2(l, max_droite_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        max_mm_bm = max2(milieu_milieu, bas_milieu);

        or_milieu = max2(haut_milieu, max_mm_bm);

        l = i8left(b0_d, b0_g);
        r = i8right(b0_g, or_milieu);

        max_l_elu = max2(l, b0_g);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

        l = i8left(max_droite_elu, max_gauche_elu);
        r = i8right(max_gauche_elu, max_milieu_elu);

        max_l_elu = max2(l, max_gauche_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_max3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_max3_ui8matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui8matrix(T, i0, i1-1, j0, (j1)/8, "X pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
            // displayM_ui8matrix(Y_P, i0, i1-1, j0, (j1-1)/8, "RESULT packé"); // résultat packé
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
    // display_ui8matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage résultat unpacké
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_rot(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_red(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_ilu3(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_ilu3_red(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_ilu3_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------


// ---------------- SWP16 ----------------
void line_swp_max3_ui16matrix_basic(uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        for(int j = j0; j <= j1;   j++){

            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            b0_g = max3(haut_gauche , milieu_gauche , bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = max3(bas_milieu , milieu_milieu , haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit , milieu_droit , bas_droit);


            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res = max3(or_milieu , l , r);
            store2(Y, i, j, res);
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_rot(uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1);
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            haut_gauche = haut_milieu;
            milieu_gauche = milieu_milieu;
            bas_gauche = bas_milieu;

            haut_milieu = haut_droit;
            milieu_milieu = milieu_droit;
            bas_milieu = bas_droit;
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_red                 (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);


            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_ilu3                (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;
        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  max3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_ilu3_red            (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_elu2_red            (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i16left(max_gauche_elu, max_milieu_elu);
            r = i16right(max_milieu_elu, max_droite_elu);
            res =  max3(max_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_elu2_red_factor     (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        max_mg_bg, max_bm_mm, max_md_bd, max_g_m, max_g_m_elu, max_l_orm, max_l_mm_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res, l_elu;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_bm_mm = max2(bas_milieu, milieu_milieu);
        or_milieu = max2(max_bm_mm, haut_milieu);
        max_milieu_elu = max2(max_bm_mm, milieu_elu);

        l = i16left(b0_g, or_milieu);
        max_l_orm = max2(l, or_milieu);

        l_elu = i16left(max_gauche_elu, max_milieu_elu);
        max_l_mm_elu = max2(l_elu, max_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            max_md_bd = max2(milieu_droit, bas_droit);
            b0_d = max2(haut_droit, max_md_bd);

            r = i16right(or_milieu, b0_d);
            res =  max2(max_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            max_droite_elu = max2(max_md_bd, droite_elu);

            r = i16right(max_milieu_elu, max_droite_elu);

            res =  max2(max_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;

            l = i16left(b0_g, or_milieu);
            max_l_orm = max2(l, or_milieu);

            l_elu = i16left(max_gauche_elu, max_milieu_elu);
            max_l_mm_elu = max2(l_elu, max_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_ilu3_elu2_red(uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i16left(max_gauche_elu, max_milieu_elu);
            r = i16right(max_milieu_elu, max_droite_elu);
            res =  max3(l, max_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);

            l = i16left(max_milieu_elu, max_droite_elu);
            r = i16right(max_droite_elu, max_gauche_elu);
            res =  max3(l, max_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

            l = i16left(max_droite_elu, max_gauche_elu);
            r = i16right(max_gauche_elu, max_milieu_elu);
            res =  max3(l, max_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_max3_ui16matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_ilu3_elu2_red_factor(uint16 **T, int i, int j0, int j1, uint16 **Y)
{   
    uint16 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu, max_l_elu,

          max_mg_bg, max_mm_bm, max_md_bd, max_g_m, max_g_m_elu,

          b0_g, b0_d, or_milieu,

          l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_mm_bm = max2(milieu_milieu, bas_milieu);
        or_milieu = max2(haut_milieu, max_mm_bm);
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        max_md_bd = max2(milieu_droit, bas_droit);

        b0_d = max2(haut_droit, max_md_bd);

        l = i16left(b0_g, or_milieu);
        r = i16right(or_milieu, b0_d);

        max_l_elu = max2(l, or_milieu);
        res =  max2(max_l_elu, r);
        store2(Y, i, j, res);
        // --
        max_droite_elu = max2(max_md_bd, droite_elu);

        l = i16left(max_gauche_elu, max_milieu_elu);
        r = i16right(max_milieu_elu, max_droite_elu);

        max_l_elu = max2(l, max_milieu_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        max_mg_bg = max2(milieu_gauche, bas_gauche);

        b0_g  = max2(haut_gauche, max_mg_bg);

        l = i16left(or_milieu, b0_d);
        r = i16right(b0_d, b0_g);

        max_l_elu = max2(l, b0_d);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        l = i16left(max_milieu_elu, max_droite_elu);
        r = i16right(max_droite_elu, max_gauche_elu);

        max_l_elu = max2(l, max_droite_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        max_mm_bm = max2(milieu_milieu, bas_milieu);

        or_milieu = max2(haut_milieu, max_mm_bm);

        l = i16left(b0_d, b0_g);
        r = i16right(b0_g, or_milieu);

        max_l_elu = max2(l, b0_g);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

        l = i16left(max_droite_elu, max_gauche_elu);
        r = i16right(max_gauche_elu, max_milieu_elu);

        max_l_elu = max2(l, max_gauche_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_max3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_max3_ui16matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_basic          (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16)
{

    pack_ui16matrix(X, i1, j1, T16); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui16matrix(T16, i0, i1-1, j0, (j1-1)/16, "X16 pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_basic(T16, i, j0, (j1-1)/16, Y_P16);
    }
            // displayM_ui16matrix(Y_P16, i0, i1-1, j0, (j1-1)/16, "RESULT packé"); // résultat packé
    unpack_ui16matrix(Y_P16, i1, (j1/16)+1, Y_UP16); // unpack de result packé dans Y_UP16
    // display_ui8matrix(Y_UP16,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_rot                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_rot(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_red(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_ilu3(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_ilu3_red(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_ilu3_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------





// ---------------- SWP32 ----------------

// ---------------- SWP32 ----------------
void line_swp_max3_ui32matrix_basic(uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        for(int j = j0; j <= j1;   j++){

            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            b0_g = max3(haut_gauche , milieu_gauche , bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = max3(bas_milieu , milieu_milieu , haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit , milieu_droit , bas_droit);


            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res = max3(or_milieu , l , r);
            store2(Y, i, j, res);
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_rot(uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1);
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            haut_gauche = haut_milieu;
            milieu_gauche = milieu_milieu;
            bas_gauche = bas_milieu;

            haut_milieu = haut_droit;
            milieu_milieu = milieu_droit;
            bas_milieu = bas_droit;
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_red                 (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);


            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_ilu3                (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;
        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);

            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  max3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_ilu3_red            (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);

            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_elu2_red            (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  max3(or_milieu, l, r);
            store2(Y, i, j, res);

            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i32left(max_gauche_elu, max_milieu_elu);
            r = i32right(max_milieu_elu, max_droite_elu);
            res =  max3(max_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_elu2_red_factor     (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        max_mg_bg, max_bm_mm, max_md_bd, max_g_m, max_g_m_elu, max_l_orm, max_l_mm_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res, l_elu;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);


        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_bm_mm = max2(bas_milieu, milieu_milieu);
        or_milieu = max2(max_bm_mm, haut_milieu);
        max_milieu_elu = max2(max_bm_mm, milieu_elu);

        l = i32left(b0_g, or_milieu);
        max_l_orm = max2(l, or_milieu);

        l_elu = i32left(max_gauche_elu, max_milieu_elu);
        max_l_mm_elu = max2(l_elu, max_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            max_md_bd = max2(milieu_droit, bas_droit);
            b0_d = max2(haut_droit, max_md_bd);

            r = i32right(or_milieu, b0_d);
            res =  max2(max_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            max_droite_elu = max2(max_md_bd, droite_elu);

            r = i32right(max_milieu_elu, max_droite_elu);

            res =  max2(max_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            max_gauche_elu = max_milieu_elu;
            max_milieu_elu = max_droite_elu;

            l = i32left(b0_g, or_milieu);
            max_l_orm = max2(l, or_milieu);

            l_elu = i32left(max_gauche_elu, max_milieu_elu);
            max_l_mm_elu = max2(l_elu, max_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_ilu3_elu2_red(uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu,

        b0_g, b0_d, or_milieu,

        l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);


        b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = max3(bas_milieu, milieu_milieu, haut_milieu);

        max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);
        max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = max3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  max3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            max_droite_elu = max3(milieu_droit, bas_droit, droite_elu);

            l = i32left(max_gauche_elu, max_milieu_elu);
            r = i32right(max_milieu_elu, max_droite_elu);
            res =  max3(l, max_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = max3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  max3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            max_gauche_elu = max3(milieu_gauche, bas_gauche, gauche_elu);

            l = i32left(max_milieu_elu, max_droite_elu);
            r = i32right(max_droite_elu, max_gauche_elu);
            res =  max3(l, max_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = max3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  max3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            max_milieu_elu = max3(milieu_milieu, bas_milieu, milieu_elu);

            l = i32left(max_droite_elu, max_gauche_elu);
            r = i32right(max_gauche_elu, max_milieu_elu);
            res =  max3(l, max_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_max3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_max3_ui32matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui32matrix_ilu3_elu2_red_factor(uint32 **T, int i, int j0, int j1, uint32 **Y)
{   
    uint32 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          max_all, max_gauche, max_milieu, max_actu,

          gauche_elu , milieu_elu, droite_elu,
          max_gauche_elu, max_milieu_elu, max_droite_elu, max_all_elu, max_l_elu,

          max_mg_bg, max_mm_bm, max_md_bd, max_g_m, max_g_m_elu,

          b0_g, b0_d, or_milieu,

          l, r, y, res;

        haut_gauche = load2(T, i-1, j0-1);
        milieu_gauche = (load2(T, i, j0-1));
        bas_gauche = load2(T, i+1, j0-1);
        gauche_elu = load2(T, i+2, j0-1);

        haut_milieu = load2(T, i-1, j0);
        milieu_milieu = load2(T, i, j0);
        bas_milieu = load2(T, i+1, j0);
        milieu_elu = load2(T, i+2, j0);

        max_mg_bg = max2(milieu_gauche, bas_gauche);
        b0_g = max2(haut_gauche, max_mg_bg);
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        max_mm_bm = max2(milieu_milieu, bas_milieu);
        or_milieu = max2(haut_milieu, max_mm_bm);
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        max_md_bd = max2(milieu_droit, bas_droit);

        b0_d = max2(haut_droit, max_md_bd);

        l = i32left(b0_g, or_milieu);
        r = i32right(or_milieu, b0_d);

        max_l_elu = max2(l, or_milieu);
        res =  max2(max_l_elu, r);
        store2(Y, i, j, res);
        // --
        max_droite_elu = max2(max_md_bd, droite_elu);

        l = i32left(max_gauche_elu, max_milieu_elu);
        r = i32right(max_milieu_elu, max_droite_elu);

        max_l_elu = max2(l, max_milieu_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        max_mg_bg = max2(milieu_gauche, bas_gauche);

        b0_g  = max2(haut_gauche, max_mg_bg);

        l = i32left(or_milieu, b0_d);
        r = i32right(b0_d, b0_g);

        max_l_elu = max2(l, b0_d);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        max_gauche_elu = max2(max_mg_bg, gauche_elu);

        l = i32left(max_milieu_elu, max_droite_elu);
        r = i32right(max_droite_elu, max_gauche_elu);

        max_l_elu = max2(l, max_droite_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        max_mm_bm = max2(milieu_milieu, bas_milieu);

        or_milieu = max2(haut_milieu, max_mm_bm);

        l = i32left(b0_d, b0_g);
        r = i32right(b0_g, or_milieu);

        max_l_elu = max2(l, b0_g);
        res =  max2(max_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        max_milieu_elu = max2(max_mm_bm, milieu_elu);

        l = i32left(max_droite_elu, max_gauche_elu);
        r = i32right(max_gauche_elu, max_milieu_elu);

        max_l_elu = max2(l, max_gauche_elu);
        res =  max2(max_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_max3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_max3_ui32matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_basic          (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32)
{
    pack_ui32matrix(X, i1, j1, T32); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui32matrix(T32, i0, i1-1, j0, (j1-1)/32, "X32 pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_basic(T32, i, j0, (j1-1)/32, Y_P32);
    }
            // displayM_ui32matrix(Y_P32, i0, i1-1, j0, (j1-1)/32, "RESULT packé"); // résultat packé
    unpack_ui32matrix(Y_P32, i1, (j1/32)+1, Y_UP32); // unpack de result packé dans Y_UP32
    // display_ui8matrix(Y_UP32,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_rot                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_rot(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_red(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_ilu3(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_ilu3_red(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_ilu3_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result packé dans Y_UP
}
// -------------------------------------------------------------------------------------------




// -------------------------------------------------------------------------------------------
//  BENCH 8
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_basic_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui8matrix(T, i0, i1-1, j0, (j1)/8, "X pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
            // displayM_ui8matrix(Y_P, i0, i1-1, j0, (j1-1)/8, "RESULT packé"); // résultat packé
    // display_ui8matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage résultat unpacké
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_rot_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_rot(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_red(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_ilu3(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_ilu3_red(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_elu2_red_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_ilu3_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui8matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui8matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------




// -------------------------------------------------------------------------------------------
//  BENCH 16
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_basic_bench          (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16)
{

            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui16matrix(T16, i0, i1-1, j0, (j1-1)/16, "X16 pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_basic(T16, i, j0, (j1-1)/16, Y_P16);
    }
            // displayM_ui16matrix(Y_P16, i0, i1-1, j0, (j1-1)/16, "RESULT packé"); // résultat packé
    // display_ui8matrix(Y_UP16,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_rot_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_rot(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_red(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_ilu3(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_ilu3_red(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_elu2_red_bench       (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_ilu3_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui16matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------
//  BENCH 32
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_basic_bench          (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32)
{
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui32matrix(T32, i0, i1-1, j0, (j1-1)/32, "X32 pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_basic(T32, i, j0, (j1-1)/32, Y_P32);
    }
            // displayM_ui32matrix(Y_P32, i0, i1-1, j0, (j1-1)/32, "RESULT packé"); // résultat packé
    // display_ui8matrix(Y_UP32,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_rot_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_rot(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_red(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_ilu3(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui32matrix_ilu3_red(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_elu2_red_bench       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_ilu3_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void max3_swp_ui32matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_max3_ui32matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_max3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
