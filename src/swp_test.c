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



// // -------------------------------------------------------------------------------
void rand_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 x0, uint8 x1)
// -------------------------------------------------------------------------------
{
    int d = x1 - x0 + 1; // range [x0, x1]
    
    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {
        
            int x = x0 + rand() % d;
            
            X[i][j] = (uint8) x;
        }
    }
}
// -------------------------------------------------------------------------
void rand1_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, int density)
// -------------------------------------------------------------------------
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
// // ------------------------------------------------------------------------------------
int compare_swp_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y, char* name)
// ------------------------------------------------------------------------------------
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


// // ------------------------------------------------------------------------------------
void test_swp_morpho_max_routine(int h, int w0){
// ------------------------------------------------------------------------------------
    int b = 1; // bord

    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    if(w1 > w0) puts("w1 > w0");
    uint8 **X, **Y_bas_originale, **T, **Y_P;
    uint8 **Y_bas_SWP, **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf,
    ** Y_bas_or_packed;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci
    T             = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_bas_or_packed = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);
    Y_P           = ui8matrix(0, h-1, 0, w1-1);   // résultat mais en packé


    Y_bas_SWP         = ui8matrix(0, h-1, 0, w1-1);
    Y_reg         = ui8matrix(0, h-1, 0, w1-1);
    Y_rot         = ui8matrix(0, h-1, 0, w1-1);
    Y_red         = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3        = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r       = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf      = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(T,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct

    zero_ui8matrix(Y_bas_or_packed,  0-b, h-1+b, 0-b, w1-1+b); // basique 100% correct packée
    zero_ui8matrix(Y_P,         0, h-1, 0, w1-1);              // Y packé

    zero_ui8matrix(Y_bas_SWP,         0, h-1, 0, w1-1);       // basique swp
    zero_ui8matrix(Y_reg,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_rot,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_red,         0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3,        0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2r,       0, h-1, 0, w1-1);
    zero_ui8matrix(Y_elu2rf,      0, h-1, 0, w1-1);
    zero_ui8matrix(Y_ilu3_elu2rf, 0, h-1, 0, w1-1);

    //--------------------- RAND ---------------------
    //puts("rand");
    // deux generateurs aleatoire au choix
    //rand_ui8matrix(X, 0, h-1, 0, w0-1, 1, 255); // niveau de gris [1,255]
    rand1_swp_ui8matrix(X, 0, h-1, 0, w0-1, 20); // binaire [0,1]: pourcentage de point a 1

    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);
    max3_swp_ui8matrix_basic           (X, 0, h, 0, w0, T, Y_P, Y_bas_SWP);

                    // pack_ui8matrix(Y_bas_originale, h, w0, Y_bas_or_packed);  // PACKAGE 100% true
                    // displayM_ui8matrix(Y_bas_or_packed, 0, h-1, 0, (w0-1)/8, "Y bas or pack");  // display 100% true packé

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP,         0, h-1, 0, w0-1, "%5d", "Y bas_swp        ");
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
    puts("-- compare max swp --");
    c = compare_swp_ui8matrix(Y_bas_originale, 0, h-1, 0, w0-1, Y_bas_SWP        , "Y bas_swp                       ");
    
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);
    // free_ui8matrix(T,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_or_packed,    0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_P,                0, h-1, 0, w1-1);


    // free_ui8matrix(Y_bas_SWP        , 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}


// -----------------------
void test_swp_morpho_max(void)
// -----------------------
{
    int h0 = 8;
    int w0 = 16;
    
    int dh = 4;
    int dw = 3;
    
    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            test_swp_morpho_max_routine(h, w);
        }
    }
}


int test_swp(int argc, char* argv[]){
    puts("=== test_swap ===");
    test_swp_morpho_max();
    return 0;
}