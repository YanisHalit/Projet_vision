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
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf, **Y_bas_originale,
          **Y_bas_SWP8;

    uint8  **T, **Y_P;


    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T                   = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P                 = ui8matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP8          = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui8matrix(T,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui8matrix(Y_P,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP8,         0, h-1, 0, w1-1);       // basique SWP8
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


    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui8matrix_basic           (X, 0, h, 0, w0, T, Y_P, Y_bas_SWP8);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP8,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP8        ");
    // display_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w0-1, "%5d", "Y bas_swp16        ");
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
    puts("-- compare max swp 8 --");
    c = compare_swp_ui8matrix(Y_bas_SWP8, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp8                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);
    // free_ui8matrix(T,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_P,                0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
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
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf, **Y_bas_originale,
          **Y_bas_SWP8;

    uint8  **T, **Y_P;


    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T                   = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P                 = ui8matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP8          = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui8matrix(T,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui8matrix(Y_P,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP8,         0, h-1, 0, w1-1);       // basique SWP8
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


    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    min3_swp_ui8matrix_basic           (X, 0, h, 0, w0, T, Y_P, Y_bas_SWP8);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP8,         0, h-1, 0, w0-1, "%5d", "Y bas_SWP8        ");
    // display_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w0-1, "%5d", "Y bas_swp16        ");
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
    puts("-- compare min swp 8 --");
    c = compare_swp_ui8matrix(Y_bas_SWP8, 0, h-1, 0, w0-1,  Y_bas_originale, "Y bas_swp8                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);
    // free_ui8matrix(T,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_P,                0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}
// ------------------------------------------------------------------------------------
// ------------------------------ Test SWP16 ------------------------------------------
void test_swp16_morpho_max_routine(int h, int w0)
{
    int b = 1; // bord

    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp16_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X, **Y_bas_originale;
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf,
          **Y_bas_SWP16;

    uint16 **T16, **Y_P16;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T16                 = ui16matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P16               = ui16matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP16         = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui16matrix(T16,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui16matrix(Y_P16,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w1-1);       // basique SWP16
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


    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui16matrix_basic          (X, 0, h, 0, w0, T16, Y_P16, Y_bas_SWP16);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w0-1, "%5d", "Y bas_swp16        ");
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
    puts("-- compare max swp 16 --");
    c = compare_swp_ui8matrix(Y_bas_SWP16, 0, h-1, 0, w0-1, Y_bas_originale, "Y bas_swp16                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);

    // free_ui16matrix(T16,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui16matrix(Y_P16,                0, h-1, 0, w1-1);

    // free_ui8matrix(Y_bas_SWP16        , 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp16_morpho_min_routine(int h, int w0)
// ------------------------------------------------------------------------------------
{
    int b = 1; // bord

    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp16_morpho_min_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X, **Y_bas_originale;
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf,
          **Y_bas_SWP16;

    uint16 **T16, **Y_P16;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T16                 = ui16matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P16               = ui16matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP16         = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui16matrix(T16,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui16matrix(Y_P16,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w1-1);       // basique SWP16
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


    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    min3_swp_ui16matrix_basic          (X, 0, h, 0, w0, T16, Y_P16, Y_bas_SWP16);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP16,         0, h-1, 0, w0-1, "%5d", "Y bas_swp16        ");
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
    puts("-- compare min swp 16 --");
    c = compare_swp_ui8matrix(Y_bas_SWP16, 0, h-1, 0, w0-1, Y_bas_originale, "Y bas_swp16                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);

    // free_ui16matrix(T16,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui16matrix(Y_P16,                0, h-1, 0, w1-1);

    // free_ui8matrix(Y_bas_SWP16        , 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}
// ------------------------------------------------------------------------------------
// ------------------------------ Test SWP32 ------------------------------------------
void test_swp32_morpho_max_routine(int h, int w0)
{
    int b = 1; // bord
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp32_morpho_max_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X, **Y_bas_originale;
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf,
          **Y_bas_SWP32;

    uint32 **T32, **Y_P32;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T32                 = ui32matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P32               = ui32matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP32         = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui32matrix(T32,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui32matrix(Y_P32,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP32,         0, h-1, 0, w1-1);       // basique SWP32
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


    max3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    max3_swp_ui32matrix_basic          (X, 0, h, 0, w0, T32, Y_P32, Y_bas_SWP32);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP32,         0, h-1, 0, w0-1, "%5d", "Y bas_swp32        ");
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
    puts("-- compare max swp 32 --");
    c = compare_swp_ui8matrix(Y_bas_SWP32, 0, h-1, 0, w0-1, Y_bas_originale, "Y bas_swp32                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);

    // free_ui32matrix(T32,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui32matrix(Y_P32,                0, h-1, 0, w1-1);

    // free_ui8matrix(Y_bas_SWP32        , 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}
// ------------------------------------------------------------------------------------
void test_swp32_morpho_min_routine(int h, int w0)
{
    int b = 1; // bord
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    puts("--------------------------------------------------");
    printf("test_swp32_morpho_min_routine h = %d w0 = %d w8 = %d, w1 = %d\n", h, w0, w8, w1);
    // if(w1 > w0) puts("w1 > w0");
    uint8 **X, **Y_bas_originale;
    uint8 **Y_reg, **Y_rot, **Y_red, **Y_ilu3, **Y_ilu3r, **Y_elu2r, **Y_elu2rf, **Y_ilu3_elu2rf,
          **Y_bas_SWP32;

    uint32 **T32, **Y_P32;

    int c; // error

    //--------------------- ALLOC ---------------------
    //puts("malloc");
    X  = ui8matrix(0-b, h-1+b, 0-b, w1-1+b);  // départ
    Y_bas_originale         = ui8matrix(0, h-1, 0, w1-1); // comparer avec celle-ci

    T32                 = ui32matrix(0-b, h-1+b, 0-b, w1-1+b);   // X packé
    Y_P32               = ui32matrix(0, h-1, 0, w1-1);   // résultat mais en packé

    Y_bas_SWP32         = ui8matrix(0, h-1, 0, w1-1);
    Y_reg               = ui8matrix(0, h-1, 0, w1-1);
    Y_rot               = ui8matrix(0, h-1, 0, w1-1);
    Y_red               = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3              = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2r             = ui8matrix(0, h-1, 0, w1-1);
    Y_elu2rf            = ui8matrix(0, h-1, 0, w1-1);
    Y_ilu3_elu2rf       = ui8matrix(0, h-1, 0, w1-1);/**/


    //--------------------- INIT ---------------------
    //puts("zero");
    zero_ui8matrix(X,  0-b, h-1+b, 0-b, w1-1+b);
    zero_ui8matrix(Y_bas_originale,         0, h-1, 0, w1-1); // basique 100% correct
    zero_ui32matrix(T32,  0-b, h-1+b, 0-b, w1-1+b);              // temporaire packée
    zero_ui32matrix(Y_P32,         0, h-1, 0, w1-1);              // Y packé
    zero_ui8matrix(Y_bas_SWP32,         0, h-1, 0, w1-1);       // basique SWP32
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


    min3_ui8matrix_basic               (X, 0, h, 0, w0, Y_bas_originale);

    //--------------------- AFFICHAGE BAS ORIGINALE --------------------
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    //--------------------- APPEL FONCTIONS SWP ---------------------
    // puts("\n-- max3 swp --");
    min3_swp_ui32matrix_basic          (X, 0, h, 0, w0, T32, Y_P32, Y_bas_SWP32);

    //--------------------- AFFICHAGE ---------------------
    // puts("display");
    // display_ui8matrix(X,         0, h-1, 0, w0-1, "%5d", "X bas        ");
    // display_ui8matrix(Y_bas_originale,         0, h-1, 0, w0-1, "%5d", "Y bas originale        "); // Basique 100% correct
    // display_ui8matrix(Y_bas_SWP32,         0, h-1, 0, w0-1, "%5d", "Y bas_swp32        ");
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
    puts("-- compare min swp 32 --");
    c = compare_swp_ui8matrix(Y_bas_SWP32, 0, h-1, 0, w0-1, Y_bas_originale, "Y bas_swp32                       ");
    //--------------------- DESALLOC---------------------
    // puts("-- free --");
    // free_ui8matrix(X,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui8matrix(Y_bas_originale,    0, h-1, 0, w1-1);

    // free_ui32matrix(T32,                  0-b, h-1+b, 0-b, w1-1+b);
    // free_ui32matrix(Y_P32,                0, h-1, 0, w1-1);

    // free_ui8matrix(Y_bas_SWP32        , 0, h-1, 0, w1-1);
    // free_ui8matrix(Y_reg        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_rot        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_red        , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3       , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2r      , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_elu2rf     , 0,   h-1,   0,   w1-1);
    // free_ui8matrix(Y_ilu3_elu2rf, 0,   h-1,   0,   w1-1);
}
// ------------------------------------------------------------------------------------

// ------------------------------ Appel test SWP max ----------------------------------
void test_swp_morpho_max(void)
{
    int h0 = 8;
    int w0 = 16;

    int dh = 4;
    int dw = 3;

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
    int dw = 3;

    for(int h = h0; h <= h0+dh; h++) { // pour tester elu2
        for(int w = w0; w <= w0+dw; w++) { // pour tester ilu3
            test_swp8_morpho_min_routine(h, w);
            test_swp16_morpho_min_routine(h, w);
            test_swp32_morpho_min_routine(h, w);
        }
    }
}
// ------------------------------------------------------------------------------------

// ------------------------------ Test SWP --------------------------------------------
int test_swp(int argc, char* argv[]){
    puts("=== test_swap ===");
    test_swp_morpho_max();
    test_swp_morpho_min();
    return 0;
}
// ------------------------------------------------------------------------------------
