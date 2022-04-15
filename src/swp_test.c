#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"

#include "../include/macro_bench.h"
#include "x86intrin.h" // _rdtsc()

#include "../include/swp.h"
#include "../include/morpho_max.h"
#include "../include/morpho_min.h"
#include "../include/morpho_ouverture.h"
#include "../include/swp_test.h"



// ------------------------------ UTILITAIRES -----------------------------------------
void rand_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 x0, uint8 x1)
{
    int d = x1 - x0 + 1; // range [x0, x1]

    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {

            int x = x0 + rand() % d;

            X[i][j] = (uint8) x;
        }
    }
}
// ------------------------------------------------------------------------------------
void rand1_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, int density)
{
    // binary generator

    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {

            int r = rand() % 100;
            if(r <= density)
                X[i][j] = 1;
            else
                X[i][j] = 0;
        }
    }
}
// ------------------------------------------------------------------------------------
int compare_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y, char* name)
{
    int c = 0;

    //display_ui8matrix(Y, i0, i1, j0, j1, "%5d", name);

    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {
            if(X[i][j] != Y[i][j]) {
                printf("%s [%3d][%3d] = %3d NEQ %3d\n", name, i, j, X[i][i], Y[i][i]);
                c++;
            }
        }
    }
    if(c) {
        printf("%d errors !\n", c); exit(-1);
    } else {
        printf("[compare]: %s is OK\n", name);
        return c;
    }
    puts("----------------------");
}
// ------------------------------------------------------------------------------------


// ------------------------------ Test SWP8 -------------------------------------------
void test_swp8_morpho_max_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp8_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint8 **T_bas                     , **Y_P_bas       , **Y_bas;
    uint8 **T_rot                     , **Y_P_rot       , **Y_rot;
    uint8 **T_red                     , **Y_P_red       , **Y_red;
    uint8 **T_ilu3                    , **Y_P_ilu3      , **Y_ilu3;
    uint8 **T_ilu3r                   , **Y_P_ilu3r     , **Y_ilu3r;
    uint8 **T_elu2r                   , **Y_P_elu2r       , **Y_elu2r;
    uint8 **T_elu2rf                  , **Y_P_elu2rf      , **Y_elu2rf;
    uint8 **T_ilu3_elu2r              , **Y_P_ilu3_elu2r  , **Y_ilu3_elu2r;
    uint8 **T_ilu3_elu2rf             , **Y_P_ilu3_elu2rf , **Y_ilu3_elu2rf;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_rot         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_red         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3        = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_elu2r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);

    // Y_P NO BORDER
    Y_P_bas         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_rot         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_red         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3        = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_elu2r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    zero_ui8matrix(T_bas,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_rot,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_red,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3,        0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_elu2r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_elu2rf,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_bas,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_rot,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_red,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    max3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui8matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui8matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui8matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    max3_swp_ui8matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui8matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui8matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui8matrix_ilu3_red est decommentée
    max3_swp_ui8matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui8matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui8matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP8        ");
    // display_ui8matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui8matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui8matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui8matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui8matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui8matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui8matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare max swp 8 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp8                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp8                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp8                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp8                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp8                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp8                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp8                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp8              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp8             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui8matrix(T_bas,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_rot,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_red,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3,        0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_elu2r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_elu2rf,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_bas,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_rot,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_red,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp8_morpho_min_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp8_morpho_min_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint8 **T_bas                     , **Y_P_bas       , **Y_bas;
    uint8 **T_rot                     , **Y_P_rot       , **Y_rot;
    uint8 **T_red                     , **Y_P_red       , **Y_red;
    uint8 **T_ilu3                    , **Y_P_ilu3      , **Y_ilu3;
    uint8 **T_ilu3r                   , **Y_P_ilu3r     , **Y_ilu3r;
    uint8 **T_elu2r                   , **Y_P_elu2r       , **Y_elu2r;
    uint8 **T_elu2rf                  , **Y_P_elu2rf      , **Y_elu2rf;
    uint8 **T_ilu3_elu2r              , **Y_P_ilu3_elu2r  , **Y_ilu3_elu2r;
    uint8 **T_ilu3_elu2rf             , **Y_P_ilu3_elu2rf , **Y_ilu3_elu2rf;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_rot         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_red         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3        = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_elu2r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    T_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);

    // Y_P NO BORDER
    Y_P_bas         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_rot         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_red         = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3        = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_elu2r       = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);
    Y_P_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, (w1-1+b)/8);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    zero_ui8matrix(T_bas,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_rot,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_red,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3,        0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_elu2r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_elu2rf,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_bas,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_rot,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_red,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    min3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui8matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui8matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui8matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    min3_swp_ui8matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui8matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui8matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui8matrix_ilu3_red est decommentée
    min3_swp_ui8matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui8matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui8matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP8        ");
    // display_ui8matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui8matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui8matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui8matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui8matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui8matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui8matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare min swp 8 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp8                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp8                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp8                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp8                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp8                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp8                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp8                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp8              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp8             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui8matrix(T_bas,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_rot,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_red,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3,        0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_elu2r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_elu2rf,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_bas,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_rot,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_red,         0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp8_morpho_ouverture_routine(int h, int w0)
{
    int b = 1; // bord
    // avec des ADD
    int r = 1; // rayon des operateurs
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp8_morpho_ouverture_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    if(w1 > w0) puts("w1 > w0");

    // ------- Def  -------

    uint8 **X;

    uint8 **X8, **X1;

    uint8 **T_basic;

    uint8 **Y_basic;



    // POUR FUSION -----------------------------

    // X packé dans X_P fusion
    uint8 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;
    // XP traité dans Y_P
    uint8 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;
    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;

    // POUR PIPELINE -----------------------------

    // X packée dans X_P
    uint8 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;
    // premier traitement T_P
    uint8 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;
    // deuxieme traitement Y_P
    uint8 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;
    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;

    char *format1; // affichage 1 bit/pixel
    char *format8; // affichage 8 bits/pixel
    
    int c; // error

    format8 = "%3x";
    format1 = "%4d";
    format1 = "%2d";
    //format1 = "%1d";
    

    //------- Alloc  -------

    // puts ("malloc");
    // X 2r-border
    X                               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    X8                              = ui8matrix(0-2*r, h-1+2*r, 0-1*r, w8-1+1*r);
    uint8 **T_basic_or              = ui8matrix(0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    uint8 **Y_basic_or              = ui8matrix(0, h-1, 0, w1-1);
    // X1     = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

    // Pipeline alloc
    // X packée pipeline
    X_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // T_P premier traitement pipeline
    T_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y_P final pipeline
    Y_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    // fusion alloc
    // X packée fusion
    X_P_bas_fusion                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_red_fusion                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_elu2_red_fusion               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_elu2_red_factor_fusion        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    //  Y final packé
    Y_P_bas_fusion                  = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_red_fusion             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_elu2_red_fusion        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_elu2_red_factor_fusion = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);


    //------- init  -------
    // puts("zero X");
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic_or,            0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    zero_ui8matrix(Y_basic_or,                            0, h-1, 0, w1-1);

    // Pipeline
    // X packée pipeline
    zero_ui8matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // T_P premier traitement pipeline
    zero_ui8matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y_P deuxieme traitement pipeline
    zero_ui8matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);

    // Fusion
    // X packé fusion
    zero_ui8matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_red_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final packé fusion
    zero_ui8matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    ouverture3_ui8matrix_basic                      (X, 0, h, 0, w1, T_basic_or, Y_basic_or                      );


    puts("--------------");
    puts("--- fusion ---");
    puts("--------------");

    // Traitement fusion swp 8
    // ouverture3_swp_ui8matrix_fusion                     (X, 0, h, 0, w1, X_P_bas_fusion,                    Y_P_bas_fusion,                                            Y_fusion_bas);
    // ouverture3_swp_ui8matrix_fusion_ilu5_red            (X, 0, h, 0, w1, X_P_ilu5_red_fusion,                    Y_P_ilu5_red_fusion,                                            Y_fusion_ilu5_red);
    ouverture3_swp_ui8matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w1, X_P_ilu5_elu2_red_fusion,                    Y_P_ilu5_elu2_red_fusion,                                            Y_fusion_ilu5_elu2_red);

    // Affichage fusion swp8
    // display_ui8matrix(X                      ,  0, h-1, 0, w0-1, format1, "X                      ");
    // display_ui8matrix(Y_basic_or                      ,  0, h-1, 0, w0-1, format1, "Y_basic                      ");
    // display_ui8matrix(Y_fusion_bas                     ,  0, h-1, 0, w0-1, format1, "Y_fusion_bas                     ");
    // display_ui8matrix(Y_fusion_ilu5_red            ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_red            ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red       ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red       ");

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_bas                     , "Y_fusion_bas_swp8                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_red                     , "Y_fusion_ilu5_red_swp8                ");
    c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_elu2_red                     , "Y_fusion_ilu5_elu2_red_swp8           ");

    // putchar('\n');

    // puts("--------------");
    // puts("-- pipeline --");
    // puts("--------------");

    // T: full-matrix
    // ouverture3_swp_ui8matrix_pipeline_basic               (X, 0, h, 0, w1, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             );
    // ouverture3_swp_ui8matrix_pipeline_red                 (X, 0, h, 0, w1, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_red            (X, 0, h, 0, w1, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             );
    // ouverture3_swp_ui8matrix_pipeline_elu2_red            (X, 0, h, 0, w1, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            );
    // ouverture3_swp_ui8matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w1, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w1, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w1, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      );

    //display_ui8matrix(Y_basic_or                        , 0, h-1, 0, w0-1, format1, "Y_basic_or                        ");
    //display_ui8matrix(Y_pipeline_bas                    , 0, h-1, 0, w0-1, format1, "Y_pipeline_bas                     ");
    //display_ui8matrix(Y_pipeline_red                    ,  0, h-1, 0, w0-1, format1, "Y_pipeline_red                   ");
    //display_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red_factor     ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red       ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red_factor");

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_bas                     , "Y_pipeline_bas_swp8                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_red                     , "Y_pipeline_red_swp8                 ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_red            , "Y_pipeline_ilu3_red_swp8            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red            , "Y_pipeline_elu2_red_swp8            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red_factor     , "Y_pipeline_elu2_red_factor_swp8     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red       , "Y_pipeline_ilu3_elu2_red_swp8       ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red_factor, "Y_pipeline_ilu3_elu2_red_factor_swp8");
    // putchar('\n');


    // puts("----------");
    // puts("-- free --");
    // puts("----------");

    // free_ui8matrix(X, 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    // free_ui8matrix(X8, 0-2*r, h-1+2*r, 0-1*r, w8-1+1*r);
    // free_ui8matrix(X1, 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

    // free_ui8matrix(T_basic,                        0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    // free_ui8matrix(T_pipeline,                     0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    // free_ui8matrix(T_pipeline_ilu3_red,            0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    // free_ui8matrix(T_pipeline_elu2_red_factor,     0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);

    // free_ui8matrix(Y_basic                      , 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // free_ui8matrix(Y_fusion                     , 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // free_ui8matrix(Y_fusion_ilu5_red            , 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // free_ui8matrix(Y_fusion_ilu5_elu2_red       , 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // free_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // free_ui8matrix(Y_fusion_ilu15_red           , 0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);

    // free_ui8matrix(T_pipeline,                     0-0*r, h-1+0*r, 0-0*r, w1-1+0*r);
    // puts("free done");
}
// ------------------------------------------------------------------------------------

// ------------------------------ Test SWP16 ------------------------------------------
void test_swp16_morpho_max_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 16; if(w0 % 16) w8 = w8+1;
    int w1 = 16 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp16_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint16**T_bas                     ; 
    uint16**T_rot                     ;
    uint16**T_red                     ;
    uint16**T_ilu3                    ;
    uint16**T_ilu3r                   ;
    uint16**T_elu2r                   ;
    uint16**T_elu2rf                  ;
    uint16**T_ilu3_elu2r              ;
    uint16**T_ilu3_elu2rf             ;
    
    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;

    uint16 **Y_P_bas       ;
    uint16 **Y_P_rot       ;
    uint16 **Y_P_red       ;
    uint16 **Y_P_ilu3      ;
    uint16 **Y_P_ilu3r     ;
    uint16 **Y_P_elu2r       ;
    uint16 **Y_P_elu2rf      ;
    uint16 **Y_P_ilu3_elu2r  ;
    uint16 **Y_P_ilu3_elu2rf ;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_rot         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_red         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3        = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_elu2r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_elu2rf      = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3_elu2r  = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3_elu2rf = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);

    // Y_P NO BORDER
    Y_P_bas         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_rot         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_red         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3        = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_elu2r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_elu2rf      = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3_elu2r  = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3_elu2rf = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);

    zero_ui16matrix(T_bas,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_rot,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_red,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3,        0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_elu2r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_elu2rf,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);

    zero_ui16matrix(Y_P_bas,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_rot,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_red,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);

    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    max3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui16matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui16matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui16matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    max3_swp_ui16matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui16matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui16matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui16matrix_ilu3_red est decommentée
    max3_swp_ui16matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui16matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui16matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui16matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui16matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui16matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP16        ");
    // display_ui16matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui16matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui16matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui16matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui16matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui16matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui16matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui16matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare max swp 16 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp16                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp16                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp16                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp16                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp16                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp16                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp16                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp16              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp16             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui16matrix(T_bas,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_rot,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_red,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3,        0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_elu2r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_elu2rf,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_bas,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_rot,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_red,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp16_morpho_min_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 16; if(w0 % 16) w8 = w8+1;
    int w1 = 16 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp16_morpho_min_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint16**T_bas                     ;
    uint16**T_rot                     ;
    uint16**T_red                     ;
    uint16**T_ilu3                    ;
    uint16**T_ilu3r                   ;
    uint16**T_elu2r                   ;
    uint16**T_elu2rf                  ;
    uint16**T_ilu3_elu2r              ;
    uint16**T_ilu3_elu2rf             ;
    
    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;


    uint16 **Y_P_bas       ;
    uint16 **Y_P_rot       ;
    uint16 **Y_P_red       ;
    uint16 **Y_P_ilu3      ;
    uint16 **Y_P_ilu3r     ;
    uint16 **Y_P_elu2r       ;
    uint16 **Y_P_elu2rf      ;
    uint16 **Y_P_ilu3_elu2r  ;
    uint16 **Y_P_ilu3_elu2rf ;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_rot         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_red         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3        = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_elu2r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_elu2rf      = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3_elu2r  = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    T_ilu3_elu2rf = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);

    // Y_P NO BORDER
    Y_P_bas         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_rot         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_red         = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3        = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_elu2r       = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_elu2rf      = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3_elu2r  = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);
    Y_P_ilu3_elu2rf = ui16matrix(0-b, h-1+b, 0-b, (w1-1+b)/16);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);

    zero_ui16matrix(T_bas,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_rot,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_red,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3,        0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_elu2r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_elu2rf,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);

    zero_ui16matrix(Y_P_bas,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_rot,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_red,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);

    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    min3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui16matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui16matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui16matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    min3_swp_ui16matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui16matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui16matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui16matrix_ilu3_red est decommentée
    min3_swp_ui16matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui16matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui16matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui16matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui16matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui16matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP16        ");
    // display_ui16matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui16matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui16matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui16matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui16matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui16matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui16matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui16matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare min swp 16 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp16                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp16                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp16                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp16                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp16                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp16                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp16                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp16              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp16             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui16matrix(T_bas,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_rot,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_red,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3,        0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_elu2r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_elu2rf,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_bas,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_rot,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_red,         0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/16);
    // free_ui16matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp16_morpho_ouverture_routine(int h, int w0)
{
    int b = 1; // bord
    // avec des ADD
    int r = 1; // rayon des operateurs
    int w8 = w0 / 8; if(w0 % 16) w8 = w8+1;
    int w1 = 16 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp16_morpho_ouverture_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    if(w1 > w0) puts("w1 > w0");

    // ------- Def  -------

    uint8 **X;

    uint8 **X8, **X1;

    uint8 **T_basic;

    uint8 **Y_basic;



    // POUR FUSION -----------------------------

    // X packé dans X_P fusion
    uint16 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;
    //
    // XP traité dans Y_P
    uint16 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;
    //
    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;
    //

    // POUR PIPELINE -----------------------------

    // X packée dans X_P
    uint16 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;
    //
    // premier traitement T_P
    uint16 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;
    //
    // deuxieme traitement Y_P
    uint16 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;
    //
    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;
    //


    char *format1; // affichage 1 bit/pixel
    char *format8; // affichage 8 bits/pixel
    
    int c; // error

    format8 = "%3x";
    format1 = "%4d";
    format1 = "%2d";
    //format1 = "%1d";
    

    //------- Alloc  -------

    // puts ("malloc");
    // X 2r-border
    X                               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    X8                              = ui8matrix(0-2*r, h-1+2*r, 0-1*r, w8-1+1*r);
    uint8 **T_basic_or              = ui8matrix(0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    uint8 **Y_basic_or              = ui8matrix(0, h-1, 0, w1-1);
    // X1     = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

    // Pipeline init
    // X packée pipeline
    X_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // T_P premier traitement pipeline
    T_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y_P final pipeline
    Y_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);
    //

    // X packée fusion
    X_P_bas_fusion                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_red_fusion                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_elu2_red_fusion               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_elu2_red_factor_fusion        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    //  Y final packé
    Y_P_bas_fusion                  = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_red_fusion             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_elu2_red_fusion        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_elu2_red_factor_fusion = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);
    //


    //------- init  -------
    // puts("zero X");
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic_or,            0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    zero_ui8matrix(Y_basic_or,                            0, h-1, 0, w1-1);

    // Pipeline
    // X packée pipeline
    zero_ui16matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // T_P premier traitement pipeline
    zero_ui16matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y_P deuxieme traitement pipeline
    zero_ui16matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);
    //

    // Fusion
    // X packé fusion
    zero_ui16matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_red_fusion,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y final packé fusion
    zero_ui16matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);
    //

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    ouverture3_ui8matrix_basic                      (X, 0, h, 0, w1, T_basic_or, Y_basic_or                      );    

    puts("--------------");
    puts("--- fusion ---");
    puts("--------------");

    // Traitement fusion swp 16
    // ouverture3_swp_ui16matrix_fusion                     (X, 0, h, 0, w1, X_P_bas_fusion,                    Y_P_bas_fusion,                                            Y_fusion_bas);
    // ouverture3_swp_ui16matrix_fusion_ilu5_red            (X, 0, h, 0, w1, X_P_ilu5_red_fusion,                    Y_P_ilu5_red_fusion,                                            Y_fusion_ilu5_red);
    ouverture3_swp_ui16matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w1, X_P_ilu5_elu2_red_fusion,                    Y_P_ilu5_elu2_red_fusion,                                            Y_fusion_ilu5_elu2_red);


    // Affichage fusion swp16
    // display_ui8matrix(X                      ,  0, h-1, 0, w0-1, format1, "X                      ");
    // display_ui8matrix(Y_basic_or                      ,  0, h-1, 0, w0-1, format1, "Y_basic                      ");
    // display_ui8matrix(Y_fusion_bas                     ,  0, h-1, 0, w0-1, format1, "Y_fusion_bas                     ");
    // display_ui8matrix(Y_fusion_ilu5_red            ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_red            ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red       ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red       ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red_factor,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red_factor");
    //

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_bas                     , "Y_fusion_bas_swp16                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_red                     , "Y_fusion_ilu5_red_swp16                ");
    c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_elu2_red                     , "Y_fusion_ilu5_elu2_red_swp16           ");

    // putchar('\n');

    // puts("--------------");
    // puts("-- pipeline --");
    // puts("--------------");

    // T: full-matrix
    // ouverture3_swp_ui16matrix_pipeline_basic               (X, 0, h, 0, w0, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             );
    // ouverture3_swp_ui16matrix_pipeline_red                 (X, 0, h, 0, w0, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             );
    // ouverture3_swp_ui16matrix_pipeline_ilu3_red            (X, 0, h, 0, w0, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             );
    // ouverture3_swp_ui16matrix_pipeline_elu2_red            (X, 0, h, 0, w0, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            );
    // ouverture3_swp_ui16matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w0, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    );
    // ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w0, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       );
    // ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w0, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      );

    //display_ui8matrix(Y_basic_or                        , 0, h-1, 0, w0-1, format1, "Y_basic_or                        ");
    //display_ui8matrix(Y_pipeline_bas                    , 0, h-1, 0, w0-1, format1, "Y_pipeline_bas                     ");
    //display_ui8matrix(Y_pipeline_red                    ,  0, h-1, 0, w0-1, format1, "Y_pipeline_red                   ");
    //display_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red_factor     ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red       ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red_factor");

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_bas                     , "Y_pipeline_bas_swp16                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_red                     , "Y_pipeline_red_swp16                 ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_red            , "Y_pipeline_ilu3_red_swp16            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red            , "Y_pipeline_elu2_red_swp16            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red_factor     , "Y_pipeline_elu2_red_factor_swp16     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red       , "Y_pipeline_ilu3_elu2_red_swp16       ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red_factor, "Y_pipeline_ilu3_elu2_red_factor_swp16");
    // putchar('\n');


    // puts("----------");
    // puts("-- free --");
    // puts("----------");


}
// ------------------------------------------------------------------------------------


// ------------------------------ Test SWP32 ------------------------------------------
void test_swp32_morpho_max_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 32; if(w0 % 32) w8 = w8+1;
    int w1 = 32 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp32_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint32**T_bas                     ; 
    uint32**T_rot                     ;
    uint32**T_red                     ;
    uint32**T_ilu3                    ;
    uint32**T_ilu3r                   ;
    uint32**T_elu2r                   ;
    uint32**T_elu2rf                  ;
    uint32**T_ilu3_elu2r              ;
    uint32**T_ilu3_elu2rf             ;
    
    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;


    uint32 **Y_P_bas       ;
    uint32 **Y_P_rot       ;
    uint32 **Y_P_red       ;
    uint32 **Y_P_ilu3      ;
    uint32 **Y_P_ilu3r     ;
    uint32 **Y_P_elu2r       ;
    uint32 **Y_P_elu2rf      ;
    uint32 **Y_P_ilu3_elu2r  ;
    uint32 **Y_P_ilu3_elu2rf ;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_rot         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_red         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3        = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_elu2r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_elu2rf      = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3_elu2r  = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3_elu2rf = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);

    // Y_P NO BORDER
    Y_P_bas         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_rot         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_red         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3        = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_elu2r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_elu2rf      = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3_elu2r  = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3_elu2rf = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);

    zero_ui32matrix(T_bas,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_rot,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_red,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3,        0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_elu2r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_elu2rf,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);

    zero_ui32matrix(Y_P_bas,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_rot,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_red,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);

    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    max3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui32matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui32matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui32matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    max3_swp_ui32matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui32matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui32matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui32matrix_ilu3_red est decommentée
    max3_swp_ui32matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui32matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui32matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui32matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui32matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui32matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas      ");
    // display_ui32matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui32matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui32matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui32matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui32matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui32matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui32matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui32matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare max swp 32 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp32                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp32                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp32                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp32                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp32                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp32                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp32                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp32              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp32             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui32matrix(T_bas,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_rot,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_red,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3,        0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_elu2r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_elu2rf,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_bas,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_rot,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_red,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp32_morpho_min_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 32; if(w0 % 32) w8 = w8+1;
    int w1 = 32 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp32_morpho_min_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X;
    uint8 **Y_bas_originale;

    uint32**T_bas                     ; 
    uint32**T_rot                     ;
    uint32**T_red                     ;
    uint32**T_ilu3                    ;
    uint32**T_ilu3r                   ;
    uint32**T_elu2r                   ;
    uint32**T_elu2rf                  ;
    uint32**T_ilu3_elu2r              ;
    uint32**T_ilu3_elu2rf             ;
    
    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;


    uint32 **Y_P_bas       ;
    uint32 **Y_P_rot       ;
    uint32 **Y_P_red       ;
    uint32 **Y_P_ilu3      ;
    uint32 **Y_P_ilu3r     ;
    uint32 **Y_P_elu2r       ;
    uint32 **Y_P_elu2rf      ;
    uint32 **Y_P_ilu3_elu2r  ;
    uint32 **Y_P_ilu3_elu2rf ;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X                       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    // T BORDER 1
    T_bas         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_rot         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_red         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3        = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_elu2r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_elu2rf      = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3_elu2r  = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    T_ilu3_elu2rf = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);

    // Y_P NO BORDER
    Y_P_bas         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_rot         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_red         = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3        = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_elu2r       = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_elu2rf      = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3_elu2r  = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);
    Y_P_ilu3_elu2rf = ui32matrix(0-b, h-1+b, 0-b, (w1-1+b)/32);

    // Y NO BORDER
    Y_bas         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_rot         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_red         = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3        = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2r       = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_elu2rf      = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2r  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_ilu3_elu2rf = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);

    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);

    zero_ui32matrix(T_bas,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_rot,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_red,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3,        0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_elu2r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_elu2rf,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);

    zero_ui32matrix(Y_P_bas,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_rot,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_red,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);

    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    min3_ui8matrix_basic               (X, 0, h, 0, w1, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui32matrix_basic                       (X, 0, h, 0, w1, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui32matrix_rot                         (X, 0, h, 0, w1, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui32matrix_red                         (X, 0, h, 0, w1, T_red, Y_P_red, Y_red);
    min3_swp_ui32matrix_ilu3                        (X, 0, h, 0, w1, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui32matrix_ilu3_red                    (X, 0, h, 0, w1, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui32matrix_elu2_red                    (X, 0, h, 0, w1, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui32matrix_ilu3_red est decommentée
    min3_swp_ui32matrix_elu2_red_factor             (X, 0, h, 0, w1, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui32matrix_ilu3_elu2_red               (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui32matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui32matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui32matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui32matrix(Y_bas,         0, h-1, 0, w0-1, "%5d", "Y bas      ");
    // display_ui32matrix(Y_reg,         0, h-1, 0, w0-1, "%5d", "Y reg        ");
    // display_ui32matrix(Y_rot,         0, h-1, 0, w0-1, "%5d", "Y rot        ");
    // display_ui32matrix(Y_red,         0, h-1, 0, w0-1, "%5d", "Y red        ");
    // display_ui32matrix(Y_ilu3,        0, h-1, 0, w0-1, "%5d", "Y ilu3       ");
    // display_ui32matrix(Y_ilu3r,       0, h-1, 0, w0-1, "%5d", "Y ilu3r      ");
    // display_ui32matrix(Y_elu2r,       0, h-1, 0, w0-1, "%5d", "Y elu2r      ");
    // display_ui32matrix(Y_elu2rf,      0, h-1, 0, w0-1, "%5d", "Y elu2rf     ");
    // display_ui32matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1, "%5d", "Y ilu3 elu2rf");/**/
    // puts("done\n");

    //--------------------- COMPARE ---------------------
    // puts("-- compare min swp 32 --");
    c = compare_swp_ui8matrix(Y_bas, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp32                     ");
    c = compare_swp_ui8matrix(Y_rot, 0, h-1, 0, w0-1,  Y_bas_originale, "Y rot_swp32                     ");
    c = compare_swp_ui8matrix(Y_red, 0, h-1, 0, w0-1,  Y_bas_originale, "Y red_swp32                     ");
    c = compare_swp_ui8matrix(Y_ilu3, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_swp32                    ");
    c = compare_swp_ui8matrix(Y_ilu3r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3r_swp32                   ");
    c = compare_swp_ui8matrix(Y_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2r_swp32                   ");
    c = compare_swp_ui8matrix(Y_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y elu2rf_swp32                  ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2r_swp32              ");
    c = compare_swp_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w0-1,  Y_bas_originale, "Y ilu3_elu2rf_swp32             ");

    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1);
    // free_ui32matrix(T_bas,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_rot,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_red,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3,        0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_elu2r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_elu2rf,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3_elu2r,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(T_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_bas,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_rot,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_red,         0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3_elu2r, 0, h-1, 0, (w1-1)/32);
    // free_ui32matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);
    // free_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    // free_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2r, 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp32_morpho_ouverture_routine(int h, int w0)
{
    int b = 1; // bord
    // avec des ADD
    int r = 1; // rayon des operateurs
    int w8 = w0 / 8; if(w0 % 32) w8 = w8+1;
    int w1 = 32 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp32_morpho_ouverture_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    if(w1 > w0) puts("w1 > w0");

    // ------- Def  -------

    uint8 **X;

    uint8 **X8, **X1;

    uint8 **T_basic;

    uint8 **Y_basic;



    // POUR FUSION -----------------------------

    // X packé dans X_P fusion
    uint32 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;
    //
    // XP traité dans Y_P
    uint32 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;
    //
    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;
    //

    // POUR PIPELINE -----------------------------

    // X packée dans X_P
    uint32 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;
    //
    // premier traitement T_P
    uint32 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;
    //
    // deuxieme traitement Y_P
    uint32 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;
    //
    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;
    //


    char *format1; // affichage 1 bit/pixel
    char *format8; // affichage 8 bits/pixel

    int c; // error

    format8 = "%3x";
    format1 = "%4d";
    format1 = "%2d";
    //format1 = "%1d";


    //------- Alloc  -------

    // puts ("malloc");
    // X 2r-border
    X                               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    X8                              = ui8matrix(0-2*r, h-1+2*r, 0-1*r, w8-1+1*r);
    uint8 **T_basic_or              = ui8matrix(0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    uint8 **Y_basic_or              = ui8matrix(0, h-1, 0, w1-1);
    // X1     = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

    // Pipeline init
    // X packée pipeline
    X_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // T_P premier traitement pipeline
    T_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y_P final pipeline
    Y_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);
    //

    // X packée fusion
    X_P_bas_fusion                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_red_fusion                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_elu2_red_fusion               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_elu2_red_factor_fusion        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    //  Y final packé
    Y_P_bas_fusion                  = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_red_fusion             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_elu2_red_fusion        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_elu2_red_factor_fusion = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);
    //


    //------- init  -------
    // puts("zero X");
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic_or,            0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    zero_ui8matrix(Y_basic_or,                            0, h-1, 0, w1-1);

    // Pipeline
    // X packée pipeline
    zero_ui32matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // T_P premier traitement pipeline
    zero_ui32matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y_P deuxieme traitement pipeline
    zero_ui32matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);
    //

    // Fusion
    // X packé fusion
    zero_ui32matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_red_fusion,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y final packé fusion
    zero_ui32matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);
    //

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    ouverture3_ui8matrix_basic                      (X, 0, h, 0, w0, T_basic_or, Y_basic_or                      );


    puts("--------------");
    puts("--- fusion ---");
    puts("--------------");

    // Traitement fusion swp 32
    // ouverture3_swp_ui32matrix_fusion                     (X, 0, h, 0, w1, X_P_bas_fusion,                    Y_P_bas_fusion,                                            Y_fusion_bas);
    // ouverture3_swp_ui32matrix_fusion_ilu5_red            (X, 0, h, 0, w1, X_P_ilu5_red_fusion,                    Y_P_ilu5_red_fusion,                                            Y_fusion_ilu5_red);
    // ouverture3_swp_ui32matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w1, X_P_ilu5_elu2_red_fusion,                    Y_P_ilu5_elu2_red_fusion,                                            Y_fusion_ilu5_elu2_red);


    // Affichage fusion swp32
    // display_ui8matrix(X                      ,  0, h-1, 0, w0-1, format1, "X                      ");
    // display_ui8matrix(Y_basic_or                      ,  0, h-1, 0, w0-1, format1, "Y_basic                      ");
    // display_ui8matrix(Y_fusion_bas                     ,  0, h-1, 0, w0-1, format1, "Y_fusion_bas                     ");
    // display_ui8matrix(Y_fusion_ilu5_red            ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_red            ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red       ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red       ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red_factor,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red_factor");
    //

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_bas                     , "Y_fusion_bas_swp32                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_red                     , "Y_fusion_ilu5_red_swp32                ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_fusion_ilu5_elu2_red                     , "Y_fusion_ilu5_elu2_red_swp32           ");

    // putchar('\n');


    puts("--------------");
    puts("-- pipeline --");
    puts("--------------");

    // T: full-matrix
    // ouverture3_swp_ui32matrix_pipeline_basic               (X, 0, h, 0, w0, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             );
    // ouverture3_swp_ui32matrix_pipeline_red                 (X, 0, h, 0, w0, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             );
    // ouverture3_swp_ui32matrix_pipeline_ilu3_red            (X, 0, h, 0, w0, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             );
    // ouverture3_swp_ui32matrix_pipeline_elu2_red            (X, 0, h, 0, w0, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            );
    // ouverture3_swp_ui32matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w0, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    );
    // ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w0, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       );
    // ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w0, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      );

    //display_ui8matrix(Y_basic_or                        , 0, h-1, 0, w0-1, format1, "Y_basic_or                        ");
    //display_ui8matrix(Y_pipeline_bas                    , 0, h-1, 0, w0-1, format1, "Y_pipeline_bas                     ");
    //display_ui8matrix(Y_pipeline_red                    ,  0, h-1, 0, w0-1, format1, "Y_pipeline_red                   ");
    //display_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red_factor     ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red       ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red_factor");

    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_bas                     , "Y_pipeline_bas_swp32                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_red                     , "Y_pipeline_red_swp32                 ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_red            , "Y_pipeline_ilu3_red_swp32            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red            , "Y_pipeline_elu2_red_swp32            ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red_factor     , "Y_pipeline_elu2_red_factor_swp32     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red       , "Y_pipeline_ilu3_elu2_red_swp32       ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red_factor, "Y_pipeline_ilu3_elu2_red_factor_swp32");
    // putchar('\n');


    // puts("----------");
    // puts("-- free --");
    // puts("----------");


}
// ------------------------------------------------------------------------------------


// ------------------------------ Appel test SWP max ----------------------------------
void test_swp_morpho_max(void)
{
    int h0 = 8;
    int w0 = 16;

    int dh = 4;
    int dw = 496;

    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            // test_swp8_morpho_max_routine(h, w);
            // test_swp16_morpho_max_routine(h, w);
            // test_swp32_morpho_max_routine(h, w);
        }
    }
}
// ------------------------------------------------------------------------------------

// ------------------------------ Appel test SWP min ----------------------------------
void test_swp_morpho_min(void)
{
    int h0 = 8;
    int w0 = 16;

    int dh = 4;
    int dw = 496;

    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            test_swp8_morpho_min_routine(h, w);
            test_swp16_morpho_min_routine(h, w);
            test_swp32_morpho_min_routine(h, w);
        }
    }
}
// ------------------------------------------------------------------------------------

// ------------------------------ Appel test Ouverture ----------------------------------
void test_swp_morpho_ouverture(void)
{
    int h0 = 4;
    int w0 = 16;
    
    int dh = 4;
    int dw = 3;
    
    dh = dw = 0;
    
    dh = 5; dw = 496;
    h0 = 8; w0 = 32;
    
    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            // test_swp8_morpho_ouverture_routine(h, w);
            // test_swp16_morpho_ouverture_routine(h, w);
            // test_swp32_morpho_ouverture_routine(h, w);
        }
    }
}
// ------------------------------------------------------------------------------------


// ------------------------------ Bench SWP8 ouverture ----------------------------------
void bench_swp8_morpho_ouverture(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp8_OUVERTURE.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **T_basic;
    uint8 **Y_basic;

    // POUR FUSION -----------------------------
    // X packé dans X_P fusion
    uint8 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;

    // XP traité dans Y_P
    uint8 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;

    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;

    // POUR PIPELINE -----------------------------
    // X packée dans X_P
    uint8 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;

    // premier traitement T_P
    uint8 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;

    // deuxieme traitement Y_P
    uint8 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;

    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;

    double cpp_basic;

    double cpp_fusion;
    double cpp_fusion_ilu5_red;
    double cpp_fusion_ilu5_elu2_red;

    double cpp_pipeline_basic;
    double cpp_pipeline_red;
    double cpp_pipeline_ilu3_red;
    double cpp_pipeline_elu2_red;
    double cpp_pipeline_elu2_red_factor;
    double cpp_pipeline_ilu3_elu2_red;
    double cpp_pipeline_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP8_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    X      = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    T_basic = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    // Pipeline alloc
    // X packée pipeline
    X_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // T_P premier traitement pipeline
    T_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y_P final pipeline
    Y_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    // Fusion alloc
    // X packée fusion
    X_P_bas_fusion                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_red_fusion                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_elu2_red_fusion               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu5_elu2_red_factor_fusion        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    //  Y final packé
    Y_P_bas_fusion                  = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_red_fusion             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_elu2_red_fusion        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    Y_P_ilu5_elu2_red_factor_fusion = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                           0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,                     0, h-1, 0, w1-1);

    // Pipeline init
    // X packée pipeline
    zero_ui8matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // T_P premier traitement pipeline
    zero_ui8matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y_P deuxieme traitement pipeline
    zero_ui8matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);

    // Fusion
    // X packé fusion
    zero_ui8matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_red_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final packé fusion
    zero_ui8matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 8;
        w1 = n / 1;
        
        //printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

        // PIPELINE
        // X Packé
        resize_ui8matrix(X_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(X_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );

        // Premier traitement pipeline
        resize_ui8matrix(T_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );

        // Deuxieme traitement pipeline
        resize_ui8matrix(Y_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(Y_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );

        // Y final pipeline
        resize_ui8matrix(Y_pipeline_bas                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_red                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red_factor                 , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red                   , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red_factor            , 0, h-1, 0, w1-1);


        // FUSION
        // X packée fusion
        resize_ui8matrix(X_P_bas_fusion                         , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(X_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(X_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(X_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/8);

        // Y finale packée
        resize_ui8matrix(Y_P_bas_fusion                         , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/8);

        // Y finale fusion
        resize_ui8matrix(Y_fusion_bas                         , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_red                    , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red               , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red_factor        , 0, h-1, 0, (w1-1)/8);


        // Bench basique
        BENCH(ouverture3_ui8matrix_basic                     (X, 0, h, 0, w1,                    T_basic  ,Y_basic)     , n, cpp_basic                      );

        // Bench fusion
        BENCH(ouverture3_swp_ui8matrix_fusion                     (X, 0, h, 0, w1, X_P_bas_fusion,                  Y_P_bas_fusion,           Y_fusion_bas                     )     , n, cpp_fusion                      );
        BENCH(ouverture3_swp_ui8matrix_fusion_ilu5_red            (X, 0, h, 0, w1, X_P_ilu5_red_fusion,             Y_P_ilu5_red_fusion,           Y_fusion_ilu5_red  )         , n, cpp_fusion_ilu5_red                     );
        BENCH(ouverture3_swp_ui8matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w1, X_P_ilu5_elu2_red_fusion,        Y_P_ilu5_elu2_red_fusion,           Y_fusion_ilu5_elu2_red)  , n, cpp_fusion_ilu5_elu2_red                      );

        // Bench pipeline
        BENCH(    ouverture3_swp_ui8matrix_pipeline_basic               (X, 0, h, 0, w1, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             ), n, cpp_pipeline_basic               );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_red                 (X, 0, h, 0, w1, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             ), n, cpp_pipeline_red                 );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_ilu3_red            (X, 0, h, 0, w1, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             ), n, cpp_pipeline_ilu3_red            );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_elu2_red            (X, 0, h, 0, w1, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            ), n, cpp_pipeline_elu2_red            );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w1, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    ), n, cpp_pipeline_elu2_red_factor     );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w1, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       ), n, cpp_pipeline_ilu3_elu2_red       );
        BENCH(    ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w1, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      ), n, cpp_pipeline_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");

        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d   ", n);

        // basic
        printf(format, cpp_basic                      );
        fprintf(output_file, "%.1f   ", cpp_basic);

        printf("   ");
        // fusion
        printf(format, cpp_fusion                     );
                fprintf(output_file, "%.1f   ", cpp_fusion);

        printf(format, cpp_fusion_ilu5_red            );
                fprintf(output_file, "%.1f   ", cpp_fusion_ilu5_red);

        printf(format, cpp_fusion_ilu5_elu2_red       );
                fprintf(output_file, "%.1f   ", cpp_fusion_ilu5_elu2_red);


        printf("   ");

        // pipeline
        printf(format, cpp_pipeline_basic                      );
                fprintf(output_file, "%.1f   ", cpp_pipeline_basic);

        printf(format, cpp_pipeline_red                        );
                fprintf(output_file, "%.1f   ", cpp_pipeline_red);

        printf(format, cpp_pipeline_ilu3_red                   );
                fprintf(output_file, "%.1f   ", cpp_pipeline_ilu3_red);

        printf(format, cpp_pipeline_elu2_red                   );
                fprintf(output_file, "%.1f   ", cpp_pipeline_elu2_red);

        printf(format, cpp_pipeline_elu2_red_factor            );
                fprintf(output_file, "%.1f   ", cpp_pipeline_elu2_red_factor);

        printf(format, cpp_pipeline_ilu3_elu2_red              );
                fprintf(output_file, "%.1f   ", cpp_pipeline_ilu3_elu2_red);

        printf(format, cpp_pipeline_ilu3_elu2_red_factor       );
                fprintf(output_file, "%.1f", cpp_pipeline_ilu3_elu2_red_factor);

        fprintf(output_file, "\n");
        putchar('\n');
    }
    fclose(output_file);
}
// ------------------------------ Bench SWP16 ouverture ----------------------------------
void bench_swp16_morpho_ouverture(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp16_OUVERTURE.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 16; if(w0 % 16) w8 = w8+1;
    int w1 = 16 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **T_basic;
    uint8 **Y_basic;

    // POUR FUSION -----------------------------
    // X packé dans X_P fusion
    uint16 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;

    // XP traité dans Y_P
    uint16 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;

    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;

    // POUR PIPELINE -----------------------------
    // X packée dans X_P
    uint16 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;

    // premier traitement T_P
    uint16 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;

    // deuxieme traitement Y_P
    uint16 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;

    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;

    double cpp_basic;

    double cpp_fusion;
    double cpp_fusion_red;
    double cpp_fusion_ilu5_red;
    double cpp_fusion_elu2_red;
    double cpp_fusion_elu2_red_factor;
    double cpp_fusion_ilu5_elu2_red;
    double cpp_fusion_ilu5_elu2_red_factor;

    double cpp_pipeline_basic;
    double cpp_pipeline_red;
    double cpp_pipeline_ilu3_red;
    double cpp_pipeline_elu2_red;
    double cpp_pipeline_elu2_red_factor;
    double cpp_pipeline_ilu3_elu2_red;
    double cpp_pipeline_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP16_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    // X 2r-border
    X      = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    T_basic = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    // Pipeline alloc
    // X packée pipeline
    X_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // T_P premier traitement pipeline
    T_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y_P final pipeline
    Y_P_bas                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_red                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_elu2_red                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_elu2_red_factor             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_elu2_red               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu3_elu2_red_factor        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    // Fusion alloc
    // X packée fusion
    X_P_bas_fusion                         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_red_fusion                    = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_elu2_red_fusion               = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    X_P_ilu5_elu2_red_factor_fusion        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //  Y final packé
    Y_P_bas_fusion                  = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_red_fusion             = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_elu2_red_fusion        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    Y_P_ilu5_elu2_red_factor_fusion = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,                     0, h-1, 0, w1-1);

    // Pipeline init
    // X packée pipeline
    zero_ui16matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // T_P premier traitement pipeline
    zero_ui16matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y_P deuxieme traitement pipeline
    zero_ui16matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);

    // Fusion
    // X packé fusion
    zero_ui16matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_red_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y final packé fusion
    zero_ui16matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 16;
        w1 = n / 1;
        
        //printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

        // PIPELINE
        // X Packé
        resize_ui16matrix(X_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(X_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );

        // Premier traitement pipeline
        resize_ui16matrix(T_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );

        // Deuxieme traitement pipeline
        resize_ui16matrix(Y_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(Y_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );

        // Y final pipeline
        resize_ui8matrix(Y_pipeline_bas                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_red                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red_factor                 , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red                   , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red_factor            , 0, h-1, 0, w1-1);


        // FUSION
        // X packée fusion
        resize_ui16matrix(X_P_bas_fusion                         , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(X_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(X_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(X_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/16);

        // Y finale packée
        resize_ui16matrix(Y_P_bas_fusion                         , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(Y_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(Y_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/16);
        resize_ui16matrix(Y_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/16);

        // Y finale fusion
        resize_ui8matrix(Y_fusion_bas                         , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_red                    , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red               , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red_factor        , 0, h-1, 0, (w1-1)/8);

        // Bench basique
        BENCH(ouverture3_ui8matrix_basic                     (X, 0, h, 0, w1,                    T_basic  ,Y_basic)     , n, cpp_basic                      );

        // Bench fusion
        BENCH(ouverture3_swp_ui16matrix_fusion                     (X, 0, h, 0, w0, X_P_bas_fusion,                  Y_P_bas_fusion,           Y_fusion_bas                     )     , n, cpp_fusion                      );
        BENCH(ouverture3_swp_ui16matrix_fusion_ilu5_red            (X, 0, h, 0, w0, X_P_ilu5_red_fusion,             Y_P_ilu5_red_fusion,           Y_fusion_ilu5_red  )         , n, cpp_fusion_ilu5_red                      );
        BENCH(ouverture3_swp_ui16matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w0, X_P_ilu5_elu2_red_fusion,        Y_P_ilu5_elu2_red_fusion,           Y_fusion_ilu5_elu2_red)  , n, cpp_fusion_ilu5_elu2_red                      );

        // Bench pipeline
        BENCH(    ouverture3_swp_ui16matrix_pipeline_basic               (X, 0, h, 0, w1, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             ), n, cpp_pipeline_basic               );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_red                 (X, 0, h, 0, w1, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             ), n, cpp_pipeline_red                 );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_ilu3_red            (X, 0, h, 0, w1, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             ), n, cpp_pipeline_ilu3_red            );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_elu2_red            (X, 0, h, 0, w1, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            ), n, cpp_pipeline_elu2_red            );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w1, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    ), n, cpp_pipeline_elu2_red_factor     );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w1, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       ), n, cpp_pipeline_ilu3_elu2_red       );
        BENCH(    ouverture3_swp_ui16matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w1, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      ), n, cpp_pipeline_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");

        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d    ", n);

        // basic
        printf(format, cpp_basic                      );
        fprintf(output_file, "%.1f  ", cpp_basic);

        // fusion
        printf(format, cpp_fusion                     );
                fprintf(output_file, "%.1f  ", cpp_fusion);

        printf(format, cpp_fusion_ilu5_red            );
                fprintf(output_file, "%.1f  ", cpp_fusion_ilu5_red);

        printf(format, cpp_fusion_ilu5_elu2_red       );
                fprintf(output_file, "%.1f  ", cpp_fusion_ilu5_elu2_red);


        printf("   ");

        // pipeline
        printf(format, cpp_pipeline_basic                      );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_basic);

        printf(format, cpp_pipeline_red                        );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_red);

        printf(format, cpp_pipeline_ilu3_red                   );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_ilu3_red);

        printf(format, cpp_pipeline_elu2_red                   );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_elu2_red);

        printf(format, cpp_pipeline_elu2_red_factor            );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_elu2_red_factor);
        
        printf(format, cpp_pipeline_ilu3_elu2_red              );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_ilu3_elu2_red);

        printf(format, cpp_pipeline_ilu3_elu2_red_factor       );
                        fprintf(output_file, "%.1f", cpp_pipeline_ilu3_elu2_red_factor);


        putchar('\n');
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}
// ------------------------------ Bench SWP32 ouverture ----------------------------------
void bench_swp32_morpho_ouverture(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp32_OUVERTURE.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 32; if(w0 % 32) w8 = w8+1;
    int w1 = 32 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **T_basic;
    uint8 **Y_basic;

    // POUR FUSION -----------------------------
    // X packé dans X_P fusion
    uint32 **X_P_bas_fusion                   ,
        **X_P_ilu5_red_fusion                ,
        **X_P_ilu5_elu2_red_fusion           ,
        **X_P_ilu5_elu2_red_factor_fusion    ;

    // XP traité dans Y_P
    uint32 **Y_P_bas_fusion                             ,
        **Y_P_ilu5_red_fusion                          ,
        **Y_P_ilu5_elu2_red_fusion                     ,
        **Y_P_ilu5_elu2_red_factor_fusion              ;

    // stockage final Y_fusion
    uint8 **Y_fusion_bas                      ,
        **Y_fusion_ilu5_red                   ,
        **Y_fusion_ilu5_elu2_red              ,
        **Y_fusion_ilu5_elu2_red_factor       ;

    // POUR PIPELINE -----------------------------
    // X packée dans X_P
    uint32 **X_P_bas                   ,
        **X_P_red                     ,
        **X_P_ilu3_red                ,
        **X_P_elu2_red                ,
        **X_P_elu2_red_factor         ,
        **X_P_ilu3_elu2_red           ,
        **X_P_ilu3_elu2_red_factor    ;

    // premier traitement T_P
    uint32 **T_P_bas                   ,
        **T_P_red                     ,
        **T_P_ilu3_red                ,
        **T_P_elu2_red                ,
        **T_P_elu2_red_factor         ,
        **T_P_ilu3_elu2_red           ,
        **T_P_ilu3_elu2_red_factor    ;

    // deuxieme traitement Y_P
    uint32 **Y_P_bas                             ,
        **Y_P_red                               ,
        **Y_P_ilu3_red                          ,
        **Y_P_elu2_red                          ,
        **Y_P_elu2_red_factor                   ,
        **Y_P_ilu3_elu2_red                     ,
        **Y_P_ilu3_elu2_red_factor              ;

    // stockage final pipeline
    uint8 **Y_pipeline_bas                      ,
        **Y_pipeline_red                        ,
        **Y_pipeline_ilu3_red                   ,
        **Y_pipeline_elu2_red                   ,
        **Y_pipeline_elu2_red_factor            ,
        **Y_pipeline_ilu3_elu2_red              ,
        **Y_pipeline_ilu3_elu2_red_factor       ;

    double cpp_basic;

    double cpp_fusion;
    double cpp_fusion_red;
    double cpp_fusion_ilu5_red;
    double cpp_fusion_elu2_red;
    double cpp_fusion_elu2_red_factor;
    double cpp_fusion_ilu5_elu2_red;
    double cpp_fusion_ilu5_elu2_red_factor;

    double cpp_pipeline_basic;
    double cpp_pipeline_red;
    double cpp_pipeline_ilu3_red;
    double cpp_pipeline_elu2_red;
    double cpp_pipeline_elu2_red_factor;
    double cpp_pipeline_ilu3_elu2_red;
    double cpp_pipeline_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP32_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    // X 2r-border
    X      = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    T_basic = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    // Pipeline alloc
    // X packée pipeline
    X_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // T_P premier traitement pipeline
    T_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y_P final pipeline
    Y_P_bas                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_red                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_elu2_red                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_elu2_red_factor             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_elu2_red               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu3_elu2_red_factor        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y finale pipeline
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    // Fusion alloc
    // X packée fusion
    X_P_bas_fusion                         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_red_fusion                    = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_elu2_red_fusion               = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    X_P_ilu5_elu2_red_factor_fusion        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //  Y final packé
    Y_P_bas_fusion                  = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_red_fusion             = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_elu2_red_fusion        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    Y_P_ilu5_elu2_red_factor_fusion = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    //  Y finale fusion
    Y_fusion_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_fusion_ilu5_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,                     0, h-1, 0, w1-1);

    // Pipeline init
    // X packée pipeline
    zero_ui32matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // T_P premier traitement pipeline
    zero_ui32matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y_P deuxieme traitement pipeline
    zero_ui32matrix(Y_P_bas,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_red,                          0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_elu2_red,                     0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_elu2_red_factor,              0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_elu2_red,                0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu3_elu2_red_factor,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y final pipeline
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);

    // Fusion
    // X packé fusion
    zero_ui32matrix(X_P_bas_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_red_fusion,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_elu2_red_fusion,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(X_P_ilu5_elu2_red_factor_fusion,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y final packé fusion
    zero_ui32matrix(Y_P_bas_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_elu2_red_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(Y_P_ilu5_elu2_red_factor_fusion, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    // Y final fusion
    zero_ui8matrix(Y_fusion_bas ,0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red, 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_fusion_ilu5_elu2_red_factor, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 32;
        w1 = n / 1;
        
        //printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

        // PIPELINE
        // X Packé
        resize_ui32matrix(X_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(X_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );

        // Premier traitement pipeline
        resize_ui32matrix(T_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );

        // Deuxieme traitement pipeline
        resize_ui32matrix(Y_P_bas                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_red                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_ilu3_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_elu2_red                                , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_elu2_red_factor                         , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_ilu3_elu2_red                           , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(Y_P_ilu3_elu2_red_factor                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );

        // Y final pipeline
        resize_ui8matrix(Y_pipeline_bas                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_red                             , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red                        , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_elu2_red_factor                 , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red                   , 0, h-1, 0, w1-1);
        resize_ui8matrix(Y_pipeline_ilu3_elu2_red_factor            , 0, h-1, 0, w1-1);


        // FUSION
        // X packée fusion
        resize_ui32matrix(X_P_bas_fusion                         , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(X_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(X_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(X_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/32);

        // Y finale packée
        resize_ui32matrix(Y_P_bas_fusion                         , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(Y_P_ilu5_red_fusion                    , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(Y_P_ilu5_elu2_red_fusion               , 0, h-1, 0, (w1-1)/32);
        resize_ui32matrix(Y_P_ilu5_elu2_red_factor_fusion        , 0, h-1, 0, (w1-1)/32);

        // Y finale fusion
        resize_ui8matrix(Y_fusion_bas                         , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_red                    , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red               , 0, h-1, 0, (w1-1)/8);
        resize_ui8matrix(Y_fusion_ilu5_elu2_red_factor        , 0, h-1, 0, (w1-1)/8);

        // Bench basique
        BENCH(ouverture3_ui8matrix_basic                     (X, 0, h, 0, w1,                    T_basic  ,Y_basic)     , n, cpp_basic                      );

        // Bench fusion
        BENCH(ouverture3_swp_ui32matrix_fusion                     (X, 0, h, 0, w0, X_P_bas_fusion,                  Y_P_bas_fusion,           Y_fusion_bas                     )     , n, cpp_fusion                      );
        BENCH(ouverture3_swp_ui32matrix_fusion_ilu5_red            (X, 0, h, 0, w0, X_P_ilu5_red_fusion,             Y_P_ilu5_red_fusion,           Y_fusion_ilu5_red  )         , n, cpp_fusion_ilu5_red                      );
        BENCH(ouverture3_swp_ui32matrix_fusion_ilu5_elu2_red       (X, 0, h, 0, w0, X_P_ilu5_elu2_red_fusion,        Y_P_ilu5_elu2_red_fusion,           Y_fusion_ilu5_elu2_red)  , n, cpp_fusion_ilu5_elu2_red                      );

        // Bench pipeline
        BENCH(    ouverture3_swp_ui32matrix_pipeline_basic               (X, 0, h, 0, w1, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             ), n, cpp_pipeline_basic               );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_red                 (X, 0, h, 0, w1, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             ), n, cpp_pipeline_red                 );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_ilu3_red            (X, 0, h, 0, w1, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             ), n, cpp_pipeline_ilu3_red            );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_elu2_red            (X, 0, h, 0, w1, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            ), n, cpp_pipeline_elu2_red            );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_elu2_red_factor     (X, 0, h, 0, w1, X_P_elu2_red_factor , T_P_elu2_red_factor, Y_P_elu2_red_factor, Y_pipeline_elu2_red_factor    ), n, cpp_pipeline_elu2_red_factor     );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red       (X, 0, h, 0, w1, X_P_ilu3_elu2_red, T_P_ilu3_elu2_red, Y_P_ilu3_elu2_red, Y_pipeline_ilu3_elu2_red       ), n, cpp_pipeline_ilu3_elu2_red       );
        BENCH(    ouverture3_swp_ui32matrix_pipeline_ilu3_elu2_red_factor(X, 0, h, 0, w1, X_P_ilu3_elu2_red_factor, T_P_ilu3_elu2_red_factor, Y_P_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor      ), n, cpp_pipeline_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");

        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d    ", n);

        // basic
        printf(format, cpp_basic                      );
        fprintf(output_file, "%.1f  ", cpp_basic);

        // fusion
        printf(format, cpp_fusion                     );
                fprintf(output_file, "%.1f  ", cpp_fusion);

        printf(format, cpp_fusion_ilu5_red            );
                fprintf(output_file, "%.1f  ", cpp_fusion_ilu5_red);

        printf(format, cpp_fusion_ilu5_elu2_red       );
                fprintf(output_file, "%.1f  ", cpp_fusion_ilu5_elu2_red);


        printf("   ");

        // pipeline
        printf(format, cpp_pipeline_basic                      );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_basic);

        printf(format, cpp_pipeline_red                        );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_red);

        printf(format, cpp_pipeline_ilu3_red                   );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_ilu3_red);

        printf(format, cpp_pipeline_elu2_red                   );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_elu2_red);

        printf(format, cpp_pipeline_elu2_red_factor            );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_elu2_red_factor);
        
        printf(format, cpp_pipeline_ilu3_elu2_red              );
                        fprintf(output_file, "%.1f  ", cpp_pipeline_ilu3_elu2_red);

        printf(format, cpp_pipeline_ilu3_elu2_red_factor       );
                        fprintf(output_file, "%.1f", cpp_pipeline_ilu3_elu2_red_factor);


        putchar('\n');
        fprintf(output_file, "\n");
    }
    fclose(output_file);
}
// ------------------------------------------------------------------------------------



// ------------------------------ Appel test SWP8 MAX ----------------------------------
void bench_swp8_morpho_max(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp8_MAX.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **Y_basic;

    uint8 **T_bas                     , **Y_P_bas       , **Y_bas;
    uint8 **T_rot                     , **Y_P_rot       , **Y_rot;
    uint8 **T_red                     , **Y_P_red       , **Y_red;
    uint8 **T_ilu3                    , **Y_P_ilu3      , **Y_ilu3;
    uint8 **T_ilu3r                   , **Y_P_ilu3r     , **Y_ilu3r;
    uint8 **T_elu2r                   , **Y_P_elu2r       , **Y_elu2r;
    uint8 **T_elu2rf                  , **Y_P_elu2rf      , **Y_elu2rf;
    uint8 **T_ilu3_elu2r              , **Y_P_ilu3_elu2r  , **Y_ilu3_elu2r;
    uint8 **T_ilu3_elu2rf             , **Y_P_ilu3_elu2rf , **Y_ilu3_elu2rf;

    double cpp_basic;

    double cpp_bas;
    double cpp_rot;
    double cpp_red;
    double cpp_ilu3;
    double cpp_ilu3_red;
    double cpp_elu2_red;
    double cpp_elu2_red_factor;
    double cpp_ilu3_elu2_red;
    double cpp_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP8_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    X      =  ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    T_bas         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_rot         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_red         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_ilu3        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_ilu3r       = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_elu2r       = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_elu2rf      = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_ilu3_elu2r  = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_ilu3_elu2rf = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    Y_P_bas         = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_rot         = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_red         = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3        = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3r       = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_elu2r       = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_elu2rf      = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3_elu2r  = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3_elu2rf = ui8matrix(0, h-1, 0, (w1-1)/8);

    Y_bas         = ui8matrix(0, h-1, 0, w1-1);
    Y_rot         = ui8matrix(0, h-1, 0, w1-1);
    Y_red         = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3        = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf      = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2r  = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,               0, h-1, 0, w1-1);

    zero_ui8matrix(T_bas,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_rot,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_red,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_ilu3,        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_ilu3r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_elu2r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_elu2rf,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_ilu3_elu2r,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_ilu3_elu2rf, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    zero_ui8matrix(Y_P_rot,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_red,         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2r,  0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/8);
 
    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r,  0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 8;
        w1 = n / 1;

        // printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
        resize_ui8matrix(Y_basic , 0, h-1, 0, w1-1);

        // X PACKE
        resize_ui8matrix(T_bas                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8 );
        resize_ui8matrix(T_rot                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8 );
        resize_ui8matrix(T_red                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8 );
        resize_ui8matrix(T_ilu3                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_ilu3r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_elu2r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_elu2rf                                   , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_ilu3_elu2r                               , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );
        resize_ui8matrix(T_ilu3_elu2rf                              , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/8 );

        // RESULT PACKE
        resize_ui8matrix(Y_P_bas                                      , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_rot                                      , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_red                                      , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_ilu3                                     , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_ilu3r                                    , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_elu2r                                    , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_elu2rf                                   , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_ilu3_elu2r                               , 0, h-1, 0, (w1-1)/8 );
        resize_ui8matrix(Y_P_ilu3_elu2rf                              , 0, h-1, 0, (w1-1)/8 );

        // Y final pipeline
        resize_ui8matrix(Y_bas                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_rot                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_red                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3                                     , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2rf                                   , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2r                               , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2rf                              , 0, h-1, 0, (w1-1) );


        // Bench basic
        BENCH(max3_ui8matrix_basic                     (X, 0, h-1, 0, h-1,                   Y_basic                     )     , n, cpp_basic                      );

        //Bench SWP8
        BENCH(    max3_swp_ui8matrix_basic                       (X, 0, h-1, 0, h-1, T_bas, Y_P_bas, Y_bas) , n, cpp_bas               );
        BENCH(    max3_swp_ui8matrix_rot                         (X, 0, h-1, 0, h-1, T_rot, Y_P_rot, Y_rot), n, cpp_rot                );
        BENCH(    max3_swp_ui8matrix_red                         (X, 0, h-1, 0, h-1, T_red, Y_P_red, Y_red), n, cpp_red                );
        BENCH(    max3_swp_ui8matrix_ilu3                        (X, 0, h-1, 0, h-1, T_ilu3, Y_P_ilu3, Y_ilu3), n, cpp_ilu3            );
        BENCH(    max3_swp_ui8matrix_ilu3_red                    (X, 0, h-1, 0, h-1, T_ilu3r, Y_P_ilu3r, Y_ilu3r), n, cpp_ilu3_red     );
        BENCH(    max3_swp_ui8matrix_elu2_red                    (X, 0, h-1, 0, h-1, T_elu2r, Y_P_elu2r, Y_elu2r), n, cpp_elu2_red      );
        BENCH(    max3_swp_ui8matrix_elu2_red_factor             (X, 0, h-1, 0, h-1, T_elu2rf, Y_P_elu2rf, Y_elu2rf), n, cpp_elu2_red_factor);
        BENCH(    max3_swp_ui8matrix_ilu3_elu2_red               (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r), n, cpp_ilu3_elu2_red        );
        BENCH(    max3_swp_ui8matrix_ilu3_elu2_red_factor        (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf), n, cpp_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");

        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d   ", n);

        // basic 
        printf(format, cpp_basic                      );
        fprintf(output_file, "%.1f   ", cpp_basic);
        printf("   ");

        printf(format, cpp_bas                     );
                fprintf(output_file, "%.1f   ", cpp_bas);
        printf(format, cpp_rot            );
                fprintf(output_file, "%.1f   ", cpp_rot);
        printf(format, cpp_red       );
                fprintf(output_file, "%.1f   ", cpp_red);
        printf(format, cpp_ilu3);
                fprintf(output_file, "%.1f   ", cpp_ilu3);
        printf(format, cpp_ilu3_red           );
                fprintf(output_file, "%.1f   ", cpp_ilu3_red);
        printf(format, cpp_elu2_red           );
                fprintf(output_file, "%.1f   ", cpp_elu2_red);
        printf(format, cpp_elu2_red_factor           );
                fprintf(output_file, "%.1f   ", cpp_elu2_red_factor);
        printf(format, cpp_ilu3_elu2_red           );
                fprintf(output_file, "%.1f   ", cpp_ilu3_elu2_red);
        printf(format, cpp_ilu3_elu2_red_factor           );
                fprintf(output_file, "%.1f", cpp_ilu3_elu2_red_factor);
        fprintf(output_file, "\n");

        putchar('\n');

    }
    fclose(output_file);
}
// ------------------------------ Appel test SWP16 MAX ----------------------------------
void bench_swp16_morpho_max(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp16MAX.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 16; if(w0 % 16) w8 = w8+1;
    int w1 = 16 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **Y_basic;

    uint16**T_bas                     ;
    uint16**T_rot                     ;
    uint16**T_red                     ;
    uint16**T_ilu3                    ;
    uint16**T_ilu3r                   ;
    uint16**T_elu2r                   ;
    uint16**T_elu2rf                  ;
    uint16**T_ilu3_elu2r              ;
    uint16**T_ilu3_elu2rf             ;

    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;

    uint16 **Y_P_bas            ;
    uint16 **Y_P_rot            ;
    uint16 **Y_P_red            ;
    uint16 **Y_P_ilu3           ;
    uint16 **Y_P_ilu3r          ;
    uint16 **Y_P_elu2r          ;
    uint16 **Y_P_elu2rf         ;
    uint16 **Y_P_ilu3_elu2r     ;
    uint16 **Y_P_ilu3_elu2rf    ;

    double cpp_basic;

    double cpp_bas;
    double cpp_rot;
    double cpp_red;
    double cpp_ilu3;
    double cpp_ilu3_red;
    double cpp_elu2_red;
    double cpp_elu2_red_factor;
    double cpp_ilu3_elu2_red;
    double cpp_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP8_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    X      =  ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    T_bas         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_rot         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_red         = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_ilu3        = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_ilu3r       = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_elu2r       = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_elu2rf      = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_ilu3_elu2r  = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    T_ilu3_elu2rf = ui16matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);

    Y_P_bas         = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_rot         = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_red         = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_ilu3        = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_ilu3r       = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_elu2r       = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_elu2rf      = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_ilu3_elu2r  = ui16matrix(0, h-1, 0, (w1-1)/16);
    Y_P_ilu3_elu2rf = ui16matrix(0, h-1, 0, (w1-1)/16);

    Y_bas         = ui8matrix(0, h-1, 0, w1-1);
    Y_rot         = ui8matrix(0, h-1, 0, w1-1);
    Y_red         = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3        = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf      = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2r  = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,               0, h-1, 0, w1-1);

    zero_ui16matrix(T_bas,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_rot,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_red,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_ilu3,        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_ilu3r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_elu2r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_elu2rf,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_ilu3_elu2r,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);
    zero_ui16matrix(T_ilu3_elu2rf, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16);

    zero_ui16matrix(Y_P_rot,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_red,         0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2r,  0, h-1, 0, (w1-1)/16);
    zero_ui16matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/16);
 
    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r,  0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 16;
        w1 = n / 1;

        // printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
        resize_ui8matrix(Y_basic , 0, h-1, 0, w1-1);

        // X PACKE
        resize_ui16matrix(T_bas                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16 );
        resize_ui16matrix(T_rot                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16 );
        resize_ui16matrix(T_red                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/16 );
        resize_ui16matrix(T_ilu3                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_ilu3r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_elu2r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_elu2rf                                   , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_ilu3_elu2r                               , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );
        resize_ui16matrix(T_ilu3_elu2rf                              , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/16 );

        // RESULT PACKE
        resize_ui16matrix(Y_P_bas                                      , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_rot                                      , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_red                                      , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_ilu3                                     , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_ilu3r                                    , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_elu2r                                    , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_elu2rf                                   , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_ilu3_elu2r                               , 0, h-1, 0, (w1-1)/16 );
        resize_ui16matrix(Y_P_ilu3_elu2rf                              , 0, h-1, 0, (w1-1)/16 );

        resize_ui8matrix(Y_bas                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_rot                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_red                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3                                     , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2rf                                   , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2r                               , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2rf                              , 0, h-1, 0, (w1-1) );



        // Bench basic
        BENCH(max3_ui8matrix_basic                     (X, 0, h-1, 0, h-1,                   Y_basic                     )     , n, cpp_basic                      );
        // Bench SWP16
        BENCH(    max3_swp_ui16matrix_basic                       (X, 0, h-1, 0, h-1, T_bas, Y_P_bas, Y_bas) , n, cpp_bas               );
        BENCH(    max3_swp_ui16matrix_rot                         (X, 0, h-1, 0, h-1, T_rot, Y_P_rot, Y_rot), n, cpp_rot                );
        BENCH(    max3_swp_ui16matrix_red                         (X, 0, h-1, 0, h-1, T_red, Y_P_red, Y_red), n, cpp_red                );
        BENCH(    max3_swp_ui16matrix_ilu3                        (X, 0, h-1, 0, h-1, T_ilu3, Y_P_ilu3, Y_ilu3), n, cpp_ilu3            );
        BENCH(    max3_swp_ui16matrix_ilu3_red                    (X, 0, h-1, 0, h-1, T_ilu3r, Y_P_ilu3r, Y_ilu3r), n, cpp_ilu3_red     );
        BENCH(    max3_swp_ui16matrix_elu2_red                    (X, 0, h-1, 0, h-1, T_elu2r, Y_P_elu2r, Y_elu2r), n, cpp_elu2_red      );
        BENCH(    max3_swp_ui16matrix_elu2_red_factor             (X, 0, h-1, 0, h-1, T_elu2rf, Y_P_elu2rf, Y_elu2rf), n, cpp_elu2_red_factor);
        BENCH(    max3_swp_ui16matrix_ilu3_elu2_red               (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r), n, cpp_ilu3_elu2_red        );
        BENCH(    max3_swp_ui16matrix_ilu3_elu2_red_factor        (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf), n, cpp_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");

        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d   ", n);

        // basic
        printf(format, cpp_basic                      );
        printf("   ");
        fprintf(output_file, "%.1f   ", cpp_basic);

        // fusion
        printf(format, cpp_bas                     );
            fprintf(output_file, "%.1f   ", cpp_bas);
        printf(format, cpp_rot            );
            fprintf(output_file, "%.1f   ", cpp_rot);
        printf(format, cpp_red       );
            fprintf(output_file, "%.1f   ", cpp_red);
        printf(format, cpp_ilu3);
            fprintf(output_file, "%.1f   ", cpp_ilu3);
        printf(format, cpp_ilu3_red           );
            fprintf(output_file, "%.1f   ", cpp_ilu3_red);
        printf(format, cpp_elu2_red           );
            fprintf(output_file, "%.1f   ", cpp_elu2_red);
        printf(format, cpp_elu2_red_factor           );
            fprintf(output_file, "%.1f   ", cpp_elu2_red_factor);
        printf(format, cpp_ilu3_elu2_red           );
            fprintf(output_file, "%.1f   ", cpp_ilu3_elu2_red);
        printf(format, cpp_ilu3_elu2_red_factor           );
            fprintf(output_file, "%.1f", cpp_ilu3_elu2_red_factor);

        fprintf(output_file, "\n");
        putchar('\n');

    }
    fclose(output_file);
}
// ------------------------------ Appel test SWP32 MAX ----------------------------------
void bench_swp32_morpho_max(int n0, int n1, int nstep)
{
    // fichiers textes avec résultats
    char str[1000];
    const char* filename = "./bench_txt/bench_swp32_MAX.txt";
    FILE* output_file = fopen(filename, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    int r = 1;
    int h = n1; // max size
    int w = n1; //max size
    int w0 = w;
    int w8 = w0 / 32; if(w0 % 32) w8 = w8+1;
    int w1 = 32 * w8; // w1 >= w

    uint8 **X;
    uint8 **X8, **X1;

    uint8 **Y_basic;

    uint32**T_bas                     ;
    uint32**T_rot                     ;
    uint32**T_red                     ;
    uint32**T_ilu3                    ;
    uint32**T_ilu3r                   ;
    uint32**T_elu2r                   ;
    uint32**T_elu2rf                  ;
    uint32**T_ilu3_elu2r              ;
    uint32**T_ilu3_elu2rf             ;

    uint8 **Y_bas, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2r, **Y_ilu3_elu2rf;

    uint32 **Y_P_bas            ;
    uint32 **Y_P_rot            ;
    uint32 **Y_P_red            ;
    uint32 **Y_P_ilu3           ;
    uint32 **Y_P_ilu3r          ;
    uint32 **Y_P_elu2r          ;
    uint32 **Y_P_elu2rf         ;
    uint32 **Y_P_ilu3_elu2r     ;
    uint32 **Y_P_ilu3_elu2rf    ;

    double cpp_basic;

    double cpp_bas;
    double cpp_rot;
    double cpp_red;
    double cpp_ilu3;
    double cpp_ilu3_red;
    double cpp_elu2_red;
    double cpp_elu2_red_factor;
    double cpp_ilu3_elu2_red;
    double cpp_ilu3_elu2_red_factor;

    char* format = "%8.2f";

    format = "%5.0f";
    format = "%6.1f";

    puts("== bench_morpho_SWP8_ouverture ==");

    //------- Alloc  -------
    puts("malloc");

    X      =  ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    Y_basic = ui8matrix(0, h-1, 0, w1-1);

    T_bas         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_rot         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_red         = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_ilu3        = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_ilu3r       = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_elu2r       = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_elu2rf      = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_ilu3_elu2r  = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    T_ilu3_elu2rf = ui32matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);

    Y_P_bas         = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_rot         = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_red         = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_ilu3        = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_ilu3r       = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_elu2r       = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_elu2rf      = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_ilu3_elu2r  = ui32matrix(0, h-1, 0, (w1-1)/32);
    Y_P_ilu3_elu2rf = ui32matrix(0, h-1, 0, (w1-1)/32);

    Y_bas         = ui8matrix(0, h-1, 0, w1-1);
    Y_rot         = ui8matrix(0, h-1, 0, w1-1);
    Y_red         = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3        = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf      = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2r  = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf = ui8matrix(0, h-1, 0, w1-1);

    //------- init  -------
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(Y_basic,               0, h-1, 0, w1-1);

    zero_ui32matrix(T_bas,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_rot,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_red,         0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_ilu3,        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_ilu3r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_elu2r,       0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_elu2rf,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_ilu3_elu2r,      0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);
    zero_ui32matrix(T_ilu3_elu2rf, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32);

    zero_ui32matrix(Y_P_rot,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_red,         0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3,        0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2r,       0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_elu2rf,      0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2r,  0, h-1, 0, (w1-1)/32);
    zero_ui32matrix(Y_P_ilu3_elu2rf, 0, h-1, 0, (w1-1)/32);
 
    zero_ui8matrix(Y_bas,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2r,  0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    puts("temps de calcul en ccp (cycle/point)");

    for(int n = n0; n <= n1; n += nstep) {

        h = n;
        w8 = n / 32;
        w1 = n / 1;

        // printf("i = %3d\n", n);

        resize_ui8matrix(X , 0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
        resize_ui8matrix(Y_basic , 0, h-1, 0, w1-1);

        // X PACKE
        resize_ui32matrix(T_bas                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32 );
        resize_ui32matrix(T_rot                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32 );
        resize_ui32matrix(T_red                                      , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/32 );
        resize_ui32matrix(T_ilu3                                     , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_ilu3r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_elu2r                                    , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_elu2rf                                   , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_ilu3_elu2r                               , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );
        resize_ui32matrix(T_ilu3_elu2rf                              , 0-1*r, h-1+1*r, 0-1*r, (w1-1+1*r)/32 );

        // RESULT PACKE
        resize_ui32matrix(Y_P_bas                                      , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_rot                                      , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_red                                      , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_ilu3                                     , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_ilu3r                                    , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_elu2r                                    , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_elu2rf                                   , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_ilu3_elu2r                               , 0, h-1, 0, (w1-1)/32 );
        resize_ui32matrix(Y_P_ilu3_elu2rf                              , 0, h-1, 0, (w1-1)/32 );

        resize_ui8matrix(Y_bas                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_rot                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_red                                      , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3                                     , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2r                                    , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_elu2rf                                   , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2r                               , 0, h-1, 0, (w1-1) );
        resize_ui8matrix(Y_ilu3_elu2rf                              , 0, h-1, 0, (w1-1) );



        // Bench basic
        BENCH(max3_ui8matrix_basic                     (X, 0, h-1, 0, h-1,                   Y_basic                     )     , n, cpp_basic                      );
        // Bench SWP32
        BENCH(    max3_swp_ui32matrix_basic                       (X, 0, h-1, 0, h-1, T_bas, Y_P_bas, Y_bas) , n, cpp_bas               );
        BENCH(    max3_swp_ui32matrix_rot                         (X, 0, h-1, 0, h-1, T_rot, Y_P_rot, Y_rot), n, cpp_rot                );
        BENCH(    max3_swp_ui32matrix_red                         (X, 0, h-1, 0, h-1, T_red, Y_P_red, Y_red), n, cpp_red                );
        BENCH(    max3_swp_ui32matrix_ilu3                        (X, 0, h-1, 0, h-1, T_ilu3, Y_P_ilu3, Y_ilu3), n, cpp_ilu3            );
        BENCH(    max3_swp_ui32matrix_ilu3_red                    (X, 0, h-1, 0, h-1, T_ilu3r, Y_P_ilu3r, Y_ilu3r), n, cpp_ilu3_red     );
        BENCH(    max3_swp_ui32matrix_elu2_red                    (X, 0, h-1, 0, h-1, T_elu2r, Y_P_elu2r, Y_elu2r), n, cpp_elu2_red      );
        BENCH(    max3_swp_ui32matrix_elu2_red_factor             (X, 0, h-1, 0, h-1, T_elu2rf, Y_P_elu2rf, Y_elu2rf), n, cpp_elu2_red_factor);
        BENCH(    max3_swp_ui32matrix_ilu3_elu2_red               (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r), n, cpp_ilu3_elu2_red        );
        BENCH(    max3_swp_ui32matrix_ilu3_elu2_red_factor        (X, 0, h-1, 0, h-1, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf), n, cpp_ilu3_elu2_red_factor);

        // /**/
        printf("i = %4d", n);
        printf("   ");


        fwrite("i = ", 1, strlen("i = "), output_file);
        fprintf(output_file, "%d   ", n);

        // basic
        printf(format, cpp_basic                      );
        printf("   ");
        fprintf(output_file, "%.1f          ", cpp_basic);

        printf(format, cpp_bas                     );
                fprintf(output_file, "%.1f   ", cpp_bas);
        printf(format, cpp_rot            );
                fprintf(output_file, "%.1f   ", cpp_rot);
        printf(format, cpp_red       );
                fprintf(output_file, "%.1f   ", cpp_red);
        printf(format, cpp_ilu3);
                fprintf(output_file, "%.1f   ", cpp_ilu3);
        printf(format, cpp_ilu3_red           );
                fprintf(output_file, "%.1f   ", cpp_ilu3_red);
        printf(format, cpp_elu2_red           );
                fprintf(output_file, "%.1f   ", cpp_elu2_red);
        printf(format, cpp_elu2_red_factor           );
                fprintf(output_file, "%.1f   ", cpp_elu2_red_factor);
        printf(format, cpp_ilu3_elu2_red           );
                fprintf(output_file, "%.1f   ", cpp_ilu3_elu2_red);
        printf(format, cpp_ilu3_elu2_red_factor           );
                fprintf(output_file, "%.1f", cpp_ilu3_elu2_red_factor);
        fprintf(output_file, "\n");

        putchar('\n');

    }

    fclose(output_file);

}
// ------------------------------------------------------------------------------------



// ------------------------------ Test SWP --------------------------------------------
int test_swp(int argc, char* argv[]){
    puts("=== test_swap ===");
    // test_swp_morpho_max();
    // test_swp_morpho_min();
    // test_swp_morpho_ouverture();

    // MAX
        // bench_swp8_morpho_max(128, 512, 8);
        // bench_swp8_morpho_max(128, 1024, 8);

        // bench_swp16_morpho_max(128, 512, 8);
        // bench_swp16_morpho_max(128, 1024, 8);

        // bench_swp32_morpho_max(128, 512, 8);
        // bench_swp32_morpho_max(128, 1024, 8);

    // OUVERTURE
        // bench_swp8_morpho_ouverture(128, 512, 8);
        // bench_swp8_morpho_ouverture(128, 1024, 8);

        // bench_swp16_morpho_ouverture(128, 512, 8);
        // bench_swp16_morpho_ouverture(128, 1024, 8);

        bench_swp32_morpho_ouverture(128, 512, 8);
        // bench_swp32_morpho_ouverture(128, 1024, 8);

    return 0;
}
// ------------------------------------------------------------------------------------
