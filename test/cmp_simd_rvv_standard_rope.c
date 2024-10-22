#include <stdio.h>
#include <math.h>
#include <rope.h>

#define D 4096
#define VL 8

int main() {
    // Define two vectors of 32-bit floating point numbers
    float New_Token[D];
    float Token[D/VL][VL];
    for(int i = 0; i < D/VL; i++) {
        for (int j = 0 ; j < VL; j++){
            Token[i][j] = i*VL + j;
        }
    }

    int m_pos = 457;
    NEW_RVV_ROPE_FP32(&Token[0][0], New_Token, VL, m_pos, D);
    //printf("FIN\n");
    return 0;  // 명시적으로 0 반환
}
