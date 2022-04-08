/* -------------------- */
/* --- morpho_max.h --- */
/* -------------------- */

/*
 * Copyright (c) 2004 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#ifndef __MORPHO_MAX_H__
#define __MORPHO_MAX_H__

#ifdef __cplusplus
#ifdef PRAGMA_VERBOSE
#pragma message ("C++")
#endif
extern "C" {
#endif

// line functions
// Classique
    void line_max3_ui8matrix_basic               (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_reg                 (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_rot                 (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_red                 (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_ilu3                (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_ilu3_red            (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_elu2_red            (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_elu2_red_factor     (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_ilu3_elu2_red       (uint8 **X, int i, int j0, int j1, uint8 **Y);
    void line_max3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i, int j0, int j1, uint8 **Y);
//

//swp8
    void line_swp_max3_ui8matrix_basic               (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_rot                 (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_red                 (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_ilu3                (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_ilu3_red            (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_elu2_red            (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_elu2_red_factor     (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_ilu3_elu2_red       (uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_ilu3_elu2_red(uint8 **T, int i, int j0, int j1, uint8 **Y);
    void line_swp_max3_ui8matrix_ilu3_elu2_red_factor(uint8 **T, int i, int j0, int j1, uint8 **Y);
//

//swp16
    void line_swp_max3_ui16matrix_basic               (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_rot                 (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_red                 (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_ilu3                (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_ilu3_red            (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_elu2_red            (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_elu2_red_factor     (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_ilu3_elu2_red       (uint16 **T, int i, int j0, int j1, uint16 **Y);
    void line_swp_max3_ui16matrix_ilu3_elu2_red_factor(uint16 **T, int i, int j0, int j1, uint16 **Y);
//

//swp32
    void line_swp_max3_ui32matrix_basic              (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_rot                 (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_red                 (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_ilu3                (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_ilu3_red            (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_elu2_red            (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_elu2_red_factor     (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_ilu3_elu2_red       (uint32 **T, int i, int j0, int j1, uint32 **Y);
    void line_swp_max3_ui32matrix_ilu3_elu2_red_factor(uint32 **T, int i, int j0, int j1, uint32 **Y);
//

//full-matrix functions

// classique
    void max3_ui8matrix_basic               (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_reg                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_rot                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
    void max3_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
//

//swp8
    void max3_swp_ui8matrix_basic               (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_rot                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_ilu3_elu2_red(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
    void max3_swp_ui8matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint8 **T, uint8 **Y_P, uint8 **Y_UP);
//

//swp16
    void max3_swp_ui16matrix_basic               (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_rot                  (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_red                  (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_ilu3                 (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_ilu3_red             (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_elu2_red             (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_elu2_red_factor      (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_ilu3_elu2_red        (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
    void max3_swp_ui16matrix_ilu3_elu2_red_factor (uint8 **X, int i0, int i1, int j0, int j1, uint16 **T16, uint16 **Y_P16, uint8 **Y_UP16);
//

//swp32
    void max3_swp_ui32matrix_basic               (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_rot                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_red                 (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_ilu3                (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_ilu3_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_elu2_red            (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_elu2_red_factor     (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_ilu3_elu2_red       (uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
    void max3_swp_ui32matrix_ilu3_elu2_red_factor(uint8 **X, int i0, int i1, int j0, int j1, uint32 **T32, uint32 **Y_P32, uint8 **Y_UP32);
//
#ifdef __cplusplus
}
#endif

#endif // __MORPHO_MAX_H__
