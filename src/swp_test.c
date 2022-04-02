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
// void rand_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 x0, uint8 x1)
// // -------------------------------------------------------------------------------
// {
//     int d = x1 - x0 + 1; // range [x0, x1]
    
//     for(int i=i0; i<=i1; i++) {
//         for(int j=j0; j<=j1; j++) {
        
//             int x = x0 + rand() % d;
            
//             X[i][j] = (uint8) x;
//         }
//     }
// }
// // -------------------------------------------------------------------------
// void rand1_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, int density)
// // -------------------------------------------------------------------------
// {
//     // binary generator
    
//     for(int i=i0; i<=i1; i++) {
//         for(int j=j0; j<=j1; j++) {
        
//             int r = rand() % 100;
//             if(r <= density)
//                 X[i][j] = 1;
//             else
//                 X[i][j] = 0;
//         }
//     }
// }
// // ------------------------------------------------------------------------------------
// int compare_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y, char* name)
// // ------------------------------------------------------------------------------------
// {
//     int c = 0;
    
//     //display_ui8matrix(Y, i0, i1, j0, j1, "%5d", name);
    
//     for(int i=i0; i<=i1; i++) {
//         for(int j=j0; j<=j1; j++) {
//             if(X[i][j] != Y[i][j]) {
//                 printf("%s [%3d][%3d] = %3d NEQ %3d\n", name, i, j, X[i][i], Y[i][i]);
//                 c++;
//             }
//         }
//     }
//     if(c) {
//         printf("%d errors !\n", c); exit(-1);
//     } else {
//         printf("[compare]: %s is OK\n", name);
//         return c;
//     }
//     puts("----------------------");
// }



int test_swp(int argc, char* argv[]){
    puts("=== test_swap ===");
    // test_swp_morpho_max();
    return 0;
}