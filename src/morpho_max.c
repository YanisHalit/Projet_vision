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
// ------------------------------------------------------------------------
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
// ----------------------------------------------------------------------
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
// ----------------------------------------------------------------------
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
// ----------------------------------------------------------------------
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
// -----------------------------------------------------------------------
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
// ---------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------------
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
// --------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
{   
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_basic(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_reg(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_reg(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_rot(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void max3_ui8matrix_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------
void max3_ui8matrix_ilu3(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_ilu3(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void max3_ui8matrix_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_max3_ui8matrix_ilu3_red(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void max3_ui8matrix_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
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
// --------------------------------------------------------------------------------------
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
// ------------------------------------------------------------------------------------
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
// -------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------
void line_swp_max3_ui8matrix_basic(uint8 **T, int i, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,


        b0_g, b0_d, or_milieu,

        l, r, y;


        for(int j = j0; j <= j1;   j++){

            // appliquer le masque ici
            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);



            b0_g = haut_gauche | milieu_gauche | bas_gauche;



            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);




            or_milieu = bas_milieu | milieu_milieu | haut_milieu;


            // appliquer le second masque ici
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);



            b0_d = haut_droit | milieu_droit | bas_droit;

            // traitement debut avec la bordure

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            store2(Y, i, j, or_milieu | l | r);
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_max3_ui16matrix_basic(uint16 **T, int i, int j0, int j1, uint16 **Y)
// -------------------------------------------------------------------------------------------
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,


        b0_g, b0_d, or_milieu,

        l, r, y;


        for(int j = j0; j <= j1;   j++){

            // appliquer le masque ici
            haut_gauche = load2(T, i-1, j-1);
            milieu_gauche = (load2(T, i, j-1));
            bas_gauche = load2(T, i+1, j-1);



            b0_g = haut_gauche | milieu_gauche | bas_gauche;



            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);




            or_milieu = bas_milieu | milieu_milieu | haut_milieu;


            // appliquer le second masque ici
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);



            b0_d = haut_droit | milieu_droit | bas_droit;

            // traitement debut avec la bordure

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            store2(Y, i, j, or_milieu | l | r);
        }

}




void max3_swp_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
// -------------------------------------------------------------------------------------------
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui8matrix(T, i0, i1-1, j0, (j1)/8, "X pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
            // displayM_ui8matrix(Y_P, i0, i1-1, j0, (j1-1)/8, "RESULT packé"); // résultat packé
    unpack_ui8matrix(Y_P, i1, j1-1, Y_UP); // unpack de result packé dans Y_UP
    // display_ui8matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage résultat unpacké
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------
void max3_swp_ui16matrix_basic          (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP)
// -------------------------------------------------------------------------------------------
{
    pack_ui16matrix(X, i1, j1, T16); // package X dans T
            display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            displayM_ui16matrix(T16, i0, i1-1, j0, (j1-1)/16, "X16 pack");         // affichage de X packé
    for( int i = i0; i < i1; i++){
        line_swp_max3_ui16matrix_basic(T16, i, j0, (j1-1)/16, Y_P16);
    }
            displayM_ui16matrix(Y_P16, i0, i1-1, j0, (j1-1)/16, "RESULT packé"); // résultat packé
    unpack_ui16matrix(Y_P16, i1, j1, Y_UP); // unpack de result packé dans Y_UP
        printf("ici\n");exit(1000);
    // display_ui16matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage résultat unpacké
}
// -------------------------------------------------------------------------------------------