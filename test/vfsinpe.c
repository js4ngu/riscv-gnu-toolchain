#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define VECTOR_SIZE 4

void sinPE(int pos, int d_model, double* Token);
void computeSINPE();

void vfsinpe_vv_f32() {
    // Define two vectors of 32-bit floating point numbers
    float pos = 0;
    float d_model = 4;
    float vector1[VECTOR_SIZE] = {pos, d_model, 0, 0};
    float vector2[VECTOR_SIZE] = {5.0, 6.0, 7.0, 8.0};
    float result[VECTOR_SIZE];

    asm volatile (
        "vsetvli t0, %2, e32, m1\n"   // Set vector length to VECTOR_SIZE, element width to 32 bits
        "vle32.v v0, (%0)\n"          // Load vector1 into v0
        "vle32.v v1, (%1)\n"          // Load vector2 into v1
        "vfsinpe.vv v2, v0, v1\n"     
        "vse32.v v2, (%3)\n"          // Store the result from v2 to the result array
        :
        : "r" (vector1), "r" (vector2), "r" (VECTOR_SIZE), "r" (result)
        : "t0", "v0", "v1", "v2"
    );

    // Print the result
    for (int i = 0; i < VECTOR_SIZE; i++) {
        printf("result[%d] = %f\n", i, result[i]);
    }
}

// Sinusoidal Positional Encoding 벡터를 계산하고 Token 벡터와 더하는 함수
void sinPE(int pos, int d_model, double* Token) {
    double* PE_vector = (double*)malloc(d_model * sizeof(double));
    for (int i = 0; i < d_model; i += 2) {
        PE_vector[i] = sin(pos / pow(10000, ((2.0 * i) / d_model)));
        if (i + 1 < d_model) {
            PE_vector[i + 1] = cos(pos / pow(10000, ((2.0 * i) / d_model)));
        }
    }
    for (int i = 0; i < d_model; i++) {
        Token[i] += PE_vector[i];
    }
    free(PE_vector);
}

void computeSINPE() {
    int max_len = 50;  // 시퀀스의 최대 길이
    int d_model = 130;  // 모델의 차원
    int pos = 1;    // 몇번째 토큰인지

    double* Token = (double*)calloc(max_len * d_model, sizeof(double));
    // 초기 토큰 값 설정 (예: 모든 값을 0로 설정)
    for (int i = 0; i < max_len * d_model; i++) {
        Token[i] = 0.0;
    }

    sinPE(pos, d_model, &Token[pos * d_model]);

    // 결과 출력
    for (int i = 0; i < d_model; i++) {
        printf("%f ", Token[pos * d_model + i]);
    }
    printf("\n");

    free(Token);
}

int main(){
    vfsinpe_vv_f32();
    computeSINPE();
}