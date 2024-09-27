#include <rope.h>
#include <stdio.h>
#include <math.h>


void ASM_VFROPE_FP32(float *token, float *new_token, int VL, float theta, int m_pos, int base_index) {
    asm volatile (
        "mv x30, %5\n\t"                     // set Base Index
        "mv x31, %4\n\t"                     // Set M using the variable
        "vsetvli t0, %0, e32\n\t"            // Set vector length and type (e32)
        "vle32.v v1, (%1)\n\t"               // Load Token vector into v1
        "vfrope.vf v2, v1, %2\n\t"           // Perform RoPE with m_pos
        "vse32.v v2, (%3)\n\t"               // Store the result vector from v2 to New_Token
        :
        : "r" (VL), "r" (token), "f" (theta), "r" (new_token), "r" (m_pos), "r" (base_index)
        : "t0", "v1", "v2", "x31", "x30"
    );
}

void SIMD_VFROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D) {
    //float theta = (2 * M_PI ) / D; // 여기에 PI 제거
    float theta = 2.0f / D;  // PI 제거

    int base_index = 0;
    for (int i = 0; i < D / VL; i++) {
        ASM_VFROPE_FP32(&Token[i * VL], &New_Token[i * VL], VL, theta, m_pos, base_index);
        base_index = base_index + (VL / 2);
    }
}


void SCALAR_ROPE_FP32(float *token, int m, float *new_token, int n_dim) {
    for (int i = 1; i <= n_dim / 2; i++) {
        float theta_i = (2 * M_PI * i) / n_dim;
        float m_theta_i = m * theta_i;

        float cos_m_theta_i = cos(m_theta_i);
        float sin_m_theta_i = sin(m_theta_i);

        float x_2i_minus_1 = token[2 * i - 2];
        float x_2i = token[2 * i - 1];

        new_token[2 * i - 2] = x_2i_minus_1 * cos_m_theta_i - x_2i * sin_m_theta_i;
        new_token[2 * i - 1] = x_2i * cos_m_theta_i + x_2i_minus_1 * sin_m_theta_i;
    }
}
