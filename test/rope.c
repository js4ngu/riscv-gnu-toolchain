#include <rope.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>  // uint32_t 정의를 위해 추가

#define DEBUG 0

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

void ASM_SET_CONSTANT(float theta, int m_pos) {
    asm volatile (
        "fmv.s f30, %0\n\t"  // Set theta to F30
        "mv x31, %1\n\t"     // Set M using the variable
        :
        : "f" (theta), "r" (m_pos)
        : "f30", "x31"
    );
}

void ASM_VFROPE_FVX_FP32(float *token, float *new_token, int VL, int base_index) {
    asm volatile (
        "vsetvli t0, %2, e32, m1\n"
        "vle32.v v0, (%0)\n"
        "vfrope.fvx v2, v0, %3\n"
        "vse32.v v2, (%1)\n"
        : 
        : "r" (token), "r" (new_token), "r" (VL), "r" (base_index)
        : "t0", "v0", "v1", "v2"
    );
}

void SIMD_VFROPE_FVX_FP32(float *Token, float *New_Token, int VL, int m_pos, int D) {
    float theta = 2.0f / D;
    int base_index = 0;

    ASM_SET_CONSTANT(theta, m_pos);
    
    for (int i = 0; i < D / VL; i++) {
        ASM_VFROPE_FVX_FP32(&Token[i * VL], &New_Token[i * VL], VL, base_index);
        base_index = base_index + (VL / 2);
    }
}

void ASM_RVV_ROPE_FP32(float *cosX, float *sinX, int VL, uint32_t *evenMask, uint32_t *oddMask, float *Token, float *New_Token) {
    asm volatile (
        "vsetvli t0, %0, e32, m1\n\t"         // Set vector length and type

        // Load cosX and sinX
        "vle32.v v1, (%1)\n\t"                // Load  cosX into v1
        "vle32.v v2, (%2)\n\t"                // Load  sinX into v2
        "vle32.v v10,(%6)\n\t"                // Load Token into v10

        // 기존의 cosX, sinX2를 vfmul을 이용하여 계산
        //"vfmul.vv v1, v1, v10\n\t"            // v1 = cosX * Token
        //"vfmul.vv v2, v2, v10\n\t"            // v2 = sinX * Token

        // Even index processing: v5 = cosX - (slide down sinX)
        "vle32.v v3, (%4)\n\t"                // Load evenMask(1010) into v3
        "vslidedown.vi v8, v2, 1\n\t"         // Slide down sinX by 1 position
        "vmand.mm v0, v3, v3\n\t"             // Mask even positions
        "vfsub.vv v5, v1, v8, v0.t\n\t"       // Perform: v5 = cosX - (slidedown sinX)

        // Odd index processing: v6 = cosX + (slide up sinX)
        "vle32.v v4, (%3)\n\t"                // Load oddMask(0101) into v4
        "vslideup.vi v9, v2, 1\n\t"           // Slide up sinX by 1 position
        "vmand.mm v0, v4, v4\n\t"             // Mask odd positions
        "vfadd.vv v6, v1, v9, v0.t\n\t"       // Perform: v6 = cosX + (slideup sinX)

        // Combine both results: v7 = v5 OR v6
        "vor.vv v7, v5, v6\n\t"               // Combine: v7 = v5 | v6

        // Store the final result in New_Token
        "vse32.v v7, (%5)\n\t"                // Store the final result (v7)
        "fence\n\t"
        :
        : "r" (VL), "r" (cosX), "r" (sinX), "r" (evenMask), "r" (oddMask), "r" (New_Token), "r" (Token)
        : "t0", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10"
    );
}

void RVV_ROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D) {
    // Temporary arrays to store intermediate results
    volatile float cosX[VL] __attribute__((aligned(16)));
    volatile float sinX[VL] __attribute__((aligned(16)));

    // Mask constants
    uint32_t evenMask = 0xAAAAAAAA;
    uint32_t oddMask  = 0x55555555;
    
    float theta = 2.0f / D;
    float factor = 0;
    float m_theta_i;
    int base_index = 0;

    for (int block = 0; block < D / VL; block ++) {
        for (int i = 0; i < VL; i += 2) {
            base_index    = (i / 2) + 1;
            factor    = m_pos * theta * M_PI;
            m_theta_i = factor * (base_index + (VL/2) * block);
            //여기선 삼각함수 값만 정의
            cosX[i]     = Token[(block * VL) + i]     * cosf(m_theta_i); 
            cosX[i + 1] = Token[(block * VL) + i + 1] * cosf(m_theta_i);
            sinX[i]     = Token[(block * VL) + i]     * sinf(m_theta_i);
            sinX[i + 1] = Token[(block * VL) + i + 1] * sinf(m_theta_i);
        }
        
        ASM_RVV_ROPE_FP32((float *)cosX, (float *)sinX, VL, &evenMask, &oddMask, Token + (block * VL), New_Token + (block * VL));
    }
}

void NEW_ASM_RVV_ROPE_FP32(float *cosX, float *sinX, int VL, uint32_t *evenMask, uint32_t *oddMask, float *Token, float *New_Token) {    
    asm volatile (        
        "vsetvli t0, %0, e32, m1\n\t"          // Set vector length and type
        
        // Load cosX, sinX, and Token
        "vle32.v v1, (%1)\n\t"                 // Load cosX into v1
        "vle32.v v2, (%2)\n\t"                 // Load sinX into v2
        "vle32.v v10,(%6)\n\t"                 // Load Token into v10

        // Multiply cosX and sinX by Token
        "vfmul.vv v1, v1, v10\n\t"             // v1 = cosX * Token
        "vfmul.vv v2, v2, v10\n\t"             // v2 = sinX * Token

        // Even index processing: v5 = cosX - (slide down sinX)
        "vle32.v v3, (%4)\n\t"                 // Load evenMask (1010) into v3
        "vslidedown.vi v8, v2, 1\n\t"          // Slide down sinX by 1 position
        "vmand.mm v0, v3, v3\n\t"              // Mask even positions
        "vfsub.vv v5, v1, v8, v0.t\n\t"        // Perform: v5 = cosX - (slidedown sinX)

        // Odd index processing: v6 = cosX + (slide up sinX)
        "vle32.v v4, (%3)\n\t"                 // Load oddMask (0101) into v4
        "vslideup.vi v9, v2, 1\n\t"            // Slide up sinX by 1 position
        "vmand.mm v0, v4, v4\n\t"              // Mask odd positions
        "vfadd.vv v6, v1, v9, v0.t\n\t"        // Perform: v6 = cosX + (slideup sinX)

        // Combine both results: v7 = v5 OR v6
        "vor.vv v7, v5, v6\n\t"                // Combine: v7 = v5 | v6

        // Store the final result in New_Token
        "vse32.v v7, (%5)\n\t"                 // Store the final result (v7)
        "fence\n\t"        
        :        
        : "r" (VL), "r" (cosX), "r" (sinX), "r" (evenMask), "r" (oddMask), "r" (New_Token), "r" (Token)        
        : "t0", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10"
    );
}

void NEW_RVV_ROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D) {    
    // Temporary arrays to store intermediate results
    volatile float cosX[VL] __attribute__((aligned(16)));
    volatile float sinX[VL] __attribute__((aligned(16)));

    // Mask constants
    uint32_t evenMask = 0xAAAAAAAA;
    uint32_t oddMask  = 0x55555555;

    float theta = 2.0f / D;
    float factor = 0;
    float m_theta_i;
    int base_index = 0;

    // Loop over the number of blocks
    for (int block = 0; block < D / VL; block++) {
        // Calculate cosX and sinX for each element in the block
        for (int i = 0; i < VL; i += 2) {
            base_index = (i / 2) + 1;
            factor = m_pos * theta * M_PI;
            m_theta_i = factor * (base_index + (VL / 2) * block);

            // Only calculate the trigonometric values here
            cosX[i]     = cosf(m_theta_i);             
            cosX[i + 1] = cosf(m_theta_i);            
            sinX[i]     = sinf(m_theta_i);            
            sinX[i + 1] = sinf(m_theta_i);
            // Check for NaN
            if (isnan(cosX[i]) || isnan(sinX[i]));
        }

        // Move multiplication and vector operations to ASM_RVV_ROPE_FP32
        NEW_ASM_RVV_ROPE_FP32((float *)cosX, (float *)sinX, VL, &evenMask, &oddMask, Token + (block * VL), New_Token + (block * VL));
    }
}
