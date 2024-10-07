#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>  // For rand() and srand()
#include <rope.h>

#define USE_SIMD_VFROPE_FP32 0 
#define USE_SIMD_VFROPE_FVX_FP32 1
#define USE_RVV_ROPE_FP32 0

#define RANDOM_MAX 10
#define SEED 32

#define MODE 0
#define INPUT_LOG 0
#define ROPE_LOG 0
#define CMP_LOG 1

#define D 4096
#define VL 8

void debug_rope(float Token[][VL], float *New_Token_baseline, float *New_Token);

int main() {
    // For e32
    srand(SEED);
    int m_pos = (int)(rand() % RANDOM_MAX);
    float Token[D/VL][VL];
    for(int i = 0; i < D/VL; i++) 
        for (int j = 0 ; j < VL; j++)
            Token[i][j] = i*VL + j;
    float New_Token[D];
    float New_Token_baseline[D];

    SCALAR_ROPE_FP32(&Token[0][0], m_pos, New_Token_baseline, D);

    #if USE_SIMD_VFROPE_FP32 == 1
        SIMD_VFROPE_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    #elif USE_SIMD_VFROPE_FVX_FP32 == 1
        SIMD_VFROPE_FVX_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    #elif USE_RVV_ROPE_FP32 == 1
        RVV_ROPE_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    #else
        SIMD_VFROPE_FVX_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    #endif

    debug_rope(Token, New_Token_baseline, New_Token);

    return 0;
}

void debug_rope(float Token[][VL], float *New_Token_baseline, float *New_Token) {
    #if INPUT_LOG == 1
    printf("\nInput Token \n");
    for (int i = 0; i < D/VL; i++) {
        for (int j = 0; j < VL; j++) {
            printf("[%d][%d] : %f, \t", i, j, Token[i][j]);
        }
        printf("\n");
    }
    #endif

    #if ROPE_LOG == 1
    printf("\nBASELINE, VFROPE.VF \n");
    for (int i = 0; i < D; i++) {
        printf("BASELINE[%d]:VFROPE.vf[%d] : \t %f ,\t%f\n", i, i, New_Token_baseline[i], New_Token[i]);
    }
    #endif

    #if CMP_LOG == 1
    printf("\nDIFF w/ BASE LINE \n");
    for (int i = 0; i < D/VL; i++) {
        printf("[%d] : ", i);
        for (int j = 0 ; j < VL; j++) {
            printf("%f, ", New_Token_baseline[i*VL+j] - New_Token[i*VL+j]);
        }
        printf("\n");
    }
    #endif
}
