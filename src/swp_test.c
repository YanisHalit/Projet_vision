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

    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui8matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui8matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui8matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    max3_swp_ui8matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui8matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui8matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui8matrix_ilu3_red est decommentée
    max3_swp_ui8matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui8matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui8matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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

    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui8matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui8matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui8matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    min3_swp_ui8matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui8matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui8matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui8matrix_ilu3_red est decommentée
    min3_swp_ui8matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui8matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui8matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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
// ------------------------------------------------------------------------------------
{
    int b = 1; // bord
    // avec des ADD
    int r = 1; // rayon des operateurs
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp_morpho_ouverture_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    if(w1 > w0) puts("w1 > w0");
    
    uint8 **X;

    uint8 **X8, **X1;

    uint8 **T_basic;

    uint8 **Y_basic;

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

    // stockage final
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
    
    puts("malloc");
    // X 2r-border
    X                               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    X8                              = ui8matrix(0-2*r, h-1+2*r, 0-1*r, w8-1+1*r);
    uint8 **T_basic_or              = ui8matrix(0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    uint8 **Y_basic_or              = ui8matrix(0, h-1, 0, w1-1);
    // X1     = ui8matrix(0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);

    // X packée
    X_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    X_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    // T_P premier traitement
    T_P_bas                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_red                         = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red                    = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_elu2_red_factor             = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red               = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    T_P_ilu3_elu2_red_factor        = ui8matrix(0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    // Y_P deuxieme traitement
    Y_P_bas                         = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_red                         = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3_red                    = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_elu2_red                    = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_elu2_red_factor             = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3_elu2_red               = ui8matrix(0, h-1, 0, (w1-1)/8);
    Y_P_ilu3_elu2_red_factor        = ui8matrix(0, h-1, 0, (w1-1)/8);

    // Y finale
    Y_pipeline_bas                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_red                  = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red             = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_elu2_red_factor      = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red        = ui8matrix(0, h-1, 0, w1-1);
    Y_pipeline_ilu3_elu2_red_factor = ui8matrix(0, h-1, 0, w1-1);

    // puts("zero X");
    zero_ui8matrix(X,                     0-2*r, h-1+2*r, 0-2*r, w1-1+2*r);
    zero_ui8matrix(T_basic_or,            0-1*r, h-1+1*r, 0-1*r, w1-1+1*r);
    zero_ui8matrix(Y_basic_or,                            0, h-1, 0, w1-1);

    // X packée
    zero_ui8matrix(X_P_bas,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_red,                             0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red,                        0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_elu2_red_factor,                 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red,                   0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(X_P_ilu3_elu2_red_factor,            0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    // T_P premier traitement
    zero_ui8matrix(T_P_bas                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_red                 , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red            , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_elu2_red_factor     , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red       , 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);
    zero_ui8matrix(T_P_ilu3_elu2_red_factor, 0-2*r, h-1+2*r, 0-2*r, (w1-1+2*r)/8);

    // Y_P deuxiem traitement
    zero_ui8matrix(Y_P_bas,                         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_red,                         0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_red,                    0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2_red,                    0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_elu2_red_factor,             0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red,               0, h-1, 0, (w1-1)/8);
    zero_ui8matrix(Y_P_ilu3_elu2_red_factor,        0, h-1, 0, (w1-1)/8);

    //PUTS("zero Y");
    zero_ui8matrix(Y_pipeline_bas               , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_red                 , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w1-1);
    zero_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w1-1);

    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 80);
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 90); // binaire [0,1] (afin d'avoir des '1' apres traitement et pas que des '0');

    ouverture3_ui8matrix_basic                      (X, 0, h, 0, w0, T_basic_or, Y_basic_or                      );
    // --ouverture3_ui8matrix_basic = min puis max
    // ouverture3_ui8matrix_fusion                     (X, 0, h-1, 0, w0-1,          Y_fusion                     );
    // ouverture3_ui8matrix_fusion_ilu5_red            (X, 0, h-1, 0, w0-1,          Y_fusion_ilu5_red            );
    // ouverture3_ui8matrix_fusion_ilu5_elu2_red       (X, 0, h-1, 0, w0-1,          Y_fusion_ilu5_elu2_red       );
    // ouverture3_ui8matrix_fusion_ilu5_elu2_red_factor(X, 0, h-1, 0, w0-1,          Y_fusion_ilu5_elu2_red_factor);
    // ouverture3_ui8matrix_fusion_ilu15_red           (X, 0, h-1, 0, w0-1,          Y_fusion_ilu15_red           );
    
    // display_ui8matrix(X                      ,  0, h-1, 0, w0-1, format1, "X                      ");
    // display_ui8matrix(Y_basic_or                      ,  0, h-1, 0, w0-1, format1, "Y_basic                      ");
    //display_ui8matrix(Y_fusion                     ,  0, h-1, 0, w0-1, format1, "Y_fusion                     ");
    //display_ui8matrix(Y_fusion_ilu5_red            ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_red            ");
    //display_ui8matrix(Y_fusion_ilu5_elu2_red       ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red       ");
    // display_ui8matrix(Y_fusion_ilu5_elu2_red_factor,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu5_elu2_red_factor");
    //display_ui8matrix(Y_fusion_ilu15_red           ,  0, h-1, 0, w0-1, format1, "Y_fusion_ilu15_red           ");

    puts("--------------");
    puts("-- pipeline --");
    puts("--------------");
    
    // T: full-matrix
    // ouverture3_swp_ui8matrix_pipeline_basic               (X, 0, h, 0, w0, X_P_bas , T_P_bas, Y_P_bas, Y_pipeline_bas                             );
    // ouverture3_swp_ui8matrix_pipeline_red                 (X, 0, h, 0, w0, X_P_red , T_P_red, Y_P_red, Y_pipeline_red                             );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_red            (X, 0, h, 0, w0, X_P_ilu3_red , T_P_ilu3_red, Y_P_ilu3_red, Y_pipeline_ilu3_red             );
    ouverture3_swp_ui8matrix_pipeline_elu2_red            (X, 0, h, 0, w0, X_P_elu2_red , T_P_elu2_red, Y_P_elu2_red, Y_pipeline_elu2_red            );
    // ouverture3_swp_ui8matrix_pipeline_elu2_red_factor     (X, 0, h-1, 0, w0-1, T_pipeline_elu2_red_factor     , Y_pipeline_elu2_red_factor     );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red       (X, 0, h-1, 0, w0-1, T_pipeline_ilu3_elu2_red       , Y_pipeline_ilu3_elu2_red       );
    // ouverture3_swp_ui8matrix_pipeline_ilu3_elu2_red_factor(X, 0, h-1, 0, w0-1, T_pipeline_ilu3_elu2_red_factor, Y_pipeline_ilu3_elu2_red_factor);

    display_ui8matrix(Y_basic_or                        , 0, h-1, 0, w0-1, format1, "Y_basic_or                        ");
    // display_ui8matrix(Y_pipeline_bas                    , 0, h-1, 0, w0-1, format1, "Y_pipeline_bas_swp                     ");
    // display_ui8matrix(Y_pipeline_red                    ,  0, h-1, 0, w0-1, format1, "Y_pipeline_red                   ");
    //display_ui8matrix(Y_pipeline_ilu3_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_red            ");
    display_ui8matrix(Y_pipeline_elu2_red            , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red            ");
    //display_ui8matrix(Y_pipeline_elu2_red_factor     , 0, h-1, 0, w0-1, format1, "Y_pipeline_elu2_red_factor     ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red       , 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red       ");
    //display_ui8matrix(Y_pipeline_ilu3_elu2_red_factor, 0, h-1, 0, w0-1, format1, "Y_pipeline_ilu3_elu2_red_factor");
    
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_bas                     , "Y_pipeline_bas_swp8                     ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_red                     , "Y_pipeline_red_swp8                 ");
    // c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_ilu3_red            , "Y_pipeline_ilu3_red_swp8            ");
    c = compare_swp_ui8matrix(Y_basic_or, 0, h-1, 0, w0-1, Y_pipeline_elu2_red            , "Y_pipeline_elu2_red_swp8            ");
    // c = compare_swp_ui8matrix(Y_basic, 0, h-1, 0, w0-1, Y_pipeline_elu2_red_factor     , "Y_pipeline_elu2_red_factor     ");
    // c = compare_swp_ui8matrix(Y_basic, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red       , "Y_pipeline_ilu3_elu2_red       ");
    // c = compare_swp_ui8matrix(Y_basic, 0, h-1, 0, w0-1, Y_pipeline_ilu3_elu2_red_factor, "Y_pipeline_ilu3_elu2_red_factor");
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

    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui16matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui16matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui16matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    max3_swp_ui16matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui16matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui16matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui16matrix_ilu3_red est decommentée
    max3_swp_ui16matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui16matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui16matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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

    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui16matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui16matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui16matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    min3_swp_ui16matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui16matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui16matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui16matrix_ilu3_red est decommentée
    min3_swp_ui16matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui16matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui16matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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

    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui32matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    max3_swp_ui32matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    max3_swp_ui32matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    max3_swp_ui32matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    max3_swp_ui32matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    max3_swp_ui32matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si max3_swp_ui32matrix_ilu3_red est decommentée
    max3_swp_ui32matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    max3_swp_ui32matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    max3_swp_ui32matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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

    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- min3 swp --");
    min3_swp_ui32matrix_basic                       (X, 0, h, 0, w0, T_bas, Y_P_bas, Y_bas);
    min3_swp_ui32matrix_rot                         (X, 0, h, 0, w0, T_rot, Y_P_rot, Y_rot);
    min3_swp_ui32matrix_red                         (X, 0, h, 0, w0, T_red, Y_P_red, Y_red);
    min3_swp_ui32matrix_ilu3                        (X, 0, h, 0, w0, T_ilu3, Y_P_ilu3, Y_ilu3);
    min3_swp_ui32matrix_ilu3_red                    (X, 0, h, 0, w0, T_ilu3r, Y_P_ilu3r, Y_ilu3r);
    min3_swp_ui32matrix_elu2_red                    (X, 0, h, 0, w0, T_elu2r, Y_P_elu2r, Y_elu2r); // fait une segmentation fault si min3_swp_ui32matrix_ilu3_red est decommentée
    min3_swp_ui32matrix_elu2_red_factor             (X, 0, h, 0, w0, T_elu2rf, Y_P_elu2rf, Y_elu2rf);
    min3_swp_ui32matrix_ilu3_elu2_red               (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2r);
    min3_swp_ui32matrix_ilu3_elu2_red_factor        (X, 0, h, 0, w0, T_ilu3_elu2r, Y_P_ilu3_elu2r, Y_ilu3_elu2rf);

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


// ------------------------------ Appel test SWP max ----------------------------------
void test_swp_morpho_max(void)
{
    int h0 = 8;
    int w0 = 16;

    int dh = 4;
    int dw = 496;

    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            test_swp8_morpho_max_routine(h, w);
            test_swp16_morpho_max_routine(h, w);
            test_swp32_morpho_max_routine(h, w);
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
// -----------------------------
{
    int h0 = 4;
    int w0 = 16;
    
    int dh = 4;
    int dw = 3;
    
    dh = dw = 0;
    
    dh = 5; dw = 4;
    h0 = 8; w0 = 32;
    
    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            test_swp8_morpho_ouverture_routine(h, w);
        }
    }
}
// ----------------------
// ------------------------------ Test SWP --------------------------------------------
int test_swp(int argc, char* argv[]){
    puts("=== test_swap ===");
    // test_swp_morpho_max();
    // test_swp_morpho_min();
    test_swp_morpho_ouverture();
    return 0;
}
// ------------------------------------------------------------------------------------
