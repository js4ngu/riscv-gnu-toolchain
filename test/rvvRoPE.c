#include <stdio.h>
#include <math.h>
#include <rope.h>

#define D 8
#define VL 4

void main() {
    // Define two vectors of 32-bit floating point numbers
    float New_Token[D];
    float Token[D/VL][VL];
    for(int i = 0; i < D/VL; i++) {
        for (int j = 0 ; j < VL; j++){
            Token[i][j] = i*VL + j;
        }
    }

    int m_pos = 5;
    RVV_ROPE_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    printf("\n========================================\n");
    for (int i = 0; i < D; i++) {
        printf("VFROPE.vf[%d] :%f\n", i, New_Token[i]);
    }
}