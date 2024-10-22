#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <rope.h>

#define D 4096
#define VL 16

int main() {
    float Token[D/VL][VL];
    for(int i = 0; i < D/VL; i++) {
        for (int j = 0 ; j < VL; j++){
            Token[i][j] = i*VL + j;
        }
    }
    int m_pos = 457;

    float New_Token[D];
    SIMD_VFROPE_FVX_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    return 0;
}