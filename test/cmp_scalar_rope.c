#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <rope.h>

#define D 16
#define VL 4


int main() {
    float Token[D/VL][VL];
    for(int i = 0; i < D/VL; i++) {
        for (int j = 0 ; j < VL; j++){
            Token[i][j] = i*VL + j;
        }
    }
    int m_pos = 4;

    float New_Token[D];
    float New_Token_baseline[D];
    SCALAR_ROPE_FP32(&Token[0][0], m_pos, New_Token_baseline, D);
    for (int i = 0; i < D; i++) {
        printf("VFROPE.vf[%d] :%f\n", i, New_Token_baseline[i]);
    }
    return 0;
}