 /* -------------------- */
/* --- morpho_min.c --- */
/* -------------------- */

/*
 * Copyright (c) 2004 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 * Creation: 2004-05-18 : creation, from an idea of Antoine Manzanera
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"

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
    uint8 haut_gauche, haut_milieu, haut_droite,
          milieu_gauche, milieu_milieu, milieu_droit,
          bas_gauche, bas_milieu, bas_droit,
          min_haut, min_milieu, min_bas, min_all;
    for( int j = j0; j < j1; j++){
        haut_gauche = load2(X, i-1, j-1);
        haut_milieu = load2(X, i-1, j);
        haut_droite = load2(X, i-1, j+1);

        milieu_gauche = (load2(X, i, j-1));
        milieu_milieu = load2(X, i, j);
        milieu_droit = load2(X, i, j+1 );

        bas_gauche = load2(X, i+1, j-1);
        bas_milieu = load2(X, i+1, j);
        bas_droit = load2(X, i+1, j+1);

        min_haut = min3( haut_gauche, haut_milieu, haut_droite);
        min_milieu = min3( milieu_gauche, milieu_milieu, milieu_droit);
        min_bas = min3( bas_milieu, bas_droit, bas_gauche);

        min_all = min3( min_haut, min_milieu, min_bas);
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

        haut_gauche = haut_milieu;
        milieu_gauche = milieu_milieu;
        bas_gauche = bas_milieu;

        haut_milieu = haut_droit;
        milieu_milieu = milieu_droit;
        bas_milieu = bas_droit;
    }
}
// -----------------------------------------------------------------------
void line_min3_ui8matrix_ilu3(uint8 **X, int i, int j0, int j1, uint8 **Y)
// -----------------------------------------------------------------------
{
 int tmp;
    for( int j = j0; j < j1-3; j+=3 ){
        store2(Y, i, j, min3( min3( (load2(X, i-1, j-1)) , load2(X, i-1, j), load2(X, i-1, j+1 ) ),
                              min3( (load2(X, i, j-1)) , load2(X, i, j), load2(X, i, j+1 ) ),
                              min3( (load2(X, i+1, j-1)) , load2(X, i+1, j), load2(X, i+1, j+1 ) )
                            ));
        tmp = j + 1;
        store2(Y, i, tmp, min3( min3( (load2(X, i-1, tmp-1)) , load2(X, i-1, tmp), load2(X, i-1, tmp+1 ) ),
                              min3( (load2(X, i, tmp-1)) , load2(X, i, tmp), load2(X, i, tmp+1 ) ),
                              min3( (load2(X, i+1, tmp-1)) , load2(X, i+1, tmp), load2(X, i+1, tmp+1 ) )
                            ));

        tmp = j + 2;

        store2(Y, i, tmp, min3( min3( (load2(X, i-1, tmp-1)) , load2(X, i-1, tmp), load2(X, i-1, tmp+1 ) ),
                              min3( (load2(X, i, tmp-1)) , load2(X, i, tmp), load2(X, i, tmp+1 ) ),
                              min3( (load2(X, i+1, tmp-1)) , load2(X, i+1, tmp), load2(X, i+1, tmp+1 ) )
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
}
// ---------------------------------------------------------------------------
void line_min3_ui8matrix_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------------
void line_min3_ui8matrix_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ----------------------------------------------------------------------------------
{
}
// --------------------------------------------------------------------------------
void line_min3_ui8matrix_ilu3_elu2_red(uint8 **X, int i, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------
{
}
// ---------------------------------------------------------------------------------------
void line_min3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y)
// ---------------------------------------------------------------------------------------
{
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
}
// -------------------------------------------------------------------------------
void min3_ui8matrix_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------
{
}
// --------------------------------------------------------------------------------------
void min3_ui8matrix_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// --------------------------------------------------------------------------------------
{
}
// ------------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// ------------------------------------------------------------------------------------
{
}
// -------------------------------------------------------------------------------------------
void min3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
// -------------------------------------------------------------------------------------------
{
}
