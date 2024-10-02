#ifndef ROPE_H
#define ROPE_H

void SIMD_VFROPE_FP32(float *Token, float *New_Token, int VL, int m_pos, int D);
void ASM_VFROPE_FP32(float *token, float *new_token, int VL, float theta, int m_pos, int base_index);
void SCALAR_ROPE_FP32(float *token, int m, float *new_token, int n_dim);

void ASM_SET_CONSTANT(float theta, int m_pos);
void ASM_VFROPE_FVX_FP32(float *token, float *new_token, int VL, int base_index);
void SIMD_VFROPE_FVX_FP32(float *Token, float *New_Token, int VL, int m_pos, int D);
void ASM_SET_CONSTANT(float theta, int m_pos);

#endif
