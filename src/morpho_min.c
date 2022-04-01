/* -------------------- */
/* --- morpho_min.c --- */
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

#define PRINT(s) printf("%d\n",s)

// ------------------------------------------------------------------------
void line_min3_ui8matrix_basic(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ------------------------------------------------------------------------
{
    for( int j = j0; j < j1; j++){
        store2(Y, i, j, min3( min3( (load2(X, i-1, j-1)) , load2(X, i-1, j), load2(X, i-1, j+1 ) ),
                              min3( (load2(X, i, j-1)) , load2(X, i, j), load2(X, i, j+1 ) ),
                              min3( (load2(X, i+1, j-1)) , load2(X, i+1, j), load2(X, i+1, j+1 ) )
                            ));
    }
}
// ----------------------------------------------------------------------
void line_min3_ui8matrix_reg(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_haut, min_milieu, min_bas, min_all;
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

        min_haut = min3(haut_gauche, haut_milieu, haut_droit);
        min_milieu = min3(milieu_gauche, milieu_milieu, milieu_droit);
        min_bas = min3(bas_milieu, bas_droit, bas_gauche);

        min_all = min3(min_haut, min_milieu, min_bas);
        store2(Y, i, j, min_all);
    }
}
// ----------------------------------------------------------------------
void line_min3_ui8matrix_rot(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_haut, min_milieu, min_bas, min_all;

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

        min_haut = min3( haut_gauche, haut_milieu, haut_droit);
        min_milieu = min3( milieu_gauche, milieu_milieu, milieu_droit);
        min_bas = min3( bas_milieu, bas_droit, bas_gauche);

        min_all = min3( min_haut, min_milieu, min_bas);
        store2(Y, i, j, min_all);

        haut_gauche = haut_milieu;
        milieu_gauche = milieu_milieu;
        bas_gauche = bas_milieu;

        haut_milieu = haut_droit;
        milieu_milieu = milieu_droit;
        bas_milieu = bas_droit;
    }
}
// ----------------------------------------------------------------------
void line_min3_ui8matrix_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);

    min_gauche = min3(haut_gauche, milieu_gauche, bas_gauche);
    min_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        min_actu = min3(haut_droit, milieu_droit, bas_droit);


        min_all = min3(min_gauche, min_milieu, min_actu);
        store2(Y, i, j, min_all);

        min_gauche = min_milieu;
        min_milieu = min_actu;
    }
}
// -----------------------------------------------------------------------
void line_min3_ui8matrix_ilu3(uint8 **X, int i, int j0, int j1, uint8 **Y)
// -----------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu;

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

        store2(Y, i, j, min3( min3(haut_gauche , haut_milieu, haut_droit),
                              min3(milieu_gauche, milieu_milieu, milieu_droit),
                              min3(bas_gauche, bas_milieu, bas_droit)
                            ));

        haut_gauche = load2(X, i-1, j+2);
        milieu_gauche = load2(X, i, j+2);
        bas_gauche = load2(X, i+1, j+2);

        store2(Y, i, j+1, min3( min3(haut_milieu , haut_droit, haut_gauche),
                              min3(milieu_milieu , milieu_droit, milieu_gauche),
                              min3(bas_milieu , bas_droit, bas_gauche)
                            ));

        haut_milieu =load2(X, i-1, j+3);
        milieu_milieu = load2(X, i, j+3);
        bas_milieu = load2(X, i+1, j+3);

        store2(Y, i, j+2, min3( min3(haut_droit, haut_gauche, haut_milieu),
                              min3(milieu_droit, milieu_gauche, milieu_milieu),
                              min3(bas_droit, bas_gauche, bas_milieu)
                            ));
    }

    int r = (j1-j0-1) % 3;
    for(int y = (j1-j0-r-1); y < (j1-j0); y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
    }
}
// ---------------------------------------------------------------------------
void line_min3_ui8matrix_ilu3_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);

    min_gauche = min3(haut_gauche, milieu_gauche, bas_gauche);
    min_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

    for( int j = j0; j < j1-3; j+=3){
        // -------- 1 --------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        min_actu = min3(haut_droit, milieu_droit, bas_droit);

        min_all = min3(min_gauche, min_milieu, min_actu);
        store2(Y, i, j, min_all);
        // -------- 2 --------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        min_gauche = min3(haut_droit, milieu_droit, bas_droit);

        min_all = min3(min_milieu, min_actu, min_gauche);
        store2(Y, i, j+1, min_all);
        // -------- 3 --------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        min_milieu = min3(haut_droit, milieu_droit, bas_droit);

        min_all = min3(min_actu, min_gauche, min_milieu);
        store2(Y, i, j+2, min_all);
    }

    int r = (j1-j0-1) % 3;
    for(int y = (j1-j0-r-1); y < (j1-j0); y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
    }
}
// ---------------------------------------------------------------------------
void line_min3_ui8matrix_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------
{
       uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    min_gauche = min3(haut_gauche, milieu_gauche, bas_gauche);
    min_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
    min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
    min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        min_actu = min3(haut_droit, milieu_droit, bas_droit);
        min_all = min3(min_gauche, min_milieu, min_actu);
        store2(Y, i, j, min_all);


        min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);
        min_all_elu = min3(min_gauche_elu, min_milieu_elu, min_droite_elu);
        store2(Y, i+1, j, min_all_elu);

        min_gauche = min_milieu;
        min_milieu = min_actu;

        min_gauche_elu = min_milieu_elu;
        min_milieu_elu = min_droite_elu;
    }
}
// ----------------------------------------------------------------------------------
void line_min3_ui8matrix_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

          min_mg_bg, min_mm_bm, min_md_bd, min_g_m, min_g_m_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    min_mg_bg = min2(milieu_gauche, bas_gauche);

    min_gauche = min2(haut_gauche, min_mg_bg);
    min_gauche_elu = min2(min_mg_bg, gauche_elu);

    min_mm_bm = min2(milieu_milieu, bas_milieu);
    min_milieu = min2(haut_milieu, min_mm_bm);
    min_milieu_elu = min2(min_mm_bm, milieu_elu);

    min_g_m_elu = min2(min_gauche_elu, min_milieu_elu);

    for( int j = j0; j < j1; j++){
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        min_md_bd = min2(milieu_droit, bas_droit);
        min_g_m = min2(min_gauche, min_milieu);

        min_actu = min2(haut_droit, min_md_bd);
        min_all = min2(min_g_m, min_actu);
        store2(Y, i, j, min_all);

        min_droite_elu = min2(min_md_bd, droite_elu);

        min_all_elu = min2(min_g_m_elu, min_droite_elu);
        store2(Y, i+1, j, min_all_elu);

        min_gauche = min_milieu;
        min_milieu = min_actu;

        min_g_m_elu = min2(min_milieu_elu, min_droite_elu);
        min_milieu_elu = min_droite_elu;
    }
}
// --------------------------------------------------------------------------------
void line_min3_ui8matrix_ilu3_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------
{
       uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    min_gauche = min3(haut_gauche, milieu_gauche, bas_gauche);
    min_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
    min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
    min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

    for( int j = j0; j < j1 - 3; j += 3){
        // ----- 1 ------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        min_actu = min3(haut_droit, milieu_droit, bas_droit);
        min_all = min3(min_gauche, min_milieu, min_actu);
        store2(Y, i, j, min_all);


        min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);
        min_all_elu = min3(min_gauche_elu, min_milieu_elu, min_droite_elu);
        store2(Y, i+1, j, min_all_elu);
        // ----- 2 ------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        droite_elu = load2(X, i+2, j+2);

        min_gauche = min3(haut_droit, milieu_droit, bas_droit);
        min_all = min3(min_milieu, min_actu, min_gauche);
        store2(Y, i, j+1, min_all);


        min_gauche_elu = min3(milieu_droit, bas_droit, droite_elu);
        min_all_elu = min3(min_milieu_elu, min_droite_elu, min_gauche_elu);
        store2(Y, i+1, j+1, min_all_elu);
        // ----- 3 ------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        droite_elu = load2(X, i+2, j+3);

        min_milieu = min3(haut_droit, milieu_droit, bas_droit);
        min_all = min3(min_actu, min_gauche, min_milieu);
        store2(Y, i, j+2, min_all);


        min_milieu_elu = min3(milieu_droit, bas_droit, droite_elu);
        min_all_elu = min3(min_droite_elu, min_gauche_elu, min_milieu_elu);
        store2(Y, i+1, j+2, min_all_elu);
    }
    int r = (j1-j0-1) % 3;
    line_min3_ui8matrix_basic(X, i, j1-r-1, j1, Y);
    line_min3_ui8matrix_basic(X, i+1, j1-r-1, j1, Y);
}
// ---------------------------------------------------------------------------------------
void line_min3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

          min_mg_bg, min_mm_bm, min_md_bd, min_g_m, min_g_m_elu;

    haut_gauche = load2(X, i-1, j0-1);
    milieu_gauche = load2(X, i, j0-1);
    bas_gauche = load2(X, i+1, j0-1);
    gauche_elu = load2(X, i+2, j0-1);

    haut_milieu = load2(X, i-1, j0);
    milieu_milieu = load2(X, i, j0);
    bas_milieu = load2(X, i+1, j0);
    milieu_elu = load2(X, i+2, j0);

    min_mg_bg = min2(milieu_gauche, bas_gauche);
    
    min_gauche = min2(haut_gauche, min_mg_bg);
    min_gauche_elu = min2(min_mg_bg, gauche_elu);

    min_mm_bm = min2(milieu_milieu, bas_milieu);
    min_milieu = min2(haut_milieu, min_mm_bm);
    min_milieu_elu = min2(min_mm_bm, milieu_elu);

    min_g_m_elu = min2(min_gauche_elu, min_milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(X, i-1, j+1);
        milieu_droit = load2(X, i, j+1);
        bas_droit = load2(X, i+1, j+1);

        droite_elu = load2(X, i+2, j+1);

        min_md_bd = min2(milieu_droit, bas_droit);
        min_g_m = min2(min_gauche, min_milieu);

        min_actu = min2(haut_droit, min_md_bd);
        min_all = min2(min_g_m, min_actu);
        store2(Y, i, j, min_all);

        min_droite_elu = min2(min_md_bd, droite_elu);

        min_all_elu = min2(min_g_m_elu, min_droite_elu);
        store2(Y, i+1, j, min_all_elu);

        min_g_m_elu = min2(min_milieu_elu, min_droite_elu);

        // ------ 2 ------
        haut_droit = load2(X, i-1, j+2);
        milieu_droit = load2(X, i, j+2);
        bas_droit = load2(X, i+1, j+2);

        droite_elu = load2(X, i+2, j+2);

        min_md_bd = min2(milieu_droit, bas_droit);
        min_g_m = min2(min_milieu, min_actu);

        min_gauche = min2(haut_droit, min_md_bd);
        min_all = min2(min_g_m, min_gauche);
        store2(Y, i, j+1, min_all);

        min_gauche_elu = min2(min_md_bd, droite_elu);

        min_all_elu = min2(min_g_m_elu, min_gauche_elu);
        store2(Y, i+1, j+1, min_all_elu);

        min_g_m_elu = min2(min_droite_elu, min_gauche_elu);
        // ------ 3 ------
        haut_droit = load2(X, i-1, j+3);
        milieu_droit = load2(X, i, j+3);
        bas_droit = load2(X, i+1, j+3);

        droite_elu = load2(X, i+2, j+3);

        min_md_bd = min2(milieu_droit, bas_droit);
        min_g_m = min2(min_actu, min_gauche);

        min_milieu = min2(haut_droit, min_md_bd);
        min_all = min2(min_g_m, min_milieu);
        store2(Y, i, j+2, min_all);

        min_milieu_elu = min2(min_md_bd, droite_elu);

        min_all_elu = min2(min_g_m_elu, min_milieu_elu);
        store2(Y, i+1, j+2, min_all_elu);

        min_g_m_elu = min2(min_gauche_elu, min_milieu_elu);
    }

    int r = (j1-j0-1) % 3;
    line_min3_ui8matrix_basic(X, i, j1-r-1, j1, Y);
    line_min3_ui8matrix_basic(X, i+1, j1-r-1, j1, Y);
}
// ----------------------------------------------------------------------------
void min3_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------
{   
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_basic(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_reg(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_reg(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_rot(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------
void min3_ui8matrix_ilu3(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_ilu3(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_ilu3_red(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void min3_ui8matrix_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_min3_ui8matrix_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// --------------------------------------------------------------------------------------
void min3_ui8matrix_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------------
{
    for( int i = i0; i < i1-2; i += 2){
        line_min3_ui8matrix_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// ------------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ------------------------------------------------------------------------------------
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_min3_ui8matrix_ilu3_elu2_red(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
// -------------------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------------------
{
    int i;
    for( i = i0; i < i1-2; i += 2){
        line_min3_ui8matrix_ilu3_elu2_red_factor(X, i, j0, j1, Y);
    }
    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        for( int y = j0; y < j1; y++){
        store2(Y, i, y, min3( min3( (load2(X, i-1, y-1)) , load2(X, i-1, y), load2(X, i-1, y+1 ) ),
                              min3( (load2(X, i, y-1)) , load2(X, i, y), load2(X, i, y+1 ) ),
                              min3( (load2(X, i+1, y-1)) , load2(X, i+1, y), load2(X, i+1, y+1 ) )
                            ));
        }
    }
}
