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
    int m_pos = 10;

    float New_Token[D];
    float New_Token_baseline[D];
    float theta = (2 * M_PI ) / D;

    SCALAR_ROPE_FP32(&Token[0][0], m_pos, New_Token_baseline, D);

    return 0;
}