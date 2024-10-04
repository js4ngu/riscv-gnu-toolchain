#ifndef ROPE_H
#define ROPE_H
#include <stdint.h>  // uint32_t 정의를 위해 추가

void SIMD_VFROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D);
void ASM_VFROPE_FP32(float *token, float *new_token, int VL, float theta, int m_pos, int base_index);
void SCALAR_ROPE_FP32(float *token, int m, float *new_token, int n_dim);

void ASM_SET_CONSTANT(float theta, int m_pos);
void ASM_VFROPE_FVX_FP32(float *token, float *new_token, int VL, int base_index);
void SIMD_VFROPE_FVX_FP32(float *Token, float *New_Token, int VL, int m_pos, int D);
void ASM_SET_CONSTANT(float theta, int m_pos);
void RVV_ROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D);
void ASM_RVV_ROPE_FP32(float *cosX, float *sinX, int VL, uint32_t *evenMask, uint32_t *oddMask, float *New_Token);

#endif
