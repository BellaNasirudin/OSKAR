/*
 * Copyright (c) 2011, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "math/cudak/oskar_cudak_jones_mul_mat1_c1.h"
#include "math/cudak/oskar_cudaf_mul_c_mat2c.h"

// Single precision.
__global__
void oskar_cudak_jones_mul_mat1_c1_f(int n, const float4c* j1,
        const float2* s1, float4c* m)
{
    // Get the array index ID that this thread is working on.
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i >= n) return;

    // Get the data from global memory.
    float4c c_j1 = j1[i];
    float2 c_s1 = s1[i];

    // Multiply Jones matrix by complex scalar.
    oskar_cudaf_mul_c_mat2c_f(c_s1, c_j1);

    // Copy result back to global memory.
    m[i] = c_j1;
}

// Double precision.
__global__
void oskar_cudak_jones_mul_mat1_c1_d(int n, const double4c* j1,
        const double2* s1, double4c* m)
{
    // Get the array index ID that this thread is working on.
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i >= n) return;

    // Get the data from global memory.
    double4c c_j1 = j1[i];
    double2 c_s1 = s1[i];

    // Multiply Jones matrix by complex scalar.
    oskar_cudaf_mul_c_mat2c_d(c_s1, c_j1);

    // Copy result back to global memory.
    m[i] = c_j1;
}
