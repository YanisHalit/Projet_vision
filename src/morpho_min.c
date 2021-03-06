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
#include "../include/swp.h"

// ------------------------------------------------------------------------
void line_min3_ui8matrix_basic(uint8 **X, int i, int j0, int j1, uint8 **Y)
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
{   
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_basic(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_reg(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_reg(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_rot(X, i, j0, j1, Y);
    }
}
// --------------------------------------------------------------------------
void min3_ui8matrix_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_red(X, i, j0, j1, Y);
    }
}
// ---------------------------------------------------------------------------
void min3_ui8matrix_ilu3(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_ilu3(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
{
    for( int i = i0; i < i1; i++){
        line_min3_ui8matrix_ilu3_red(X, i, j0, j1, Y);
    }
}
// -------------------------------------------------------------------------------
void min3_ui8matrix_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
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

// ---------------- SWP ----------------

// ---------------- SWP8 ----------------
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_basic(uint8 **T, int i, int j0, int j1, uint8 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);


            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_rot(uint8 **T, int i, int j0, int j1, uint8 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
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
void line_swp_min3_ui8matrix_red                 (uint8 **T, int i, int j0, int j1, uint8 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);


            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_ilu3                (uint8 **T, int i, int j0, int j1, uint8 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  min3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_ilu3_red            (uint8 **T, int i, int j0, int j1, uint8 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_elu2_red            (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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

        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i8left(min_gauche_elu, min_milieu_elu);
            r = i8right(min_milieu_elu, min_droite_elu);
            res =  min3(min_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_elu2_red_factor     (uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

        min_mg_bg, min_bm_mm, min_md_bd, min_g_m, min_g_m_elu, min_l_orm, min_l_mm_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_bm_mm = min2(bas_milieu, milieu_milieu);
        or_milieu = min2(min_bm_mm, haut_milieu);
        min_milieu_elu = min2(min_bm_mm, milieu_elu);

        l = i8left(b0_g, or_milieu);
        min_l_orm = min2(l, or_milieu);

        l_elu = i8left(min_gauche_elu, min_milieu_elu);
        min_l_mm_elu = min2(l_elu, min_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            min_md_bd = min2(milieu_droit, bas_droit);
            b0_d = min2(haut_droit, min_md_bd);

            r = i8right(or_milieu, b0_d);
            res =  min2(min_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            min_droite_elu = min2(min_md_bd, droite_elu);

            r = i8right(min_milieu_elu, min_droite_elu);

            res =  min2(min_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;

            l = i8left(b0_g, or_milieu);
            min_l_orm = min2(l, or_milieu);

            l_elu = i8left(min_gauche_elu, min_milieu_elu);
            min_l_mm_elu = min2(l_elu, min_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_ilu3_elu2_red(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i8left(b0_g, or_milieu);
            r = i8right(or_milieu, b0_d);
            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i8left(min_gauche_elu, min_milieu_elu);
            r = i8right(min_milieu_elu, min_droite_elu);
            res =  min3(l, min_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i8left(or_milieu, b0_d);
            r = i8right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);

            l = i8left(min_milieu_elu, min_droite_elu);
            r = i8right(min_droite_elu, min_gauche_elu);
            res =  min3(l, min_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i8left(b0_d, b0_g);
            r = i8right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

            l = i8left(min_droite_elu, min_gauche_elu);
            r = i8right(min_gauche_elu, min_milieu_elu);
            res =  min3(l, min_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_min3_ui8matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui8matrix_ilu3_elu2_red_factor(uint8 **T, int i, int j0, int j1, uint8 **Y)
{
    uint8 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu, min_l_elu,

          min_mg_bg, min_mm_bm, min_md_bd, min_g_m, min_g_m_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_mm_bm = min2(milieu_milieu, bas_milieu);
        or_milieu = min2(haut_milieu, min_mm_bm);
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        min_md_bd = min2(milieu_droit, bas_droit);

        b0_d = min2(haut_droit, min_md_bd);

        l = i8left(b0_g, or_milieu);
        r = i8right(or_milieu, b0_d);

        min_l_elu = min2(l, or_milieu);
        res =  min2(min_l_elu, r);
        store2(Y, i, j, res);
        // --
        min_droite_elu = min2(min_md_bd, droite_elu);

        l = i8left(min_gauche_elu, min_milieu_elu);
        r = i8right(min_milieu_elu, min_droite_elu);

        min_l_elu = min2(l, min_milieu_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        min_mg_bg = min2(milieu_gauche, bas_gauche);

        b0_g  = min2(haut_gauche, min_mg_bg);

        l = i8left(or_milieu, b0_d);
        r = i8right(b0_d, b0_g);

        min_l_elu = min2(l, b0_d);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        l = i8left(min_milieu_elu, min_droite_elu);
        r = i8right(min_droite_elu, min_gauche_elu);

        min_l_elu = min2(l, min_droite_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        min_mm_bm = min2(milieu_milieu, bas_milieu);

        or_milieu = min2(haut_milieu, min_mm_bm);

        l = i8left(b0_d, b0_g);
        r = i8right(b0_g, or_milieu);

        min_l_elu = min2(l, b0_g);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

        l = i8left(min_droite_elu, min_gauche_elu);
        r = i8right(min_gauche_elu, min_milieu_elu);

        min_l_elu = min2(l, min_gauche_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_min3_ui8matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_min3_ui8matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_basic(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui8matrix(T, i0, i1-1, j0, (j1)/8, "X pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
            // displayM_ui8matrix(Y_P, i0, i1-1, j0, (j1-1)/8, "RESULT pack??"); // r??sultat pack??
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
    // display_ui8matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage r??sultat unpack??
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_rot(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_rot(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_red(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_ilu3(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_ilu3_red(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_ilu3_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    pack_ui8matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
    unpack_ui8matrix(Y_P, i1, (j1/8)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------


// ---------------- SWP16 ----------------
void line_swp_min3_ui16matrix_basic(uint16 **T, int i, int j0, int j1, uint16 **Y)
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

            b0_g = min3(haut_gauche , milieu_gauche , bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = min3(bas_milieu , milieu_milieu , haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit , milieu_droit , bas_droit);


            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res = min3(or_milieu , l , r);
            store2(Y, i, j, res);
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_rot(uint16 **T, int i, int j0, int j1, uint16 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
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
void line_swp_min3_ui16matrix_red                 (uint16 **T, int i, int j0, int j1, uint16 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);


            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_ilu3                (uint16 **T, int i, int j0, int j1, uint16 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  min3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_ilu3_red            (uint16 **T, int i, int j0, int j1, uint16 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_elu2_red            (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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

        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i16left(min_gauche_elu, min_milieu_elu);
            r = i16right(min_milieu_elu, min_droite_elu);
            res =  min3(min_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_elu2_red_factor     (uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

        min_mg_bg, min_bm_mm, min_md_bd, min_g_m, min_g_m_elu, min_l_orm, min_l_mm_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_bm_mm = min2(bas_milieu, milieu_milieu);
        or_milieu = min2(min_bm_mm, haut_milieu);
        min_milieu_elu = min2(min_bm_mm, milieu_elu);

        l = i16left(b0_g, or_milieu);
        min_l_orm = min2(l, or_milieu);

        l_elu = i16left(min_gauche_elu, min_milieu_elu);
        min_l_mm_elu = min2(l_elu, min_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            min_md_bd = min2(milieu_droit, bas_droit);
            b0_d = min2(haut_droit, min_md_bd);

            r = i16right(or_milieu, b0_d);
            res =  min2(min_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            min_droite_elu = min2(min_md_bd, droite_elu);

            r = i16right(min_milieu_elu, min_droite_elu);

            res =  min2(min_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;

            l = i16left(b0_g, or_milieu);
            min_l_orm = min2(l, or_milieu);

            l_elu = i16left(min_gauche_elu, min_milieu_elu);
            min_l_mm_elu = min2(l_elu, min_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_ilu3_elu2_red(uint16 **T, int i, int j0, int j1, uint16 **Y)
{
    uint16 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i16left(b0_g, or_milieu);
            r = i16right(or_milieu, b0_d);
            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i16left(min_gauche_elu, min_milieu_elu);
            r = i16right(min_milieu_elu, min_droite_elu);
            res =  min3(l, min_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i16left(or_milieu, b0_d);
            r = i16right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);

            l = i16left(min_milieu_elu, min_droite_elu);
            r = i16right(min_droite_elu, min_gauche_elu);
            res =  min3(l, min_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i16left(b0_d, b0_g);
            r = i16right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

            l = i16left(min_droite_elu, min_gauche_elu);
            r = i16right(min_gauche_elu, min_milieu_elu);
            res =  min3(l, min_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_min3_ui16matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui16matrix_ilu3_elu2_red_factor(uint16 **T, int i, int j0, int j1, uint16 **Y)
{   
    uint16 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu, min_l_elu,

          min_mg_bg, min_mm_bm, min_md_bd, min_g_m, min_g_m_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_mm_bm = min2(milieu_milieu, bas_milieu);
        or_milieu = min2(haut_milieu, min_mm_bm);
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        min_md_bd = min2(milieu_droit, bas_droit);

        b0_d = min2(haut_droit, min_md_bd);

        l = i16left(b0_g, or_milieu);
        r = i16right(or_milieu, b0_d);

        min_l_elu = min2(l, or_milieu);
        res =  min2(min_l_elu, r);
        store2(Y, i, j, res);
        // --
        min_droite_elu = min2(min_md_bd, droite_elu);

        l = i16left(min_gauche_elu, min_milieu_elu);
        r = i16right(min_milieu_elu, min_droite_elu);

        min_l_elu = min2(l, min_milieu_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        min_mg_bg = min2(milieu_gauche, bas_gauche);

        b0_g  = min2(haut_gauche, min_mg_bg);

        l = i16left(or_milieu, b0_d);
        r = i16right(b0_d, b0_g);

        min_l_elu = min2(l, b0_d);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        l = i16left(min_milieu_elu, min_droite_elu);
        r = i16right(min_droite_elu, min_gauche_elu);

        min_l_elu = min2(l, min_droite_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        min_mm_bm = min2(milieu_milieu, bas_milieu);

        or_milieu = min2(haut_milieu, min_mm_bm);

        l = i16left(b0_d, b0_g);
        r = i16right(b0_g, or_milieu);

        min_l_elu = min2(l, b0_g);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

        l = i16left(min_droite_elu, min_gauche_elu);
        r = i16right(min_gauche_elu, min_milieu_elu);

        min_l_elu = min2(l, min_gauche_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_min3_ui16matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_min3_ui16matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_basic          (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16)
{

    pack_ui16matrix(X, i1, j1, T16); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui16matrix(T16, i0, i1-1, j0, (j1-1)/16, "X16 pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_basic(T16, i, j0, (j1-1)/16, Y_P16);
    }
            // displayM_ui16matrix(Y_P16, i0, i1-1, j0, (j1-1)/16, "RESULT pack??"); // r??sultat pack??
    unpack_ui16matrix(Y_P16, i1, (j1/16)+1, Y_UP16); // unpack de result pack?? dans Y_UP16
    // display_ui8matrix(Y_UP16,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_rot                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_rot(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_red(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_ilu3(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_ilu3_red(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_ilu3_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    pack_ui16matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
    unpack_ui16matrix(Y_P, i1, (j1/16)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------





// ---------------- SWP32 ----------------

// ---------------- SWP32 ----------------
void line_swp_min3_ui32matrix_basic(uint32 **T, int i, int j0, int j1, uint32 **Y)
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

            b0_g = min3(haut_gauche , milieu_gauche , bas_gauche);


            haut_milieu = load2(T, i-1, j);
            milieu_milieu = load2(T, i, j);
            bas_milieu = load2(T, i+1, j);

            or_milieu = min3(bas_milieu , milieu_milieu , haut_milieu);


            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit , milieu_droit , bas_droit);


            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res = min3(or_milieu , l , r);
            store2(Y, i, j, res);
        }

}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_rot(uint32 **T, int i, int j0, int j1, uint32 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
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
void line_swp_min3_ui32matrix_red                 (uint32 **T, int i, int j0, int j1, uint32 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);


            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_ilu3                (uint32 **T, int i, int j0, int j1, uint32 **Y)
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

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);
            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  min3(r, b0_d, l);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_ilu3_red            (uint32 **T, int i, int j0, int j1, uint32 **Y)
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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);

            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_elu2_red            (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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

        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  min3(or_milieu, l, r);
            store2(Y, i, j, res);

            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i32left(min_gauche_elu, min_milieu_elu);
            r = i32right(min_milieu_elu, min_droite_elu);
            res =  min3(min_milieu_elu, l, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_elu2_red_factor     (uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

        min_mg_bg, min_bm_mm, min_md_bd, min_g_m, min_g_m_elu, min_l_orm, min_l_mm_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_bm_mm = min2(bas_milieu, milieu_milieu);
        or_milieu = min2(min_bm_mm, haut_milieu);
        min_milieu_elu = min2(min_bm_mm, milieu_elu);

        l = i32left(b0_g, or_milieu);
        min_l_orm = min2(l, or_milieu);

        l_elu = i32left(min_gauche_elu, min_milieu_elu);
        min_l_mm_elu = min2(l_elu, min_milieu_elu);

        for(int j = j0; j <= j1;   j++){
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            min_md_bd = min2(milieu_droit, bas_droit);
            b0_d = min2(haut_droit, min_md_bd);

            r = i32right(or_milieu, b0_d);
            res =  min2(min_l_orm, r);
            store2(Y, i, j, res);

            droite_elu = load2(T, i+2, j+1);

            min_droite_elu = min2(min_md_bd, droite_elu);

            r = i32right(min_milieu_elu, min_droite_elu);

            res =  min2(min_l_mm_elu, r);
            store2(Y, i+1, j, res);

            b0_g = or_milieu;
            or_milieu = b0_d;

            min_gauche_elu = min_milieu_elu;
            min_milieu_elu = min_droite_elu;

            l = i32left(b0_g, or_milieu);
            min_l_orm = min2(l, or_milieu);

            l_elu = i32left(min_gauche_elu, min_milieu_elu);
            min_l_mm_elu = min2(l_elu, min_milieu_elu);
        }
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_ilu3_elu2_red(uint32 **T, int i, int j0, int j1, uint32 **Y)
{
    uint32 haut_gauche, haut_milieu, haut_droit,
        milieu_gauche , milieu_milieu, milieu_droit,
        bas_gauche, bas_milieu, bas_droit,

        gauche_elu, milieu_elu, droite_elu,
        min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu,

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


        b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);
        or_milieu = min3(bas_milieu, milieu_milieu, haut_milieu);

        min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);
        min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

        for( int j = j0; j < j1-3; j+=3){
            // -------- 1 --------
            haut_droit = load2(T, i-1, j+1);
            milieu_droit = load2(T, i, j+1 );
            bas_droit = load2(T, i+1, j+1);

            droite_elu = load2(T, i+2, j+1);

            b0_d = min3(haut_droit, milieu_droit, bas_droit);

            l = i32left(b0_g, or_milieu);
            r = i32right(or_milieu, b0_d);
            res =  min3(l, or_milieu, r);
            store2(Y, i, j, res);
            //--
            min_droite_elu = min3(milieu_droit, bas_droit, droite_elu);

            l = i32left(min_gauche_elu, min_milieu_elu);
            r = i32right(min_milieu_elu, min_droite_elu);
            res =  min3(l, min_milieu_elu, r);
            store2(Y, i+1, j, res);

            // -------- 2 --------
            haut_gauche = load2(T, i-1, j+2);
            milieu_gauche = load2(T, i, j+2 );
            bas_gauche = load2(T, i+1, j+2);

            gauche_elu = load2(T, i+2, j+2);

            b0_g = min3(haut_gauche, milieu_gauche, bas_gauche);

            l = i32left(or_milieu, b0_d);
            r = i32right(b0_d, b0_g);

            res =  min3(l, b0_d, r);
            store2(Y, i, j+1, res);
            //--
            min_gauche_elu = min3(milieu_gauche, bas_gauche, gauche_elu);

            l = i32left(min_milieu_elu, min_droite_elu);
            r = i32right(min_droite_elu, min_gauche_elu);
            res =  min3(l, min_droite_elu, r);
            store2(Y, i+1, j+1, res);

            // -------- 3 --------
            haut_milieu = load2(T, i-1, j+3);
            milieu_milieu = load2(T, i, j+3 );
            bas_milieu = load2(T, i+1, j+3);

            milieu_elu = load2(T, i+2, j+3);

            or_milieu = min3(haut_milieu, milieu_milieu, bas_milieu);

            l = i32left(b0_d, b0_g);
            r = i32right(b0_g, or_milieu);

            res =  min3(l, b0_g, r);
            store2(Y, i, j+2, res);
            //--
            min_milieu_elu = min3(milieu_milieu, bas_milieu, milieu_elu);

            l = i32left(min_droite_elu, min_gauche_elu);
            r = i32right(min_gauche_elu, min_milieu_elu);
            res =  min3(l, min_gauche_elu, r);
            store2(Y, i+1, j+2, res);
        }
        int reste = (j1-j0-1) % 3;
        line_swp_min3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
        line_swp_min3_ui32matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------
void line_swp_min3_ui32matrix_ilu3_elu2_red_factor(uint32 **T, int i, int j0, int j1, uint32 **Y)
{   
    uint32 haut_gauche, haut_milieu, haut_droit,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_all, min_gauche, min_milieu, min_actu,

          gauche_elu , milieu_elu, droite_elu,
          min_gauche_elu, min_milieu_elu, min_droite_elu, min_all_elu, min_l_elu,

          min_mg_bg, min_mm_bm, min_md_bd, min_g_m, min_g_m_elu,

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

        min_mg_bg = min2(milieu_gauche, bas_gauche);
        b0_g = min2(haut_gauche, min_mg_bg);
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        min_mm_bm = min2(milieu_milieu, bas_milieu);
        or_milieu = min2(haut_milieu, min_mm_bm);
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

    for( int j = j0; j < j1-3; j+=3){
        // ------ 1 ------
        haut_droit = load2(T, i-1, j+1);
        milieu_droit = load2(T, i, j+1);
        bas_droit = load2(T, i+1, j+1);

        droite_elu = load2(T, i+2, j+1);

        min_md_bd = min2(milieu_droit, bas_droit);

        b0_d = min2(haut_droit, min_md_bd);

        l = i32left(b0_g, or_milieu);
        r = i32right(or_milieu, b0_d);

        min_l_elu = min2(l, or_milieu);
        res =  min2(min_l_elu, r);
        store2(Y, i, j, res);
        // --
        min_droite_elu = min2(min_md_bd, droite_elu);

        l = i32left(min_gauche_elu, min_milieu_elu);
        r = i32right(min_milieu_elu, min_droite_elu);

        min_l_elu = min2(l, min_milieu_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j, res);

        // ------ 2 ------
        haut_gauche = load2(T, i-1, j+2);
        milieu_gauche = load2(T, i, j+2 );
        bas_gauche = load2(T, i+1, j+2);

        gauche_elu = load2(T, i+2, j+2);

        min_mg_bg = min2(milieu_gauche, bas_gauche);

        b0_g  = min2(haut_gauche, min_mg_bg);

        l = i32left(or_milieu, b0_d);
        r = i32right(b0_d, b0_g);

        min_l_elu = min2(l, b0_d);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+1, res);
        // --
        min_gauche_elu = min2(min_mg_bg, gauche_elu);

        l = i32left(min_milieu_elu, min_droite_elu);
        r = i32right(min_droite_elu, min_gauche_elu);

        min_l_elu = min2(l, min_droite_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+1, res);

        // ------ 3 ------
        haut_milieu = load2(T, i-1, j+3);
        milieu_milieu = load2(T, i, j+3 );
        bas_milieu = load2(T, i+1, j+3);

        milieu_elu = load2(T, i+2, j+3);

        min_mm_bm = min2(milieu_milieu, bas_milieu);

        or_milieu = min2(haut_milieu, min_mm_bm);

        l = i32left(b0_d, b0_g);
        r = i32right(b0_g, or_milieu);

        min_l_elu = min2(l, b0_g);
        res =  min2(min_l_elu, r);
        store2(Y, i, j+2, res);

        // --
        min_milieu_elu = min2(min_mm_bm, milieu_elu);

        l = i32left(min_droite_elu, min_gauche_elu);
        r = i32right(min_gauche_elu, min_milieu_elu);

        min_l_elu = min2(l, min_gauche_elu);
        res =  min2(min_l_elu, r);
        store2(Y, i+1, j+2, res);
    }
    
    int reste = (j1-j0-1) % 3;
    line_swp_min3_ui32matrix_basic(T, i, (j1-j0-reste-1), (j1-j0), Y);
    line_swp_min3_ui32matrix_basic(T, i+1, (j1-j0-reste-1), (j1-j0), Y);
}
// -------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_basic          (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32)
{
    pack_ui32matrix(X, i1, j1, T32); // package X dans T
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui32matrix(T32, i0, i1-1, j0, (j1-1)/32, "X32 pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_basic(T32, i, j0, (j1-1)/32, Y_P32);
    }
            // displayM_ui32matrix(Y_P32, i0, i1-1, j0, (j1-1)/32, "RESULT pack??"); // r??sultat pack??
    unpack_ui32matrix(Y_P32, i1, (j1/32)+1, Y_UP32); // unpack de result pack?? dans Y_UP32
    // display_ui8matrix(Y_UP32,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_rot                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_rot(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_red(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_ilu3(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_ilu3_red(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_ilu3_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    pack_ui32matrix(X, i1, j1, T); // package X dans T
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
    unpack_ui32matrix(Y_P, i1, (j1/32)+1, Y_UP); // unpack de result pack?? dans Y_UP
}
// -------------------------------------------------------------------------------------------




// -------------------------------------------------------------------------------------------
//  BENCH 8
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_basic_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui8matrix(T, i0, i1-1, j0, (j1)/8, "X pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
            // displayM_ui8matrix(Y_P, i0, i1-1, j0, (j1-1)/8, "RESULT pack??"); // r??sultat pack??
    // display_ui8matrix(Y_UP,     i0, i1-1, j0, j1-1, "%5d", "RESULT UNPACK        "); // affichage r??sultat unpack??
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_rot_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_rot(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_red(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_ilu3(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui8matrix_ilu3_red(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_elu2_red_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_ilu3_elu2_red(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui8matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui8matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/8, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui8matrix_basic(T, i, j0, (j1)/8, Y_P);
    }
}
// -------------------------------------------------------------------------------------------




// -------------------------------------------------------------------------------------------
//  BENCH 16
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_basic_bench          (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16)
{

            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui16matrix(T16, i0, i1-1, j0, (j1-1)/16, "X16 pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_basic(T16, i, j0, (j1-1)/16, Y_P16);
    }
            // displayM_ui16matrix(Y_P16, i0, i1-1, j0, (j1-1)/16, "RESULT pack??"); // r??sultat pack??
    // display_ui8matrix(Y_UP16,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_rot_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_rot(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_red(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_ilu3(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui16matrix_ilu3_red(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_elu2_red_bench       (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_ilu3_elu2_red(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui16matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint16 **T, uint16 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui16matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/16, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui16matrix_basic(T, i, j0, (j1)/16, Y_P);
    }
}
// -------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------
//  BENCH 32
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_basic_bench          (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32)
{
            // display_ui8matrix(X,  i0, i1-1, j0, j1-1, "%5d", "X        "); // affichage de X normal
            // displayM_ui32matrix(T32, i0, i1-1, j0, (j1-1)/32, "X32 pack");         // affichage de X pack??
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_basic(T32, i, j0, (j1-1)/32, Y_P32);
    }
            // displayM_ui32matrix(Y_P32, i0, i1-1, j0, (j1-1)/32, "RESULT pack??"); // r??sultat pack??
    // display_ui8matrix(Y_UP32,  i0, i1-1, j0, j1, "%5d", "Y UP        "); // affichage de X normal
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_rot_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_rot(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_red_bench                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_red(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_bench                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_ilu3(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1; i++){
        line_swp_min3_ui32matrix_ilu3_red(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_elu2_red_bench            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_elu2_red_factor_bench     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_elu2_red_bench       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_ilu3_elu2_red(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
void min3_swp_ui32matrix_ilu3_elu2_red_factor_bench(uint8 **X, int i0, int i1, int j0, int j1, uint32 **T, uint32 **Y_P, uint8 **Y_UP)
{
    for( int i = i0; i < i1-2; i += 2){
        line_swp_min3_ui32matrix_ilu3_elu2_red_factor(T, i, j0, (j1)/32, Y_P);
    }

    int r = (i1-i0-1) % 2;
    for(int i = (i1-i0-r-1); i < (i1-i0); i++){
        line_swp_min3_ui32matrix_basic(T, i, j0, (j1)/32, Y_P);
    }
}
// -------------------------------------------------------------------------------------------
