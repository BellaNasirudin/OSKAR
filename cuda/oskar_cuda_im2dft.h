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

#ifndef OSKAR_CUDA_IM2DFT_H_
#define OSKAR_CUDA_IM2DFT_H_

/**
 * @file oskar_cuda_im2dft.h
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Computes an image using CUDA.
 *
 * @details
 * Computes a real image from a set of complex visibilities, using CUDA to
 * evaluate a 2D Direct Fourier Transform (DFT).
 *
 * The computed image is returned in the \p image array, which
 * must be pre-sized to length (nl * nm). The fastest-varying dimension is
 * along l.
 *
 * The image is assumed to be completely real: conjugated copies of the
 * visibilities should therefore NOT be supplied.
 *
 * @param[in] nv No. of independent visibilities (excluding Hermitian copy).
 * @param[in] u Array of visibility u coordinates in wavelengths (length nv).
 * @param[in] v Array of visibility v coordinates in wavelengths (length nv).
 * @param[in] vis Array of complex visibilities (length 2 * nv; see note, above).
 * @param[in] nl The image dimension.
 * @param[in] nm The image dimension.
 * @param[in] dl The pixel size (cellsize) in radians.
 * @param[in] dm The pixel size (cellsize) in radians.
 * @param[in] sl The field of view in radians (full sky = pi).
 * @param[in] sm The field of view in radians (full sky = pi).
 * @param[out] image The computed image (see note, above).
 */
void oskar_cuda_im2dft(int nv, const float* u, const float* v,
        const float* vis, int nl, int nm, float dl, float dm,
        float sl, float sm, float* image);

#ifdef __cplusplus
}
#endif

#endif // OSKAR_CUDA_IM2DFT_H_
