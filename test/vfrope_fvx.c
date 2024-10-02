#include <stdio.h>
#include <rope.h>

#define VECTOR_SIZE 4

void main() {
    // Define two vectors of 32-bit floating point numbers
    float Token[VECTOR_SIZE] = {1.0, 2.0, 3.0, 4.0};
    float newToken[VECTOR_SIZE] = {0}; // Initialize the newToken array
    float theta = 0.25;
    int baseIndex = 0;
    int m_pos = 10;

    SIMD_VFROPE_FVX_FP32(&Token[0], newToken, VECTOR_SIZE, m_pos, VECTOR_SIZE);

    for (int i = 0; i < VECTOR_SIZE; i++) {
        printf("result[%d] = %f\n", i, newToken[i]);
    }
}
