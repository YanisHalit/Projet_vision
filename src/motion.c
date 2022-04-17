/* ---------------- */
/* --- motion.c --- */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "../include/nrtype.h"
#include "../include/nrdef.h"
#include "../include/nrutil.h"

#include "../include/macro_bench.h"
#include "x86intrin.h" // _rdtsc()

#include "../include/sigmadelta.h"
#include "../include/morpho_min.h"
#include "../include/morpho_max.h"
#include "../include/motion.h"
#include "../include/swp.h"
#include "../include/morpho_ouverture.h"
#include "../include/morpho_fermeture.h"

#include <unistd.h>


// -------------------------------------------------------
void init_image(uint8 **X, int i0, int i1, int j0, int j1)
// -------------------------------------------------------
{
    // for debugging
    for(int i = i0; i <= i1; i++) {
        for(int j = j0; j <= j1; j++) {
            X[i][j] = i+j;
        }
    }
}
// ----------------------------------------------------------
void reverse_video(uint8 **X, int i0, int i1, int j0, int j1)
// ----------------------------------------------------------
{
    // for debugging
    for(int i = i0; i <= i1; i++) {
        for(int j = j0; j <= j1; j++) {
            X[i][j] = 255 - X[i][j];
        }
    }
}
// --------------------------------------------------------------------------------------------------
void threshold_ui8matrix(uint8 **X, uint8 t, uint8 valeur, uint8 **Y, int i0, int i1, int j0, int j1)
// --------------------------------------------------------------------------------------------------
{
    for(int i=i0; i<=i1; i++) {
        for(int j=j0; j<=j1; j++) {
            
            if(X[i][j] < t)
                Y[i][j] = 0;
            else
                Y[i][j] = valeur;
        }
    }
}
// --------------------------------------------------------------------------------------------------
void test_PGM(void)
// --------------------------------------------------------------------------------------------------
{
    char *src_path = SEQUENCE_SRC_PATH;
    char *dst_path = SEQUENCE_DST_PATH;
    char *filename = SEQUENCE_FILENAME;
    int ndigit     = SEQUENCE_NDIGIT;
    
    char complete_filenameI[1024];
    char complete_filenameM[1024];
    
    int tstart = SEQUENCE_TSTART;
    int tstop  = SEQUENCE_TSTOP;
    int tstep  = SEQUENCE_TSTEP;
      
    int h = SEQUENCE_HEIGHT;
    int w = SEQUENCE_WIDTH;
    
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;
    
    uint8 **I, **M;
    
    puts("--------------");
    puts("-- test_PGM --");
    puts("--------------");
    
    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;
    
    src_path = "/Users/Mlamise/Study/Projet_vision/Projet_vision/results/"; ////// A ADAPTER SELON CONFIGURATION
    
    generate_path_filename_k_ndigit_extension(dst_path, "I_", tstart, ndigit, "pgm", complete_filenameI);
    generate_path_filename_k_ndigit_extension(dst_path, "M_", tstart, ndigit, "pgm", complete_filenameM);
    
    puts(complete_filenameI);
    puts(complete_filenameM);
    
    I  = ui8matrix(0, h-1, 0, w-1);
    M  = ui8matrix(0, h-1, 0, w-1);
    
    init_image(I, 0, h-1, 0, w-1);
    
    SavePGM_ui8matrix(I, i0, i1, j0, j1, complete_filenameI);
    MLoadPGM_ui8matrix(complete_filenameI, i0, i1, j0, j1, M);
    reverse_video(M, i0+h/4, i1-h/4, j0+w/8, j1-w/8);
    SavePGM_ui8matrix(M, i0, i1, j0, j1, complete_filenameM);
    
    free_ui8matrix(I, 0, h-1, 0, w-1);
    free_ui8matrix(M, 0, h-1, 0, w-1);
}
// ==================================
void motion_detection_morpho_v1(void)
// ==================================
{
    // version sans optimisation servant de base apres validation des operateurs morphologiques sans optimisation
    
    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;
    
    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;
    
    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;
    
    // morpho
    uint8 **Erosion1, **Dilatation1, **Dilatation2, **Erosion2;
    uint8 **Erosion1_8, **Dilatation1_8, **Dilatation2_8, **Erosion2_8;
    // filename for saving data
    
    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];
    
    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];
    
    // -------
    // -- init
    // -------
    // puts("[motion_detection_morpho_v1]: les parametres de la sequence sont dans motion.h");
    // puts("[motion_detection_morpho_v1]: supprimer ce message une fois les parametres configures");
    // puts("[motion_detection_morpho_v1]: bye...");
    // return;
    
    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;
    
    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;
    
    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;
    
    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;
    
    // ----------------
    // -- allocation --
    // ----------------
    
    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}
    
    // image 1bit/pixel
    Erosion1    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1 = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2 = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    
    // image 8bits pour la visualisation
    Erosion1_8    = ui8matrix(i0, i1, j0, j1);
    Erosion2_8    = ui8matrix(i0, i1, j0, j1);
    Dilatation1_8 = ui8matrix(i0, i1, j0, j1);
    Dilatation2_8 = ui8matrix(i0, i1, j0, j1);
    // --------------------
    // -- initialisation --
    // --------------------
    
    // bords exterieurs a zero
    
    zero_ui8matrix(I  , i0,   i1,   j0,   j1);
    zero_ui8matrix(M  , i0,   i1,   j0,   j1);
    zero_ui8matrix(O  , i0,   i1,   j0,   j1);
    zero_ui8matrix(V  , i0,   i1,   j0,   j1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}
    
    zero_ui8matrix(Erosion1   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2, i0-b, i1+b, j0-b, j1+b);
    
    zero_ui8matrix(Erosion1_8   , i0, i1, j0, j1);
    zero_ui8matrix(Erosion2_8   , i0, i1, j0, j1);
    zero_ui8matrix(Dilatation1_8, i0, i1, j0, j1);
    zero_ui8matrix(Dilatation2_8, i0, i1, j0, j1);
    
    // ----------------
    // -- traitement --
    // ----------------
    
    // -- prologue --
    
    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    //puts(complete_filenameI);
        
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);
    
    // fragment de code pour verifier qu'on lit bien les bonnes images
    //SavePGM_ui8matrix(I, i0, i1, j0, j1, "verif.pgm");
    //reverse_video(I, i0+h/4, i1-h/4, j0+w/8, j1-w/8);
    //SavePGM_ui8matrix(I, i0, i1, j0, j1, complete_filename_M);
    
    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);
    
    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {
        
        printf("-- i = %3d ----------\n", t);
        
        generate_path_filename_k_ndigit_extension(src_path, filename, t, ndigit, "pgm", complete_filename_I);
        MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);
    
        // morpho en niveau de gris fonctionnant aussi sur des images 1 bit / pixel
        min3_ui8matrix_basic(E,           i0, i1, j0, j1, Erosion1);
        max3_ui8matrix_basic(Erosion1,    i0, i1, j0, j1, Dilatation1);
        max3_ui8matrix_basic(Dilatation1, i0, i1, j0, j1, Dilatation2);
        min3_ui8matrix_basic(Dilatation2, i0, i1, j0, j1, Erosion2);
    
        // traitement pour visualisation
        threshold_ui8matrix(E,           1, 255, E_8,           i0, i1, j0, j1);
        threshold_ui8matrix(Erosion1,    1, 255, Erosion1_8,    i0, i1, j0, j1);
        threshold_ui8matrix(Erosion2,    1, 255, Erosion2_8,    i0, i1, j0, j1);
        threshold_ui8matrix(Dilatation1, 1, 255, Dilatation1_8, i0, i1, j0, j1);
        threshold_ui8matrix(Dilatation2, 1, 255, Dilatation2_8, i0, i1, j0, j1);
        generate_path_filename_k_ndigit_extension(dst_path, "I_",        t, ndigit, "pgm", complete_filename_I);
        //generate_path_filename_k_ndigit_extension(dst_path, "M_",        t, ndigit, "pgm", complete_filename_M);
        //generate_path_filename_k_ndigit_extension(dst_path, "O_",        t, ndigit, "pgm", complete_filename_O);
        //generate_path_filename_k_ndigit_extension(dst_path, "V_",        t, ndigit, "pgm", complete_filename_V);
        generate_path_filename_k_ndigit_extension(dst_path, "E_",        t, ndigit, "pgm", complete_filename_E);
        generate_path_filename_k_ndigit_extension(dst_path, "Ero1_",     t, ndigit, "pgm", complete_filename_erosion1);
        generate_path_filename_k_ndigit_extension(dst_path, "Ero2_",     t, ndigit, "pgm", complete_filename_erosion2);
        generate_path_filename_k_ndigit_extension(dst_path, "Dil1_",     t, ndigit, "pgm", complete_filename_dilatation1);
        generate_path_filename_k_ndigit_extension(dst_path, "Dil2_",     t, ndigit, "pgm", complete_filename_dilatation2);
        
        //puts(complete_filename_I);
        //puts(complete_filename_M);
        //puts(complete_filename_O);
        //puts(complete_filename_V);
        puts(complete_filename_E);
        
        //puts(complete_filename_erosion1);
        //puts(complete_filename_dilatation1);
        //puts(complete_filename_dilatation2);
        puts(complete_filename_erosion2);
        
        //SavePGM_ui8matrix(I, i0, i1, j0, j1, complete_filename_I);
        //SavePGM_ui8matrix(M, i0, i1, j0, j1, complete_filename_M);
        //SavePGM_ui8matrix(O, i0, i1, j0, j1, complete_filename_O);
        //SavePGM_ui8matrix(V, i0, i1, j0, j1, complete_filename_V);
        //SavePGM_ui8matrix(E, i0, i1, j0, j1, complete_filename_E);
        
        SavePGM_ui8matrix(Erosion1_8   , i0, i1, j0, j1, complete_filename_erosion1   );
        SavePGM_ui8matrix(Dilatation1_8, i0, i1, j0, j1, complete_filename_dilatation1);
        SavePGM_ui8matrix(Dilatation2_8, i0, i1, j0, j1, complete_filename_dilatation2);
        SavePGM_ui8matrix(Erosion2_8   , i0, i1, j0, j1, complete_filename_erosion2   );
    } // t
    
    // ----------
    // -- free --
    // ----------
    
    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);
    
    free_ui8matrix(Erosion1   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2, i0-b, i1+b, j0-b, j1+b);
    
    free_ui8matrix(Erosion1_8   , i0, i1, j0, j1);
    free_ui8matrix(Erosion2_8   , i0, i1, j0, j1);
    free_ui8matrix(Dilatation1_8, i0, i1, j0, j1);
    free_ui8matrix(Dilatation2_8, i0, i1, j0, j1);
}
// ===============================
void bench_motion_detection_morpho(void)
// ===============================
{
    puts("=== Bench Motion ===");

    // fichiers textes avec résultats
    char str[1000];
    const char* fname = "./bench_txt/Motion/bench.txt";
    FILE* output_file = fopen(fname, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;
    
    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;
    
    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;
    
    // cpp
    double cpp_bas, cpp_bas1, cpp_bas2, cpp_bas3, cpp_bas4;
    double cpp_rot, cpp_rot1, cpp_rot2, cpp_rot3, cpp_rot4;
    double cpp_red, cpp_red1, cpp_red2, cpp_red3, cpp_red4;
    double cpp_ilu3, cpp_ilu3_1, cpp_ilu3_2, cpp_ilu3_3, cpp_ilu3_4;
    double cpp_ilu3_red, cpp_ilu3_red_1, cpp_ilu3_red_2, cpp_ilu3_red_3, cpp_ilu3_red_4;
    double cpp_elu2_red, cpp_elu2_red_1, cpp_elu2_red_2, cpp_elu2_red_3, cpp_elu2_red_4;
    double cpp_elu2_red_factor, cpp_elu2_red_factor_1, cpp_elu2_red_factor_2, cpp_elu2_red_factor_3, cpp_elu2_red_factor_4;
    double cpp_ilu3_elu2_red, cpp_ilu3_elu2_red_1, cpp_ilu3_elu2_red_2, cpp_ilu3_elu2_red_3, cpp_ilu3_elu2_red_4;
    double cpp_ilu3_elu2_red_factor, cpp_ilu3_elu2_red_factor_1, cpp_ilu3_elu2_red_factor_2, cpp_ilu3_elu2_red_factor_3, cpp_ilu3_elu2_red_factor_4;


    // morpho
    uint8 **Erosion1_bas, **Dilatation1_bas, **Dilatation2_bas, **Erosion2_bas;
    uint8 **Erosion1_rot, **Dilatation1_rot, **Dilatation2_rot, **Erosion2_rot;
    uint8 **Erosion1_red, **Dilatation1_red, **Dilatation2_red, **Erosion2_red;
    uint8 **Erosion1_ilu3, **Dilatation1_ilu3, **Dilatation2_ilu3, **Erosion2_ilu3;
    uint8 **Erosion1_ilu3_red, **Dilatation1_ilu3_red, **Dilatation2_ilu3_red, **Erosion2_ilu3_red;
    uint8 **Erosion1_elu2_red, **Dilatation1_elu2_red, **Dilatation2_elu2_red, **Erosion2_elu2_red;
    uint8 **Erosion1_elu2_red_factor, **Dilatation1_elu2_red_factor, **Dilatation2_elu2_red_factor, **Erosion2_elu2_red_factor;
    uint8 **Erosion1_ilu3_elu2_red, **Dilatation1_ilu3_elu2_red, **Dilatation2_ilu3_elu2_red, **Erosion2_ilu3_elu2_red;
    uint8 **Erosion1_ilu3_elu2_red_factor, **Dilatation1_ilu3_elu2_red_factor, **Dilatation2_ilu3_elu2_red_factor, **Erosion2_ilu3_elu2_red_factor;


    // filename for saving data
    
    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];
    
    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];

    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;

    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;

    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;

    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;

    // ----------------
    // -- allocation --
    // ----------------

    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // image 1bit/pixel
    Erosion1_bas    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_bas    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_bas = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_bas = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_rot    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_rot    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_rot = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_rot = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_ilu3    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_ilu3    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_ilu3 = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_ilu3 = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_ilu3_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_ilu3_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_ilu3_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_ilu3_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_elu2_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_elu2_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_elu2_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_elu2_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_elu2_red_factor    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_elu2_red_factor    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_elu2_red_factor = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_elu2_red_factor = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_ilu3_elu2_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_ilu3_elu2_red    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_ilu3_elu2_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_ilu3_elu2_red = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    Erosion1_ilu3_elu2_red_factor    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_ilu3_elu2_red_factor    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_ilu3_elu2_red_factor = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_ilu3_elu2_red_factor = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    // --------------------
    // -- initialisation --
    // --------------------

    // bords exterieurs a zero

    zero_ui8matrix(I  , i0,   i1,   j0,   j1);
    zero_ui8matrix(M  , i0,   i1,   j0,   j1);
    zero_ui8matrix(O  , i0,   i1,   j0,   j1);
    zero_ui8matrix(V  , i0,   i1,   j0,   j1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    zero_ui8matrix(Erosion1_bas   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_bas   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_bas, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_bas, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_rot   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_rot   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_rot, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_rot, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_red, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_ilu3   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_ilu3   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_ilu3, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_ilu3, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_ilu3_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_ilu3_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_ilu3_red, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_elu2_red, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_ilu3_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_ilu3_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);

    zero_ui8matrix(Erosion1_ilu3_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_ilu3_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);

    // ----------------
    // -- traitement --
    // ----------------

    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);

    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {

        printf("i = %3d     ", t);
        fprintf(output_file, "%d        ", t);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);

        // Basic
        BENCH(min3_ui8matrix_basic(E,           i0, i1, j0, j1, Erosion1_bas), j1, cpp_bas1);
        BENCH(max3_ui8matrix_basic(Erosion1_bas,    i0, i1, j0, j1, Dilatation1_bas), j1, cpp_bas2);
        BENCH(max3_ui8matrix_basic(Dilatation1_bas, i0, i1, j0, j1, Dilatation2_bas), j1, cpp_bas3);
        BENCH(min3_ui8matrix_basic(Dilatation2_bas, i0, i1, j0, j1, Erosion2_bas), j1, cpp_bas4);

        cpp_bas = cpp_bas1 + cpp_bas2 + cpp_bas3 + cpp_bas4;
        printf("%6.1f", cpp_bas                      );
        fprintf(output_file, "%.1f  ", cpp_bas);
        //

        // Rot
        BENCH(min3_ui8matrix_rot(E,           i0, i1, j0, j1, Erosion1_rot), j1, cpp_rot1);
        BENCH(max3_ui8matrix_rot(Erosion1_rot,    i0, i1, j0, j1, Dilatation1_rot), j1, cpp_rot2);
        BENCH(max3_ui8matrix_rot(Dilatation1_rot, i0, i1, j0, j1, Dilatation2_rot), j1, cpp_rot3);
        BENCH(min3_ui8matrix_rot(Dilatation2_rot, i0, i1, j0, j1, Erosion2_rot), j1, cpp_rot4);

        cpp_rot = cpp_rot1 + cpp_rot2 + cpp_rot3 + cpp_rot4;
        printf("%6.1f", cpp_rot                      );
        fprintf(output_file, "%.1f  ", cpp_rot);
        //

        // Red
        BENCH(min3_ui8matrix_red(E,           i0, i1, j0, j1, Erosion1_red), j1, cpp_red1);
        BENCH(max3_ui8matrix_red(Erosion1_red,    i0, i1, j0, j1, Dilatation1_red), j1, cpp_red2);
        BENCH(max3_ui8matrix_red(Dilatation1_red, i0, i1, j0, j1, Dilatation2_red), j1, cpp_red3);
        BENCH(min3_ui8matrix_red(Dilatation2_red, i0, i1, j0, j1, Erosion2_red), j1, cpp_red4);

        cpp_red = cpp_red1 + cpp_red2 + cpp_red3 + cpp_red4;
        printf("%6.1f", cpp_red                      );
        fprintf(output_file, "%.1f  ", cpp_red);
        //

        // ilu3
        BENCH(min3_ui8matrix_ilu3(E,           i0, i1, j0, j1, Erosion1_ilu3), j1, cpp_ilu3_1);
        BENCH(max3_ui8matrix_ilu3(Erosion1_ilu3,    i0, i1, j0, j1, Dilatation1_ilu3), j1, cpp_ilu3_2);
        BENCH(max3_ui8matrix_ilu3(Dilatation1_ilu3, i0, i1, j0, j1, Dilatation2_ilu3), j1, cpp_ilu3_3);
        BENCH(min3_ui8matrix_ilu3(Dilatation2_ilu3, i0, i1, j0, j1, Erosion2_ilu3), j1, cpp_ilu3_4);

        cpp_ilu3 = cpp_ilu3_1 + cpp_ilu3_2 + cpp_ilu3_3 + cpp_ilu3_4;
        printf("%6.1f", cpp_ilu3                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3);
        //

        // ilu3_red
        BENCH(min3_ui8matrix_ilu3_red(E,           i0, i1, j0, j1, Erosion1_ilu3_red), j1, cpp_ilu3_red_1);
        BENCH(max3_ui8matrix_ilu3_red(Erosion1_ilu3_red,    i0, i1, j0, j1, Dilatation1_ilu3_red), j1, cpp_ilu3_red_2);
        BENCH(max3_ui8matrix_ilu3_red(Dilatation1_ilu3_red, i0, i1, j0, j1, Dilatation2_ilu3_red), j1, cpp_ilu3_red_3);
        BENCH(min3_ui8matrix_ilu3_red(Dilatation2_ilu3_red, i0, i1, j0, j1, Erosion2_ilu3_red), j1, cpp_ilu3_red_4);

        cpp_ilu3_red = cpp_ilu3_red_1 + cpp_ilu3_red_2 + cpp_ilu3_red_3 + cpp_ilu3_red_4;
        printf("%6.1f", cpp_ilu3_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_red);
        //

        // elu2_red
        BENCH(min3_ui8matrix_elu2_red(E,           i0, i1, j0, j1, Erosion1_elu2_red), j1, cpp_elu2_red_1);
        BENCH(max3_ui8matrix_elu2_red(Erosion1_elu2_red,    i0, i1, j0, j1, Dilatation1_elu2_red), j1, cpp_elu2_red_2);
        BENCH(max3_ui8matrix_elu2_red(Dilatation1_elu2_red, i0, i1, j0, j1, Dilatation2_elu2_red), j1, cpp_elu2_red_3);
        BENCH(min3_ui8matrix_elu2_red(Dilatation2_elu2_red, i0, i1, j0, j1, Erosion2_elu2_red), j1, cpp_elu2_red_4);

        cpp_elu2_red = cpp_elu2_red_1 + cpp_elu2_red_2 + cpp_elu2_red_3 + cpp_elu2_red_4;
        printf("%6.1f", cpp_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red);
        //

        // elu2_red_factor
        BENCH(min3_ui8matrix_elu2_red_factor(E,           i0, i1, j0, j1, Erosion1_elu2_red_factor), j1, cpp_elu2_red_factor_1);
        BENCH(max3_ui8matrix_elu2_red_factor(Erosion1_elu2_red_factor,    i0, i1, j0, j1, Dilatation1_elu2_red_factor), j1, cpp_elu2_red_factor_2);
        BENCH(max3_ui8matrix_elu2_red_factor(Dilatation1_elu2_red_factor, i0, i1, j0, j1, Dilatation2_elu2_red_factor), j1, cpp_elu2_red_factor_3);
        BENCH(min3_ui8matrix_elu2_red_factor(Dilatation2_elu2_red_factor, i0, i1, j0, j1, Erosion2_elu2_red_factor), j1, cpp_elu2_red_factor_4);

        cpp_elu2_red_factor = cpp_elu2_red_factor_1 + cpp_elu2_red_factor_2 + cpp_elu2_red_factor_3 + cpp_elu2_red_factor_4;
        printf("%6.1f", cpp_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red_factor);
        //

        // ilu3_elu2_red
        BENCH(min3_ui8matrix_ilu3_elu2_red(E,           i0, i1, j0, j1, Erosion1_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_1);
        BENCH(max3_ui8matrix_ilu3_elu2_red(Erosion1_ilu3_elu2_red,    i0, i1, j0, j1, Dilatation1_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_2);
        BENCH(max3_ui8matrix_ilu3_elu2_red(Dilatation1_ilu3_elu2_red, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_3);
        BENCH(min3_ui8matrix_ilu3_elu2_red(Dilatation2_ilu3_elu2_red, i0, i1, j0, j1, Erosion2_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_4);

        cpp_ilu3_elu2_red = cpp_ilu3_elu2_red_1 + cpp_ilu3_elu2_red_2 + cpp_ilu3_elu2_red_3 + cpp_ilu3_elu2_red_4;
        printf("%6.1f", cpp_ilu3_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red);
        //

        // ilu3_elu2_red_factor
        BENCH(min3_ui8matrix_ilu3_elu2_red_factor(E,           i0, i1, j0, j1, Erosion1_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_1);
        BENCH(max3_ui8matrix_ilu3_elu2_red_factor(Erosion1_ilu3_elu2_red_factor,    i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_2);
        BENCH(max3_ui8matrix_ilu3_elu2_red_factor(Dilatation1_ilu3_elu2_red_factor, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_3);
        BENCH(min3_ui8matrix_ilu3_elu2_red_factor(Dilatation2_ilu3_elu2_red_factor, i0, i1, j0, j1, Erosion2_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_4);

        cpp_ilu3_elu2_red_factor = cpp_ilu3_elu2_red_factor_1 + cpp_ilu3_elu2_red_factor_2 + cpp_ilu3_elu2_red_factor_3 + cpp_ilu3_elu2_red_factor_4;
        printf("%6.1f", cpp_ilu3_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red_factor);
        //


        putchar('\n');
        fprintf(output_file, "\n");
    }
    
    // ----------
    // -- free --
    // ----------
    
    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);
    
    free_ui8matrix(Erosion1_bas   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_bas   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_bas, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_bas, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_rot   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_rot   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_rot, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_rot, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_red, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_ilu3   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_ilu3   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_ilu3, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_ilu3, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_ilu3_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_ilu3_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_ilu3_red, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_elu2_red, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_ilu3_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_ilu3_elu2_red   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(Erosion1_ilu3_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_ilu3_elu2_red_factor   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);

    fclose(output_file);
}
// ===============================
void bench_motion_detection_morpho_SWP8(void)
// ===============================
{
    puts("=== Bench Motion SWP 8 ===");

    // fichiers textes avec résultats
    char str[1000];
    const char* fname = "./bench_txt/Motion/bench_SWP8.txt";
    FILE* output_file = fopen(fname, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;

    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;

    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;

    // cpp
    double cpp_basic, cpp_basic1, cpp_basic2, cpp_basic3, cpp_basic4;
    double cpp_bas, cpp_bas_1, cpp_bas_2, cpp_bas_3, cpp_bas_4;
    double cpp_rot, cpp_rot_1, cpp_rot_2, cpp_rot_3, cpp_rot_4;
    double cpp_red, cpp_red_1, cpp_red_2, cpp_red_3, cpp_red_4;
    double cpp_ilu3, cpp_ilu3_1, cpp_ilu3_2, cpp_ilu3_3, cpp_ilu3_4;
    double cpp_ilu3_red, cpp_ilu3_red_1, cpp_ilu3_red_2, cpp_ilu3_red_3, cpp_ilu3_red_4;
    double cpp_elu2_red, cpp_elu2_red_1, cpp_elu2_red_2, cpp_elu2_red_3, cpp_elu2_red_4;
    double cpp_elu2_red_factor, cpp_elu2_red_factor_1, cpp_elu2_red_factor_2, cpp_elu2_red_factor_3, cpp_elu2_red_factor_4;
    double cpp_ilu3_elu2_red, cpp_ilu3_elu2_red_1, cpp_ilu3_elu2_red_2, cpp_ilu3_elu2_red_3, cpp_ilu3_elu2_red_4;
    double cpp_ilu3_elu2_red_factor, cpp_ilu3_elu2_red_factor_1, cpp_ilu3_elu2_red_factor_2, cpp_ilu3_elu2_red_factor_3, cpp_ilu3_elu2_red_factor_4;



    // basic sans swp
    uint8 **Erosion1_basic, **Dilatation1_basic, **Dilatation2_basic, **Erosion2_basic;
    // swp E packé , ero1, dil1, dil2, ero2, E depacké
    uint8 **E_bas_P, **Erosion1_bas_P, **Dilatation1_bas_P, **Dilatation2_bas_P, **Erosion2_bas_P, **E_bas;
    uint8 **E_rot_P, **Erosion1_rot_P, **Dilatation1_rot_P, **Dilatation2_rot_P, **Erosion2_rot_P, **E_rot;
    uint8 **E_red_P, **Erosion1_red_P, **Dilatation1_red_P, **Dilatation2_red_P, **Erosion2_red_P, **E_red;
    uint8 **E_ilu3_P, **Erosion1_ilu3_P, **Dilatation1_ilu3_P, **Dilatation2_ilu3_P, **Erosion2_ilu3_P, **E_ilu3;
    uint8 **E_ilu3_red_P, **Erosion1_ilu3_red_P, **Dilatation1_ilu3_red_P, **Dilatation2_ilu3_red_P, **Erosion2_ilu3_red_P, **E_ilu3_red;
    uint8 **E_elu2_red_P, **Erosion1_elu2_red_P, **Dilatation1_elu2_red_P, **Dilatation2_elu2_red_P, **Erosion2_elu2_red_P, **E_elu2_red;
    uint8 **E_elu2_red_factor_P, **Erosion1_elu2_red_factor_P, **Dilatation1_elu2_red_factor_P, **Dilatation2_elu2_red_factor_P, **Erosion2_elu2_red_factor_P, **E_elu2_red_factor;
    uint8 **E_ilu3_elu2_red_P, **Erosion1_ilu3_elu2_red_P, **Dilatation1_ilu3_elu2_red_P, **Dilatation2_ilu3_elu2_red_P, **Erosion2_ilu3_elu2_red_P, **E_ilu3_elu2_red;
    uint8 **E_ilu3_elu2_red_factor_P, **Erosion1_ilu3_elu2_red_factor_P, **Dilatation1_ilu3_elu2_red_factor_P, **Dilatation2_ilu3_elu2_red_factor_P, **Erosion2_ilu3_elu2_red_factor_P, **E_ilu3_elu2_red_factor;


    // filename for saving data

    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];
    
    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];

    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;

    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;

    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;

    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;

    int w0 = w;
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    // ----------------
    // -- allocation --
    // ----------------

    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic sans swp
    Erosion1_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    // version swp
    E_bas             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_bas_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_bas_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_bas_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_bas_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_bas_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_rot             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_rot_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_rot_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_rot_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_rot_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_rot_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_red_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_ilu3             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_ilu3_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_ilu3_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_ilu3_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_ilu3_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_ilu3_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_red_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_ilu3_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_ilu3_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_ilu3_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_ilu3_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_elu2_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_elu2_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_elu2_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_elu2_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_factor_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_elu2_red_factor_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_elu2_red_factor_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_elu2_red_factor_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_elu2_red_factor_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_ilu3_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_ilu3_elu2_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_ilu3_elu2_red_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_ilu3_elu2_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_ilu3_elu2_red_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    E_ilu3_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_factor_P           = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion1_ilu3_elu2_red_factor_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Erosion2_ilu3_elu2_red_factor_P    = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation1_ilu3_elu2_red_factor_P = ui8matrix(i0-b, i1+b, j0-b, w8);
    Dilatation2_ilu3_elu2_red_factor_P = ui8matrix(i0-b, i1+b, j0-b, w8);

    // --------------------
    // -- initialisation --
    // --------------------

    // bords exterieurs a zero

    zero_ui8matrix(I  , i0,   i1,   j0,   j1);
    zero_ui8matrix(M  , i0,   i1,   j0,   j1);
    zero_ui8matrix(O  , i0,   i1,   j0,   j1);
    zero_ui8matrix(V  , i0,   i1,   j0,   j1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic
    zero_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    // version swp
    zero_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_bas_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_rot_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);

    zero_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);


    zero_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    zero_ui8matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);

    // ----------------
    // -- traitement --
    // ----------------

    uint8 **Erosion2_8    = ui8matrix(i0, i1, j0, j1);
    zero_ui8matrix(Erosion2_8   , i0, i1, j0, j1);

    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);

    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {

        printf("i = %3d     ", t);
        fprintf(output_file, "%d        ", t);

        generate_path_filename_k_ndigit_extension(src_path, filename, t, ndigit, "pgm", complete_filename_I);
        MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);

        // Basic
        BENCH(min3_ui8matrix_basic(E,           i0, i1, j0, j1, Erosion1_basic), j1, cpp_basic1);
        BENCH(max3_ui8matrix_basic(Erosion1_basic,    i0, i1, j0, j1, Dilatation1_basic), j1, cpp_basic2);
        BENCH(max3_ui8matrix_basic(Dilatation1_basic, i0, i1, j0, j1, Dilatation2_basic), j1, cpp_basic3);
        BENCH(min3_ui8matrix_basic(Dilatation2_basic, i0, i1, j0, j1, Erosion2_basic), j1, cpp_basic4);

        cpp_basic = cpp_basic1 + cpp_basic2 + cpp_basic3 + cpp_basic4;
        printf("%6.1f", cpp_basic                      );
        fprintf(output_file, "%.1f  ", cpp_basic);
        //

        // Basic SWP8
        pack_ui8matrix(E, i1, j1, E_bas_P);
        BENCH(min3_swp_ui8matrix_basic_bench(E,          i0, i1, j0, j1, E_bas_P, Erosion1_bas_P, E_bas), j1, cpp_bas_1);
        BENCH(max3_swp_ui8matrix_basic_bench(E,          i0, i1, j0, j1, Erosion1_bas_P, Dilatation1_bas_P, E_bas), j1, cpp_bas_2);
        BENCH(max3_swp_ui8matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation1_bas_P, Dilatation2_bas_P, E_bas), j1, cpp_bas_3);
        BENCH(min3_swp_ui8matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation2_bas_P, Erosion2_bas_P, E_bas), j1, cpp_bas_4);
        unpack_ui8matrix(Erosion2_bas_P, i1, w8, E_bas);

        // traitement pour visualisation
        threshold_ui8matrix(E_bas,    1, 255, Erosion2_8,    i0, i1, j0, j1);
        generate_path_filename_k_ndigit_extension(dst_path, "ALL_",        t, ndigit, "pgm", complete_filename_E);
        

        cpp_bas = cpp_bas_1 + cpp_bas_2 + cpp_bas_3 + cpp_bas_4;
        printf("%6.1f", cpp_bas                      );
        fprintf(output_file, "%.1f  ", cpp_bas);

        puts(complete_filename_erosion2);

        SavePGM_ui8matrix(Erosion2_8   , i0, i1, j0, j1, complete_filename_erosion2   );

        //

        
        
        
        
        
        
        // rot SWP8
        pack_ui8matrix(E, i1, j1, E_rot_P);
        BENCH(min3_swp_ui8matrix_rot_bench(E,          i0, i1, j0, j1, E_rot_P, Erosion1_rot_P, E_rot), j1, cpp_rot_1);
        BENCH(max3_swp_ui8matrix_rot_bench(E,          i0, i1, j0, j1, Erosion1_rot_P, Dilatation1_rot_P, E_rot), j1, cpp_rot_2);
        BENCH(max3_swp_ui8matrix_rot_bench(E, i0, i1, j0, j1, Dilatation1_rot_P, Dilatation2_rot_P, E_rot), j1, cpp_rot_3);
        BENCH(min3_swp_ui8matrix_rot_bench(E, i0, i1, j0, j1, Dilatation2_rot_P, Erosion2_rot_P, E_rot), j1, cpp_rot_4);
        unpack_ui8matrix(Erosion2_rot_P, i1, w8, E_rot);

        cpp_rot = cpp_rot_1 + cpp_rot_2 + cpp_rot_3 + cpp_rot_4;
        printf("%6.1f", cpp_rot                      );
        fprintf(output_file, "%.1f  ", cpp_rot);
        //

        // red SWP8
        pack_ui8matrix(E, i1, j1, E_red_P);
        BENCH(min3_swp_ui8matrix_red_bench(E,          i0, i1, j0, j1, E_red_P, Erosion1_red_P, E_red), j1, cpp_red_1);
        BENCH(max3_swp_ui8matrix_red_bench(E,          i0, i1, j0, j1, Erosion1_red_P, Dilatation1_red_P, E_red), j1, cpp_red_2);
        BENCH(max3_swp_ui8matrix_red_bench(E, i0, i1, j0, j1, Dilatation1_red_P, Dilatation2_red_P, E_red), j1, cpp_red_3);
        BENCH(min3_swp_ui8matrix_red_bench(E, i0, i1, j0, j1, Dilatation2_red_P, Erosion2_red_P, E_red), j1, cpp_red_4);
        unpack_ui8matrix(Erosion2_red_P, i1, w8, E_red);

        cpp_red = cpp_red_1 + cpp_red_2 + cpp_red_3 + cpp_red_4;
        printf("%6.1f", cpp_red                      );
        fprintf(output_file, "%.1f  ", cpp_red);
        //

        // ilu3 SWP8
        pack_ui8matrix(E, i1, j1, E_ilu3_P);
        BENCH(min3_swp_ui8matrix_ilu3_bench(E,          i0, i1, j0, j1, E_ilu3_P, Erosion1_ilu3_P, E_ilu3), j1, cpp_ilu3_1);
        BENCH(max3_swp_ui8matrix_ilu3_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_P, Dilatation1_ilu3_P, E_ilu3), j1, cpp_ilu3_2);
        BENCH(max3_swp_ui8matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_P, Dilatation2_ilu3_P, E_ilu3), j1, cpp_ilu3_3);
        BENCH(min3_swp_ui8matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_P, Erosion2_ilu3_P, E_ilu3), j1, cpp_ilu3_4);
        unpack_ui8matrix(Erosion2_ilu3_P, i1, w8, E_ilu3);

        cpp_ilu3 = cpp_ilu3_1 + cpp_ilu3_2 + cpp_ilu3_3 + cpp_ilu3_4;
        printf("%6.1f", cpp_ilu3                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3);
        //

        // ilu3_red SWP8
        pack_ui8matrix(E, i1, j1, E_ilu3_red_P);
        BENCH(min3_swp_ui8matrix_ilu3_red_bench(E,          i0, i1, j0, j1, E_ilu3_red_P, Erosion1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_1);
        BENCH(max3_swp_ui8matrix_ilu3_red_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_red_P, Dilatation1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_2);
        BENCH(max3_swp_ui8matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_red_P, Dilatation2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_3);
        BENCH(min3_swp_ui8matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_red_P, Erosion2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_4);
        unpack_ui8matrix(Erosion2_ilu3_red_P, i1, w8, E_ilu3_red);

        cpp_ilu3_red = cpp_ilu3_red_1 + cpp_ilu3_red_2 + cpp_ilu3_red_3 + cpp_ilu3_red_4;
        printf("%6.1f", cpp_ilu3_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_red);
        //

        // elu2_red SWP8
        pack_ui8matrix(E, i1, j1, E_elu2_red_P);
        BENCH(min3_swp_ui8matrix_elu2_red_bench(E,          i0, i1, j0, j1, E_elu2_red_P, Erosion1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_1);
        BENCH(max3_swp_ui8matrix_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_P, Dilatation1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_2);
        BENCH(max3_swp_ui8matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation1_elu2_red_P, Dilatation2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_3);
        BENCH(min3_swp_ui8matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation2_elu2_red_P, Erosion2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_4);
        unpack_ui8matrix(Erosion2_elu2_red_P, i1, w8, E_elu2_red);

        cpp_elu2_red = cpp_elu2_red_1 + cpp_elu2_red_2 + cpp_elu2_red_3 + cpp_elu2_red_4;
        printf("%6.1f", cpp_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red);
        //

        // elu2_red_factor SWP8
        pack_ui8matrix(E, i1, j1, E_elu2_red_factor_P);
        BENCH(min3_swp_ui8matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_elu2_red_factor_P, Erosion1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_1);
        BENCH(max3_swp_ui8matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_factor_P, Dilatation1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_2);
        BENCH(max3_swp_ui8matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation1_elu2_red_factor_P, Dilatation2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_3);
        BENCH(min3_swp_ui8matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation2_elu2_red_factor_P, Erosion2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_4);
        unpack_ui8matrix(Erosion2_elu2_red_factor_P, i1, w8, E_elu2_red_factor);

        cpp_elu2_red_factor = cpp_elu2_red_factor_1 + cpp_elu2_red_factor_2 + cpp_elu2_red_factor_3 + cpp_elu2_red_factor_4;
        printf("%6.1f", cpp_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red_factor);
        //

        // ilu3_elu2_red SWP8
        pack_ui8matrix(E, i1, j1, E_ilu3_elu2_red_P);
        BENCH(min3_swp_ui8matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_P, Erosion1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_1);
        BENCH(max3_swp_ui8matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_P, Dilatation1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_2);
        BENCH(max3_swp_ui8matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_P, Dilatation2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_3);
        BENCH(min3_swp_ui8matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_P, Erosion2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_4);
        unpack_ui8matrix(Erosion2_ilu3_elu2_red_P, i1, w8, E_ilu3_elu2_red);

        cpp_ilu3_elu2_red = cpp_ilu3_elu2_red_1 + cpp_ilu3_elu2_red_2 + cpp_ilu3_elu2_red_3 + cpp_ilu3_elu2_red_4;
        printf("%6.1f", cpp_ilu3_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red);
        //

        // ilu3_elu2_red_factor SWP8
        pack_ui8matrix(E, i1, j1, E_ilu3_elu2_red_factor_P);
        BENCH(min3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_factor_P, Erosion1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_1);
        BENCH(max3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_factor_P, Dilatation1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_2);
        BENCH(max3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_factor_P, Dilatation2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_3);
        BENCH(min3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_factor_P, Erosion2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_4);
        unpack_ui8matrix(Erosion2_ilu3_elu2_red_factor_P, i1, w8, E_ilu3_elu2_red_factor);

        cpp_ilu3_elu2_red_factor = cpp_ilu3_elu2_red_factor_1 + cpp_ilu3_elu2_red_factor_2 + cpp_ilu3_elu2_red_factor_3 + cpp_ilu3_elu2_red_factor_4;
        printf("%6.1f", cpp_ilu3_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red_factor);
        //

        putchar('\n');
        fprintf(output_file, "\n");
    }

    // ----------
    // -- free --
    // ----------

    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);

    free_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_bas_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_rot_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);

    free_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w8);


    free_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);
    free_ui8matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w8);

    fclose(output_file);
}
// ===============================
void bench_motion_detection_morpho_SWP16(void)
// ===============================
{
    puts("=== Bench Motion SWP 16 ===");

    // fichiers textes avec résultats
    char str[1000];
    const char* fname = "./bench_txt/Motion/bench_SWP16.txt";
    FILE* output_file = fopen(fname, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;

    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;

    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;

    // cpp
    double cpp_basic, cpp_basic1, cpp_basic2, cpp_basic3, cpp_basic4;
    double cpp_bas, cpp_bas_1, cpp_bas_2, cpp_bas_3, cpp_bas_4;
    double cpp_rot, cpp_rot_1, cpp_rot_2, cpp_rot_3, cpp_rot_4;
    double cpp_red, cpp_red_1, cpp_red_2, cpp_red_3, cpp_red_4;
    double cpp_ilu3, cpp_ilu3_1, cpp_ilu3_2, cpp_ilu3_3, cpp_ilu3_4;
    double cpp_ilu3_red, cpp_ilu3_red_1, cpp_ilu3_red_2, cpp_ilu3_red_3, cpp_ilu3_red_4;
    double cpp_elu2_red, cpp_elu2_red_1, cpp_elu2_red_2, cpp_elu2_red_3, cpp_elu2_red_4;
    double cpp_elu2_red_factor, cpp_elu2_red_factor_1, cpp_elu2_red_factor_2, cpp_elu2_red_factor_3, cpp_elu2_red_factor_4;
    double cpp_ilu3_elu2_red, cpp_ilu3_elu2_red_1, cpp_ilu3_elu2_red_2, cpp_ilu3_elu2_red_3, cpp_ilu3_elu2_red_4;
    double cpp_ilu3_elu2_red_factor, cpp_ilu3_elu2_red_factor_1, cpp_ilu3_elu2_red_factor_2, cpp_ilu3_elu2_red_factor_3, cpp_ilu3_elu2_red_factor_4;



    // basic sans swp
    uint8 **Erosion1_basic, **Dilatation1_basic, **Dilatation2_basic, **Erosion2_basic;
    // swp E packé , ero1, dil1, dil2, ero2, E depacké
    uint8  **E_bas, **E_rot, **E_red, **E_ilu3, **E_ilu3_red, **E_elu2_red, **E_elu2_red_factor, **E_ilu3_elu2_red, **E_ilu3_elu2_red_factor;

    uint16 **E_bas_P, **Erosion1_bas_P, **Dilatation1_bas_P, **Dilatation2_bas_P, **Erosion2_bas_P;
    uint16 **E_rot_P, **Erosion1_rot_P, **Dilatation1_rot_P, **Dilatation2_rot_P, **Erosion2_rot_P;
    uint16 **E_red_P, **Erosion1_red_P, **Dilatation1_red_P, **Dilatation2_red_P, **Erosion2_red_P;
    uint16 **E_ilu3_P, **Erosion1_ilu3_P, **Dilatation1_ilu3_P, **Dilatation2_ilu3_P, **Erosion2_ilu3_P;
    uint16 **E_ilu3_red_P, **Erosion1_ilu3_red_P, **Dilatation1_ilu3_red_P, **Dilatation2_ilu3_red_P, **Erosion2_ilu3_red_P;
    uint16 **E_elu2_red_P, **Erosion1_elu2_red_P, **Dilatation1_elu2_red_P, **Dilatation2_elu2_red_P, **Erosion2_elu2_red_P;
    uint16 **E_elu2_red_factor_P, **Erosion1_elu2_red_factor_P, **Dilatation1_elu2_red_factor_P, **Dilatation2_elu2_red_factor_P, **Erosion2_elu2_red_factor_P;
    uint16 **E_ilu3_elu2_red_P, **Erosion1_ilu3_elu2_red_P, **Dilatation1_ilu3_elu2_red_P, **Dilatation2_ilu3_elu2_red_P, **Erosion2_ilu3_elu2_red_P;
    uint16 **E_ilu3_elu2_red_factor_P, **Erosion1_ilu3_elu2_red_factor_P, **Dilatation1_ilu3_elu2_red_factor_P, **Dilatation2_ilu3_elu2_red_factor_P, **Erosion2_ilu3_elu2_red_factor_P;


    // filename for saving data

    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];
    
    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];

    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;

    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;

    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;

    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;

    int w0 = w;
    int w16 = w0 / 16; if(w0 % 16) w16 = w16+1;
    int w1 = 16 * w16; // w1 >= w

    // ----------------
    // -- allocation --
    // ----------------

    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic sans swp
    Erosion1_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    // version swp
    E_bas             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_bas_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_bas_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_bas_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_bas_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_bas_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_rot             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_rot_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_rot_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_rot_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_rot_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_rot_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_red_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_ilu3             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_ilu3_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_ilu3_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_ilu3_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_ilu3_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_ilu3_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_red_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_ilu3_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_ilu3_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_ilu3_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_ilu3_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_elu2_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_elu2_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_elu2_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_elu2_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_factor_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_elu2_red_factor_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_elu2_red_factor_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_elu2_red_factor_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_elu2_red_factor_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_ilu3_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_ilu3_elu2_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_ilu3_elu2_red_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_ilu3_elu2_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_ilu3_elu2_red_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    E_ilu3_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_factor_P           = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion1_ilu3_elu2_red_factor_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Erosion2_ilu3_elu2_red_factor_P    = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation1_ilu3_elu2_red_factor_P = ui16matrix(i0-b, i1+b, j0-b, w16);
    Dilatation2_ilu3_elu2_red_factor_P = ui16matrix(i0-b, i1+b, j0-b, w16);

    // --------------------
    // -- initialisation --
    // --------------------

    // bords exterieurs a zero

    zero_ui8matrix(I  , i0,   i1,   j0,   j1);
    zero_ui8matrix(M  , i0,   i1,   j0,   j1);
    zero_ui8matrix(O  , i0,   i1,   j0,   j1);
    zero_ui8matrix(V  , i0,   i1,   j0,   j1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic
    zero_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    // version swp
    zero_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_bas_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_rot_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);

    zero_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);


    zero_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui16matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    zero_ui16matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);

    // ----------------
    // -- traitement --
    // ----------------

    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);

    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {

        printf("i = %3d     ", t);
        fprintf(output_file, "%d        ", t);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);

        // Basic
        BENCH(min3_ui8matrix_basic(E,           i0, i1, j0, j1, Erosion1_basic), j1, cpp_basic1);
        BENCH(max3_ui8matrix_basic(Erosion1_basic,    i0, i1, j0, j1, Dilatation1_basic), j1, cpp_basic2);
        BENCH(max3_ui8matrix_basic(Dilatation1_basic, i0, i1, j0, j1, Dilatation2_basic), j1, cpp_basic3);
        BENCH(min3_ui8matrix_basic(Dilatation2_basic, i0, i1, j0, j1, Erosion2_basic), j1, cpp_basic4);

        cpp_basic = cpp_basic1 + cpp_basic2 + cpp_basic3 + cpp_basic4;
        printf("%6.1f", cpp_basic                      );
        fprintf(output_file, "%.1f  ", cpp_basic);
        //

        // Basic SWP16
        pack_ui16matrix(E, i1, j1, E_bas_P);
        BENCH(min3_swp_ui16matrix_basic_bench(E,          i0, i1, j0, j1, E_bas_P, Erosion1_bas_P, E_bas), j1, cpp_bas_1);
        BENCH(max3_swp_ui16matrix_basic_bench(E,          i0, i1, j0, j1, Erosion1_bas_P, Dilatation1_bas_P, E_bas), j1, cpp_bas_2);
        BENCH(max3_swp_ui16matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation1_bas_P, Dilatation2_bas_P, E_bas), j1, cpp_bas_3);
        BENCH(min3_swp_ui16matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation2_bas_P, Erosion2_bas_P, E_bas), j1, cpp_bas_4);
        unpack_ui16matrix(Erosion2_bas_P, i1, w16, E_bas);

        cpp_bas = cpp_bas_1 + cpp_bas_2 + cpp_bas_3 + cpp_bas_4;
        printf("%6.1f", cpp_bas                      );
        fprintf(output_file, "%.1f  ", cpp_bas);
        //

        // rot SWP16
        pack_ui16matrix(E, i1, j1, E_rot_P);
        BENCH(min3_swp_ui16matrix_rot_bench(E,          i0, i1, j0, j1, E_rot_P, Erosion1_rot_P, E_rot), j1, cpp_rot_1);
        BENCH(max3_swp_ui16matrix_rot_bench(E,          i0, i1, j0, j1, Erosion1_rot_P, Dilatation1_rot_P, E_rot), j1, cpp_rot_2);
        BENCH(max3_swp_ui16matrix_rot_bench(E, i0, i1, j0, j1, Dilatation1_rot_P, Dilatation2_rot_P, E_rot), j1, cpp_rot_3);
        BENCH(min3_swp_ui16matrix_rot_bench(E, i0, i1, j0, j1, Dilatation2_rot_P, Erosion2_rot_P, E_rot), j1, cpp_rot_4);
        unpack_ui16matrix(Erosion2_rot_P, i1, w16, E_rot);

        cpp_rot = cpp_rot_1 + cpp_rot_2 + cpp_rot_3 + cpp_rot_4;
        printf("%6.1f", cpp_rot                      );
        fprintf(output_file, "%.1f  ", cpp_rot);
        //

        // red SWP16
        pack_ui16matrix(E, i1, j1, E_red_P);
        BENCH(min3_swp_ui16matrix_red_bench(E,          i0, i1, j0, j1, E_red_P, Erosion1_red_P, E_red), j1, cpp_red_1);
        BENCH(max3_swp_ui16matrix_red_bench(E,          i0, i1, j0, j1, Erosion1_red_P, Dilatation1_red_P, E_red), j1, cpp_red_2);
        BENCH(max3_swp_ui16matrix_red_bench(E, i0, i1, j0, j1, Dilatation1_red_P, Dilatation2_red_P, E_red), j1, cpp_red_3);
        BENCH(min3_swp_ui16matrix_red_bench(E, i0, i1, j0, j1, Dilatation2_red_P, Erosion2_red_P, E_red), j1, cpp_red_4);
        unpack_ui16matrix(Erosion2_red_P, i1, w16, E_red);

        cpp_red = cpp_red_1 + cpp_red_2 + cpp_red_3 + cpp_red_4;
        printf("%6.1f", cpp_red                      );
        fprintf(output_file, "%.1f  ", cpp_red);
        //

        // ilu3 SWP16
        pack_ui16matrix(E, i1, j1, E_ilu3_P);
        BENCH(min3_swp_ui16matrix_ilu3_bench(E,          i0, i1, j0, j1, E_ilu3_P, Erosion1_ilu3_P, E_ilu3), j1, cpp_ilu3_1);
        BENCH(max3_swp_ui16matrix_ilu3_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_P, Dilatation1_ilu3_P, E_ilu3), j1, cpp_ilu3_2);
        BENCH(max3_swp_ui16matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_P, Dilatation2_ilu3_P, E_ilu3), j1, cpp_ilu3_3);
        BENCH(min3_swp_ui16matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_P, Erosion2_ilu3_P, E_ilu3), j1, cpp_ilu3_4);
        unpack_ui16matrix(Erosion2_ilu3_P, i1, w16, E_ilu3);

        cpp_ilu3 = cpp_ilu3_1 + cpp_ilu3_2 + cpp_ilu3_3 + cpp_ilu3_4;
        printf("%6.1f", cpp_ilu3                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3);
        //

        // ilu3_red SWP16
        pack_ui16matrix(E, i1, j1, E_ilu3_red_P);
        BENCH(min3_swp_ui16matrix_ilu3_red_bench(E,          i0, i1, j0, j1, E_ilu3_red_P, Erosion1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_1);
        BENCH(max3_swp_ui16matrix_ilu3_red_bench(E,          i0, i1, j0, j1 , Erosion1_ilu3_red_P, Dilatation1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_2);
        BENCH(max3_swp_ui16matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_red_P, Dilatation2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_3);
        BENCH(min3_swp_ui16matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_red_P, Erosion2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_4);
        unpack_ui16matrix(Erosion2_ilu3_red_P, i1, w16, E_ilu3_red);

        cpp_ilu3_red = cpp_ilu3_red_1 + cpp_ilu3_red_2 + cpp_ilu3_red_3 + cpp_ilu3_red_4;
        printf("%6.1f", cpp_ilu3_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_red);
        //

        // elu2_red SWP16
        pack_ui16matrix(E, i1, j1, E_elu2_red_P);
        BENCH(min3_swp_ui16matrix_elu2_red_bench(E,          i0, i1, j0, j1, E_elu2_red_P, Erosion1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_1);
        BENCH(max3_swp_ui16matrix_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_P, Dilatation1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_2);
        BENCH(max3_swp_ui16matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation1_elu2_red_P, Dilatation2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_3);
        BENCH(min3_swp_ui16matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation2_elu2_red_P, Erosion2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_4);
        unpack_ui16matrix(Erosion2_elu2_red_P, i1, w16, E_elu2_red);

        cpp_elu2_red = cpp_elu2_red_1 + cpp_elu2_red_2 + cpp_elu2_red_3 + cpp_elu2_red_4;
        printf("%6.1f", cpp_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red);
        //

        // elu2_red_factor SWP16
        pack_ui16matrix(E, i1, j1, E_elu2_red_factor_P);
        BENCH(min3_swp_ui16matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_elu2_red_factor_P, Erosion1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_1);
        BENCH(max3_swp_ui16matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_factor_P, Dilatation1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_2);
        BENCH(max3_swp_ui16matrix_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation1_elu2_red_factor_P, Dilatation2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_3);
        BENCH(min3_swp_ui16matrix_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation2_elu2_red_factor_P, Erosion2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_4);
        unpack_ui16matrix(Erosion2_elu2_red_factor_P, i1, w16, E_elu2_red_factor);

        cpp_elu2_red_factor = cpp_elu2_red_factor_1 + cpp_elu2_red_factor_2 + cpp_elu2_red_factor_3 + cpp_elu2_red_factor_4;
        printf("%6.1f", cpp_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red_factor);
        //

        // ilu3_elu2_red SWP16
        pack_ui16matrix(E, i1, j1, E_ilu3_elu2_red_P);
        BENCH(min3_swp_ui16matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_P, Erosion1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_1);
        BENCH(max3_swp_ui16matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_P, Dilatation1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_2);
        BENCH(max3_swp_ui16matrix_ilu3_elu2_red_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_P, Dilatation2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_3);
        BENCH(min3_swp_ui16matrix_ilu3_elu2_red_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_P, Erosion2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_4);
        unpack_ui16matrix(Erosion2_ilu3_elu2_red_P, i1, w16, E_ilu3_elu2_red);

        cpp_ilu3_elu2_red = cpp_ilu3_elu2_red_1 + cpp_ilu3_elu2_red_2 + cpp_ilu3_elu2_red_3 + cpp_ilu3_elu2_red_4;
        printf("%6.1f", cpp_ilu3_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red);
        //

        // ilu3_elu2_red_factor SWP16
        pack_ui16matrix(E, i1, j1, E_ilu3_elu2_red_factor_P);
        BENCH(min3_swp_ui16matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_factor_P, Erosion1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_1);
        BENCH(max3_swp_ui16matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_factor_P, Dilatation1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_2);
        BENCH(max3_swp_ui16matrix_ilu3_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_factor_P, Dilatation2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_3);
        BENCH(min3_swp_ui16matrix_ilu3_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_factor_P, Erosion2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_4);
        unpack_ui16matrix(Erosion2_ilu3_elu2_red_factor_P, i1, w16, E_ilu3_elu2_red_factor);

        cpp_ilu3_elu2_red_factor = cpp_ilu3_elu2_red_factor_1 + cpp_ilu3_elu2_red_factor_2 + cpp_ilu3_elu2_red_factor_3 + cpp_ilu3_elu2_red_factor_4;
        printf("%6.1f", cpp_ilu3_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red_factor);
        //

        putchar('\n');
        fprintf(output_file, "\n");
    }
    
    // ----------
    // -- free --
    // ----------
    
    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);
    
    free_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_bas_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_rot_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w16);

    free_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui16matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);
    free_ui16matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w16);

    fclose(output_file);
}
// ===============================
void bench_motion_detection_morpho_SWP32(void)
// ===============================
{
    puts("=== Bench Motion SWP 32 ===");

    // fichiers textes avec résultats
    char str[1000];
    const char* fname = "./bench_txt/Motion/bench_SWP32.txt";
    FILE* output_file = fopen(fname, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;

    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;

    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;

    // cpp
    double cpp_basic, cpp_basic1, cpp_basic2, cpp_basic3, cpp_basic4;
    double cpp_bas, cpp_bas_1, cpp_bas_2, cpp_bas_3, cpp_bas_4;
    double cpp_rot, cpp_rot_1, cpp_rot_2, cpp_rot_3, cpp_rot_4;
    double cpp_red, cpp_red_1, cpp_red_2, cpp_red_3, cpp_red_4;
    double cpp_ilu3, cpp_ilu3_1, cpp_ilu3_2, cpp_ilu3_3, cpp_ilu3_4;
    double cpp_ilu3_red, cpp_ilu3_red_1, cpp_ilu3_red_2, cpp_ilu3_red_3, cpp_ilu3_red_4;
    double cpp_elu2_red, cpp_elu2_red_1, cpp_elu2_red_2, cpp_elu2_red_3, cpp_elu2_red_4;
    double cpp_elu2_red_factor, cpp_elu2_red_factor_1, cpp_elu2_red_factor_2, cpp_elu2_red_factor_3, cpp_elu2_red_factor_4;
    double cpp_ilu3_elu2_red, cpp_ilu3_elu2_red_1, cpp_ilu3_elu2_red_2, cpp_ilu3_elu2_red_3, cpp_ilu3_elu2_red_4;
    double cpp_ilu3_elu2_red_factor, cpp_ilu3_elu2_red_factor_1, cpp_ilu3_elu2_red_factor_2, cpp_ilu3_elu2_red_factor_3, cpp_ilu3_elu2_red_factor_4;



    // basic sans swp
    uint8 **Erosion1_basic, **Dilatation1_basic, **Dilatation2_basic, **Erosion2_basic;
    // swp E packé , ero1, dil1, dil2, ero2, E depacké
    uint8  **E_bas, **E_rot, **E_red, **E_ilu3, **E_ilu3_red, **E_elu2_red, **E_elu2_red_factor, **E_ilu3_elu2_red, **E_ilu3_elu2_red_factor;

    uint32 **E_bas_P, **Erosion1_bas_P, **Dilatation1_bas_P, **Dilatation2_bas_P, **Erosion2_bas_P;
    uint32 **E_rot_P, **Erosion1_rot_P, **Dilatation1_rot_P, **Dilatation2_rot_P, **Erosion2_rot_P;
    uint32 **E_red_P, **Erosion1_red_P, **Dilatation1_red_P, **Dilatation2_red_P, **Erosion2_red_P;
    uint32 **E_ilu3_P, **Erosion1_ilu3_P, **Dilatation1_ilu3_P, **Dilatation2_ilu3_P, **Erosion2_ilu3_P;
    uint32 **E_ilu3_red_P, **Erosion1_ilu3_red_P, **Dilatation1_ilu3_red_P, **Dilatation2_ilu3_red_P, **Erosion2_ilu3_red_P;
    uint32 **E_elu2_red_P, **Erosion1_elu2_red_P, **Dilatation1_elu2_red_P, **Dilatation2_elu2_red_P, **Erosion2_elu2_red_P;
    uint32 **E_elu2_red_factor_P, **Erosion1_elu2_red_factor_P, **Dilatation1_elu2_red_factor_P, **Dilatation2_elu2_red_factor_P, **Erosion2_elu2_red_factor_P;
    uint32 **E_ilu3_elu2_red_P, **Erosion1_ilu3_elu2_red_P, **Dilatation1_ilu3_elu2_red_P, **Dilatation2_ilu3_elu2_red_P, **Erosion2_ilu3_elu2_red_P;
    uint32 **E_ilu3_elu2_red_factor_P, **Erosion1_ilu3_elu2_red_factor_P, **Dilatation1_ilu3_elu2_red_factor_P, **Dilatation2_ilu3_elu2_red_factor_P, **Erosion2_ilu3_elu2_red_factor_P;


    // filename for saving data

    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];
    
    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];

    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;

    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;

    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;

    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;

    int w0 = w;
    int w32 = w0 / 32; if(w0 % 32) w32 = w32+1;
    int w1 = 32 * w32; // w1 >= w

    // ----------------
    // -- allocation --
    // ----------------

    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic sans swp
    Erosion1_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Erosion2_basic    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation1_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    Dilatation2_basic = ui8matrix(i0-b, i1+b, j0-b, j1+b);

    // version swp
    E_bas             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_bas_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_bas_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_bas_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_bas_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_bas_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_rot             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_rot_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_rot_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_rot_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_rot_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_rot_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_red_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_ilu3             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_ilu3_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_ilu3_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_ilu3_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_ilu3_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_ilu3_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_red_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_ilu3_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_ilu3_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_ilu3_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_ilu3_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_elu2_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_elu2_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_elu2_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_elu2_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_elu2_red_factor_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_elu2_red_factor_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_elu2_red_factor_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_elu2_red_factor_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_elu2_red_factor_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_ilu3_elu2_red             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_ilu3_elu2_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_ilu3_elu2_red_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_ilu3_elu2_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_ilu3_elu2_red_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    E_ilu3_elu2_red_factor             = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    E_ilu3_elu2_red_factor_P           = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion1_ilu3_elu2_red_factor_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Erosion2_ilu3_elu2_red_factor_P    = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation1_ilu3_elu2_red_factor_P = ui32matrix(i0-b, i1+b, j0-b, w32);
    Dilatation2_ilu3_elu2_red_factor_P = ui32matrix(i0-b, i1+b, j0-b, w32);

    // --------------------
    // -- initialisation --
    // --------------------

    // bords exterieurs a zero

    zero_ui8matrix(I  , i0,   i1,   j0,   j1);
    zero_ui8matrix(M  , i0,   i1,   j0,   j1);
    zero_ui8matrix(O  , i0,   i1,   j0,   j1);
    zero_ui8matrix(V  , i0,   i1,   j0,   j1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version basic
    zero_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    zero_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    // version swp
    zero_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_bas_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_rot_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);

    zero_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);


    zero_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    zero_ui32matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    zero_ui32matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);

    // ----------------
    // -- traitement --
    // ----------------

    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);

    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {

        printf("i = %3d     ", t);
        fprintf(output_file, "%d        ", t);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);

        // Basic
        BENCH(min3_ui8matrix_basic(E,           i0, i1, j0, j1, Erosion1_basic), j1, cpp_basic1);
        BENCH(max3_ui8matrix_basic(Erosion1_basic,    i0, i1, j0, j1, Dilatation1_basic), j1, cpp_basic2);
        BENCH(max3_ui8matrix_basic(Dilatation1_basic, i0, i1, j0, j1, Dilatation2_basic), j1, cpp_basic3);
        BENCH(min3_ui8matrix_basic(Dilatation2_basic, i0, i1, j0, j1, Erosion2_basic), j1, cpp_basic4);

        cpp_basic = cpp_basic1 + cpp_basic2 + cpp_basic3 + cpp_basic4;
        printf("%6.1f", cpp_basic                      );
        fprintf(output_file, "%.1f  ", cpp_basic);
        //

        // Basic SWP32
        pack_ui32matrix(E, i1, j1, E_bas_P);
        BENCH(min3_swp_ui32matrix_basic_bench(E,          i0, i1, j0, j1, E_bas_P, Erosion1_bas_P, E_bas), j1, cpp_bas_1);
        BENCH(max3_swp_ui32matrix_basic_bench(E,          i0, i1, j0, j1, Erosion1_bas_P, Dilatation1_bas_P, E_bas), j1, cpp_bas_2);
        BENCH(max3_swp_ui32matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation1_bas_P, Dilatation2_bas_P, E_bas), j1, cpp_bas_3);
        BENCH(min3_swp_ui32matrix_basic_bench(E,          i0, i1, j0, j1, Dilatation2_bas_P, Erosion2_bas_P, E_bas), j1, cpp_bas_4);
        unpack_ui32matrix(Erosion2_bas_P, i1, w32, E_bas);

        cpp_bas = cpp_bas_1 + cpp_bas_2 + cpp_bas_3 + cpp_bas_4;
        printf("%6.1f", cpp_bas                      );
        fprintf(output_file, "%.1f  ", cpp_bas);
        //

        // rot SWP32
        pack_ui32matrix(E, i1, j1, E_rot_P);
        BENCH(min3_swp_ui32matrix_rot_bench(E,          i0, i1, j0, j1, E_rot_P, Erosion1_rot_P, E_rot), j1, cpp_rot_1);
        BENCH(max3_swp_ui32matrix_rot_bench(E,          i0, i1, j0, j1, Erosion1_rot_P, Dilatation1_rot_P, E_rot), j1, cpp_rot_2);
        BENCH(max3_swp_ui32matrix_rot_bench(E, i0, i1, j0, j1, Dilatation1_rot_P, Dilatation2_rot_P, E_rot), j1, cpp_rot_3);
        BENCH(min3_swp_ui32matrix_rot_bench(E, i0, i1, j0, j1, Dilatation2_rot_P, Erosion2_rot_P, E_rot), j1, cpp_rot_4);
        unpack_ui32matrix(Erosion2_rot_P, i1, w32, E_rot);

        cpp_rot = cpp_rot_1 + cpp_rot_2 + cpp_rot_3 + cpp_rot_4;
        printf("%6.1f", cpp_rot                      );
        fprintf(output_file, "%.1f  ", cpp_rot);
        //

        // red SWP32
        pack_ui32matrix(E, i1, j1, E_red_P);
        BENCH(min3_swp_ui32matrix_red_bench(E,          i0, i1, j0, j1, E_red_P, Erosion1_red_P, E_red), j1, cpp_red_1);
        BENCH(max3_swp_ui32matrix_red_bench(E,          i0, i1, j0, j1, Erosion1_red_P, Dilatation1_red_P, E_red), j1, cpp_red_2);
        BENCH(max3_swp_ui32matrix_red_bench(E, i0, i1, j0, j1, Dilatation1_red_P, Dilatation2_red_P, E_red), j1, cpp_red_3);
        BENCH(min3_swp_ui32matrix_red_bench(E, i0, i1, j0, j1, Dilatation2_red_P, Erosion2_red_P, E_red), j1, cpp_red_4);
        unpack_ui32matrix(Erosion2_red_P, i1, w32, E_red);

        cpp_red = cpp_red_1 + cpp_red_2 + cpp_red_3 + cpp_red_4;
        printf("%6.1f", cpp_red                      );
        fprintf(output_file, "%.1f  ", cpp_red);
        //

        // ilu3 SWP32
        pack_ui32matrix(E, i1, j1, E_ilu3_P);
        BENCH(min3_swp_ui32matrix_ilu3_bench(E,          i0, i1, j0, j1, E_ilu3_P, Erosion1_ilu3_P, E_ilu3), j1, cpp_ilu3_1);
        BENCH(max3_swp_ui32matrix_ilu3_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_P, Dilatation1_ilu3_P, E_ilu3), j1, cpp_ilu3_2);
        BENCH(max3_swp_ui32matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_P, Dilatation2_ilu3_P, E_ilu3), j1, cpp_ilu3_3);
        BENCH(min3_swp_ui32matrix_ilu3_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_P, Erosion2_ilu3_P, E_ilu3), j1, cpp_ilu3_4);
        unpack_ui32matrix(Erosion2_ilu3_P, i1, w32, E_ilu3);

        cpp_ilu3 = cpp_ilu3_1 + cpp_ilu3_2 + cpp_ilu3_3 + cpp_ilu3_4;
        printf("%6.1f", cpp_ilu3                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3);
        //

        // ilu3_red SWP32
        pack_ui32matrix(E, i1, j1, E_ilu3_red_P);
        BENCH(min3_swp_ui32matrix_ilu3_red_bench(E,          i0, i1, j0, j1, E_ilu3_red_P, Erosion1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_1);
        BENCH(max3_swp_ui32matrix_ilu3_red_bench(E,          i0, i1, j0, j1 , Erosion1_ilu3_red_P, Dilatation1_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_2);
        BENCH(max3_swp_ui32matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_red_P, Dilatation2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_3);
        BENCH(min3_swp_ui32matrix_ilu3_red_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_red_P, Erosion2_ilu3_red_P, E_ilu3_red), j1, cpp_ilu3_red_4);
        unpack_ui32matrix(Erosion2_ilu3_red_P, i1, w32, E_ilu3_red);

        cpp_ilu3_red = cpp_ilu3_red_1 + cpp_ilu3_red_2 + cpp_ilu3_red_3 + cpp_ilu3_red_4;
        printf("%6.1f", cpp_ilu3_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_red);
        //

        // elu2_red SWP32
        pack_ui32matrix(E, i1, j1, E_elu2_red_P);
        BENCH(min3_swp_ui32matrix_elu2_red_bench(E,          i0, i1, j0, j1, E_elu2_red_P, Erosion1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_1);
        BENCH(max3_swp_ui32matrix_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_P, Dilatation1_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_2);
        BENCH(max3_swp_ui32matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation1_elu2_red_P, Dilatation2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_3);
        BENCH(min3_swp_ui32matrix_elu2_red_bench(E, i0, i1, j0, j1, Dilatation2_elu2_red_P, Erosion2_elu2_red_P, E_elu2_red), j1, cpp_elu2_red_4);
        unpack_ui32matrix(Erosion2_elu2_red_P, i1, w32, E_elu2_red);

        cpp_elu2_red = cpp_elu2_red_1 + cpp_elu2_red_2 + cpp_elu2_red_3 + cpp_elu2_red_4;
        printf("%6.1f", cpp_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red);
        //

        // elu2_red_factor SWP32
        pack_ui32matrix(E, i1, j1, E_elu2_red_factor_P);
        BENCH(min3_swp_ui32matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_elu2_red_factor_P, Erosion1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_1);
        BENCH(max3_swp_ui32matrix_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_elu2_red_factor_P, Dilatation1_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_2);
        BENCH(max3_swp_ui32matrix_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation1_elu2_red_factor_P, Dilatation2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_3);
        BENCH(min3_swp_ui32matrix_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation2_elu2_red_factor_P, Erosion2_elu2_red_factor_P, E_elu2_red_factor), j1, cpp_elu2_red_factor_4);
        unpack_ui32matrix(Erosion2_elu2_red_factor_P, i1, w32, E_elu2_red_factor);

        cpp_elu2_red_factor = cpp_elu2_red_factor_1 + cpp_elu2_red_factor_2 + cpp_elu2_red_factor_3 + cpp_elu2_red_factor_4;
        printf("%6.1f", cpp_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_elu2_red_factor);
        //

        // ilu3_elu2_red SWP32
        pack_ui32matrix(E, i1, j1, E_ilu3_elu2_red_P);
        BENCH(min3_swp_ui32matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_P, Erosion1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_1);
        BENCH(max3_swp_ui32matrix_ilu3_elu2_red_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_P, Dilatation1_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_2);
        BENCH(max3_swp_ui32matrix_ilu3_elu2_red_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_P, Dilatation2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_3);
        BENCH(min3_swp_ui32matrix_ilu3_elu2_red_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_P, Erosion2_ilu3_elu2_red_P, E_ilu3_elu2_red), j1, cpp_ilu3_elu2_red_4);
        unpack_ui32matrix(Erosion2_ilu3_elu2_red_P, i1, w32, E_ilu3_elu2_red);

        cpp_ilu3_elu2_red = cpp_ilu3_elu2_red_1 + cpp_ilu3_elu2_red_2 + cpp_ilu3_elu2_red_3 + cpp_ilu3_elu2_red_4;
        printf("%6.1f", cpp_ilu3_elu2_red                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red);
        //

        // ilu3_elu2_red_factor SWP32
        pack_ui32matrix(E, i1, j1, E_ilu3_elu2_red_factor_P);
        BENCH(min3_swp_ui32matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_factor_P, Erosion1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_1);
        BENCH(max3_swp_ui32matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_factor_P, Dilatation1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_2);
        BENCH(max3_swp_ui32matrix_ilu3_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_factor_P, Dilatation2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_3);
        BENCH(min3_swp_ui32matrix_ilu3_elu2_red_factor_bench(E, i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_factor_P, Erosion2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor), j1, cpp_ilu3_elu2_red_factor_4);
        unpack_ui32matrix(Erosion2_ilu3_elu2_red_factor_P, i1, w32, E_ilu3_elu2_red_factor);

        cpp_ilu3_elu2_red_factor = cpp_ilu3_elu2_red_factor_1 + cpp_ilu3_elu2_red_factor_2 + cpp_ilu3_elu2_red_factor_3 + cpp_ilu3_elu2_red_factor_4;
        printf("%6.1f", cpp_ilu3_elu2_red_factor                      );
        fprintf(output_file, "%.1f  ", cpp_ilu3_elu2_red_factor);
        //

        putchar('\n');
        fprintf(output_file, "\n");
    }
    
    // ----------
    // -- free --
    // ----------
    
    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);
    
    free_ui8matrix(Erosion1_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Erosion2_basic   , i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation1_basic, i0-b, i1+b, j0-b, j1+b);
    free_ui8matrix(Dilatation2_basic, i0-b, i1+b, j0-b, j1+b);

    free_ui8matrix(E_bas, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_bas_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_bas_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_bas_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_bas_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_bas_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_rot, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_rot_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_rot_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_rot_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_rot_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_rot_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_red, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_red_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_ilu3, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_ilu3_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_ilu3_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_ilu3_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_ilu3_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_ilu3_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_ilu3_red, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_ilu3_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_ilu3_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_ilu3_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_ilu3_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_ilu3_red_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_elu2_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_elu2_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_elu2_red_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_ilu3_elu2_red, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_ilu3_elu2_red_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_ilu3_elu2_red_P, i0-b, i1+b, j0-b, w32);

    free_ui8matrix(E_ilu3_elu2_red_factor, i0-b, i1+b, j0-b, j1+b);
    free_ui32matrix(E_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion1_ilu3_elu2_red_factor_P   , i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Erosion2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation1_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);
    free_ui32matrix(Dilatation2_ilu3_elu2_red_factor_P, i0-b, i1+b, j0-b, w32);

    fclose(output_file);
}
// ===============================


// ===============================
void bench_debit_motion_detection_morpho_SWP8(void)
// ===============================
{
    puts("=== Bench Motion SWP 8 ===");

    // fichiers textes avec résultats
    char str[1000];
    const char* fname = "./bench_txt/Motion/bench_debit_SWP8.txt";
    FILE* output_file = fopen(fname, "w+");
    if (!output_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //

    // sequence
    char *src_path;
    char *dst_path;
    char *filename;
    int ndigit;
    int tstart, tstop, tstep;

    // image
    int h, w, b; // height, width, border
    int i0, i1, j0, j1;
    long li0, li1, lj0, lj1;

    // sigma-delta
    uint8 **I, **M, **O, **V, **E, **E_8;

    // swp E packé , ero1, dil1, dil2, ero2, E depacké
    uint8 **E_fusion_ilu5_elu2_red_P, **E_fusion_result_ouverture_ilu5_elu2_red_P, **E_fusion_result_fermeture_ilu5_elu2_red_P, **E_fusion_ilu5_elu2_red;
    uint8 **E_pipeline_ilu3_red_out_P, **E_pipeline_result_ouverture_ilu3_red_out_P, **E_pipeline_result_fermeture_ilu3_red_out_P, **E_pipeline_ilu3_red_out;
    uint8 **E_ilu3_elu2_red_factor_P, **Erosion1_ilu3_elu2_red_factor_P, **Dilatation1_ilu3_elu2_red_factor_P, **Dilatation2_ilu3_elu2_red_factor_P, **Erosion2_ilu3_elu2_red_factor_P, **E_ilu3_elu2_red_factor;

    uint8 **E_T_pipeline_result_ouverture_ilu3_red_out_P, **E_T_pipeline_result_fermeture_ilu3_red_out_P;

    // filename for saving data

    char complete_filename_I[1024];
    char complete_filename_M[1024];
    char complete_filename_O[1024];
    char complete_filename_V[1024];
    char complete_filename_E[1024];

    char complete_filename_erosion1[1024];
    char complete_filename_erosion2[1024];
    char complete_filename_dilatation1[1024];
    char complete_filename_dilatation2[1024];

    src_path = SEQUENCE_SRC_PATH;
    filename = SEQUENCE_FILENAME;
    ndigit   = SEQUENCE_NDIGIT;

    tstart = SEQUENCE_TSTART;
    tstop  = SEQUENCE_TSTOP;
    tstep  = SEQUENCE_TSTEP;

    h = SEQUENCE_HEIGHT;
    w = SEQUENCE_WIDTH;
    b = 1;
    dst_path = SEQUENCE_DST_PATH;

    i0 = 0; i1 = h-1; j0 = 0; j1 = w-1;

    int w0 = w;
    int w8 = w0 / 8; if(w0 % 8) w8 = w8+1;
    int w1 = 8 * w8; // w1 >= w

    // ----------------
    // -- allocation --
    // ----------------

    I   = ui8matrix(i0,   i1,   j0,   j1);
    M   = ui8matrix(i0,   i1,   j0,   j1);
    O   = ui8matrix(i0,   i1,   j0,   j1);
    V   = ui8matrix(i0,   i1,   j0,   j1);
    E   = ui8matrix(i0-b, i1+b, j0-b, j1+b); // image 1 bit en binaire {0,1}
    E_8 = ui8matrix(i0,   i1,   j0,   j1); // image 8 bits en, niveau de gris {0,255}

    // version swp
    E_fusion_ilu5_elu2_red_P                        = ui8matrix(i0-2, i1+2, j0-2, w8+2);
    E_fusion_result_ouverture_ilu5_elu2_red_P       = ui8matrix(i0-2, i1+2, j0-2, w8+2);
    E_fusion_result_fermeture_ilu5_elu2_red_P       = ui8matrix(i0, i1, j0, w8);
    E_fusion_ilu5_elu2_red                          = ui8matrix(i0, i1, j0, j1);

    E_pipeline_ilu3_red_out_P                        = ui8matrix(i0-2, i1+2, j0-2, w8+2);

    E_T_pipeline_result_ouverture_ilu3_red_out_P       = ui8matrix(i0-2, i1+2, j0-2, w8+2);
    E_pipeline_result_ouverture_ilu3_red_out_P       = ui8matrix(i0-2, i1+2, j0-2, w8+2);

    E_pipeline_result_fermeture_ilu3_red_out_P       = ui8matrix(i0, i1, j0, w8);
    E_T_pipeline_result_fermeture_ilu3_red_out_P       = ui8matrix(i0-2, i1+2, j0-2, w8+2);

    E_pipeline_ilu3_red_out                          = ui8matrix(i0, i1, j0, j1);

    E_ilu3_elu2_red_factor_P           = ui8matrix(i0-1, i1+1, j0-1, w8+1);
    Erosion1_ilu3_elu2_red_factor_P    = ui8matrix(i0-1, i1+1, j0-1, w8+1);
    Erosion2_ilu3_elu2_red_factor_P    = ui8matrix(i0-1, i1+1, j0-1, w8+1);
    Dilatation1_ilu3_elu2_red_factor_P = ui8matrix(i0-1, i1+1, j0-1, w8+1);
    Dilatation2_ilu3_elu2_red_factor_P = ui8matrix(i0-1, i1+1, j0-1, w8+1);
    E_ilu3_elu2_red_factor             = ui8matrix(i0-1, i1+1, j0-1, w1+1);


    // --------------------
    // -- initialisation --
    // --------------------

    // bords exterieurs a zero
    zero_ui8matrix(I  , i0,   i1,   j0,   w1);
    zero_ui8matrix(M  , i0,   i1,   j0,   w1);
    zero_ui8matrix(O  , i0,   i1,   j0,   w1);
    zero_ui8matrix(V  , i0,   i1,   j0,   w1);
    zero_ui8matrix(E  , i0-b, i1+b, j0-b, w1+b); // image 1 bit en binaire {0,1}
    zero_ui8matrix(E_8, i0,   i1,   j0,   w1); // image 8 bits en, niveau de gris {0,255}

    // version swp
    zero_ui8matrix(E_fusion_ilu5_elu2_red_P                         ,i0-2, i1+2, j0-2, w8+2);
    zero_ui8matrix(E_fusion_result_ouverture_ilu5_elu2_red_P        ,i0-2, i1+2, j0-2, w8+2);
    zero_ui8matrix(E_fusion_result_fermeture_ilu5_elu2_red_P        ,i0, i1, j0, w8);
    zero_ui8matrix(E_fusion_ilu5_elu2_red                           ,i0, i1, j0, j1);


    zero_ui8matrix(E_T_pipeline_result_ouverture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);
    zero_ui8matrix(E_T_pipeline_result_fermeture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);

    zero_ui8matrix(E_pipeline_ilu3_red_out_P                         ,i0-2, i1+2, j0-2, w8+2);
    zero_ui8matrix(E_pipeline_result_ouverture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);
    zero_ui8matrix(E_pipeline_result_fermeture_ilu3_red_out_P        ,i0, i1, j0, w8);
    zero_ui8matrix(E_pipeline_ilu3_red_out                           ,i0, i1, j0, j1);


    zero_ui8matrix(E_ilu3_elu2_red_factor,                  i0-1, i1+1, j0-1, w1+1);
    zero_ui8matrix(E_ilu3_elu2_red_factor_P   ,             i0-1, i1+1, j0-1, w1+1);
    zero_ui8matrix(Erosion1_ilu3_elu2_red_factor_P   ,      i0-1, i1+1, j0-1, w1+1);
    zero_ui8matrix(Erosion2_ilu3_elu2_red_factor_P,         i0-1, i1+1, j0-1, w1+1);
    zero_ui8matrix(Dilatation1_ilu3_elu2_red_factor_P,      i0-1, i1+1, j0-1, w1+1);
    zero_ui8matrix(Dilatation2_ilu3_elu2_red_factor_P,      i0-1, i1+1, j0-1, w1+1);

    // ----------------
    // -- traitement --
    // ----------------

    generate_path_filename_k_ndigit_extension(src_path, filename, tstart, ndigit, "pgm", complete_filename_I);
    MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);

    SigmaDelta_Step0(I, M, O, V, E, i0, i1, j0, j1);

    // -- boucle --
    for(int t=tstart; t<=tstop; t+=tstep) {

        generate_path_filename_k_ndigit_extension(src_path, filename, t, ndigit, "pgm", complete_filename_I);
        MLoadPGM_ui8matrix(complete_filename_I, i0, i1, j0, j1, I);


        printf("i = %3d     ", t);
        fprintf(output_file, "%d        ", t);

        // N = 3 ecart type autour de la moyenne
        SigmaDelta_1Step(I, M, O, V, E, 3, i0, i1, j0, j1);

       // ------------
        // ilu3_elu2_red_factor SWP8 classique
        pack_ui8matrix(E, i1, j1, E_ilu3_elu2_red_factor_P);

        float temps;
        clock_t t1, t2;
 
        t1 = clock();

        min3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, E_ilu3_elu2_red_factor_P, Erosion1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor);
        max3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Erosion1_ilu3_elu2_red_factor_P, Dilatation1_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor);
        max3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation1_ilu3_elu2_red_factor_P, Dilatation2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor);
        min3_swp_ui8matrix_ilu3_elu2_red_factor_bench(E,          i0, i1, j0, j1, Dilatation2_ilu3_elu2_red_factor_P, Erosion2_ilu3_elu2_red_factor_P, E_ilu3_elu2_red_factor);

        t2 = clock();
        temps = (float)(t2-t1)/CLOCKS_PER_SEC;

        double debit_classique =  temps;

        unpack_ui8matrix(Erosion2_ilu3_elu2_red_factor_P, i1, w8, E_ilu3_elu2_red_factor);

        printf("%f        ",                debit_classique);
        fprintf(output_file, "%lf      ",   debit_classique);
        // ------------



        // ------------
        // fusion_ilu5_elu2_red_out SWP8
        pack_ui8matrix(E, i1, j1, E_fusion_ilu5_elu2_red_P);


        clock_t t3, t4;
        t3 = clock();

        ouverture3_swp_ui8matrix_fusion_ilu5_elu2_red_bench(E, i0, i1, j0, j1, E_fusion_ilu5_elu2_red_P, E_fusion_result_ouverture_ilu5_elu2_red_P, E_fusion_ilu5_elu2_red);
        fermeture3_swp_ui8matrix_fusion_ilu5_elu2_red_bench(E, i0, i1, j0, j1, E_fusion_result_ouverture_ilu5_elu2_red_P, E_fusion_result_fermeture_ilu5_elu2_red_P, E_fusion_ilu5_elu2_red);

        t4 = clock();
        temps = (float)(t4-t3)/CLOCKS_PER_SEC;

        double debit_fusion =  temps;

        unpack_ui8matrix(E_fusion_result_fermeture_ilu5_elu2_red_P, i1, w8, E_fusion_ilu5_elu2_red);

        printf("%f  ",             debit_fusion);
        fprintf(output_file, "%lf   ",  debit_fusion);
        // // ------------

 
        // ------------
        // pipeline_ilu3_red_out SWP8
        pack_ui8matrix(E, i1, j1, E_pipeline_ilu3_red_out_P);

        t1 = clock();
        ouverture3_swp_ui8matrix_pipeline_ilu3_red_bench(E, i0, i1, j0, j1, E_pipeline_ilu3_red_out_P, E_T_pipeline_result_ouverture_ilu3_red_out_P, E_pipeline_result_ouverture_ilu3_red_out_P, E_pipeline_ilu3_red_out);
        fermeture3_swp_ui8matrix_pipeline_ilu3_red_bench(E, i0, i1, j0, j1, E_pipeline_result_ouverture_ilu3_red_out_P, E_T_pipeline_result_fermeture_ilu3_red_out_P, E_pipeline_result_fermeture_ilu3_red_out_P, E_pipeline_ilu3_red_out);

        t2 = clock();
        temps = (float)(t2-t1)/CLOCKS_PER_SEC;

        double debit_pipeline =  temps;

        unpack_ui8matrix(E_pipeline_result_fermeture_ilu3_red_out_P, i1, w8, E_pipeline_ilu3_red_out);

        printf("%f  ",            debit_pipeline);
        fprintf(output_file, "%lf   ",  debit_pipeline);
        // // ------------


        putchar('\n');
        fprintf(output_file, "\n");
    }

    // ----------
    // -- free --
    // ----------

    free_ui8matrix(I  , i0,   i1,   j0,   j1);
    free_ui8matrix(M  , i0,   i1,   j0,   j1);
    free_ui8matrix(O  , i0,   i1,   j0,   j1);
    free_ui8matrix(V  , i0,   i1,   j0,   j1);
    free_ui8matrix(E  , i0-1, i1+1, j0-1, j1+1);
    free_ui8matrix(E_8, i0,   i1,   j0,   j1);

    // version swp
    free_ui8matrix(E_fusion_ilu5_elu2_red_P                         ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_fusion_result_ouverture_ilu5_elu2_red_P        ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_fusion_result_fermeture_ilu5_elu2_red_P        ,i0, i1, j0, w8);
    free_ui8matrix(E_fusion_ilu5_elu2_red                           ,i0, i1, j0, j1);

    free_ui8matrix(E_T_pipeline_result_ouverture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_T_pipeline_result_fermeture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_pipeline_ilu3_red_out_P                         ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_pipeline_result_ouverture_ilu3_red_out_P        ,i0-2, i1+2, j0-2, w8+2);
    free_ui8matrix(E_pipeline_result_fermeture_ilu3_red_out_P        ,i0, i1, j0, w8);
    free_ui8matrix(E_pipeline_ilu3_red_out                           ,i0, i1, j0, j1);

    free_ui8matrix(E_ilu3_elu2_red_factor,                  i0-1, i1+1, j0-1, w1+1);
    free_ui8matrix(E_ilu3_elu2_red_factor_P   ,             i0-1, i1+1, j0-1, w1+1);
    free_ui8matrix(Erosion1_ilu3_elu2_red_factor_P   ,      i0-1, i1+1, j0-1, w1+1);
    free_ui8matrix(Erosion2_ilu3_elu2_red_factor_P,         i0-1, i1+1, j0-1, w1+1);
    free_ui8matrix(Dilatation1_ilu3_elu2_red_factor_P,      i0-1, i1+1, j0-1, w1+1);
    free_ui8matrix(Dilatation2_ilu3_elu2_red_factor_P,      i0-1, i1+1, j0-1, w1+1);

    fclose(output_file);
}
// ===============================

void motion_detection_morpho(void)
// ===============================
{
    motion_detection_morpho_v1();               // version basique sans optimisation
    // bench_motion_detection_morpho();            // bench version avec optimisation
    // bench_motion_detection_morpho_SWP8();       // bench version avec optimisation SWP 8
    // bench_motion_detection_morpho_SWP16();      // bench version avec optimisation SWP 16
    // bench_motion_detection_morpho_SWP32();      // bench version avec optimisation SWP 32

    // bench_debit_motion_detection_morpho_SWP8();

    // test_PGM();
}
