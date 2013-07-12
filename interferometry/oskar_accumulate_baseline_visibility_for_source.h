/*
 * Copyright (c) 2013, The University of Oxford
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

#ifndef OSKAR_ACCUMULATE_BASELINE_VISIBILITY_FOR_SOURCE_H_
#define OSKAR_ACCUMULATE_BASELINE_VISIBILITY_FOR_SOURCE_H_

/**
 * @file oskar_accumulate_baseline_visibility_for_source.h
 */

#include "oskar_global.h"
#include "math/oskar_multiply_inline.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Accumulates the visibility response on one baseline due to a single source
 * (single precision).
 *
 * @details
 * This function evaluates the visibility response for a single source on a
 * single baseline between stations p and q, accumulating the result with
 * the existing baseline visibility. It requires the final (collapsed)
 * Jones matrices for the source for stations p and q, and the unmodified
 * Stokes parameters of the source.
 *
 * The output visibility on the baseline is updated according to:
 *
 * V_pq = V_pq + J_p * B * J_q^H
 *
 * where the brightness matrix B is assembled as:
 *
 * B = [ I + Q    U + iV ]
 *     [ U - iV    I - Q ]
 *
 * Visibilities are updated for a single source, even though array pointers are
 * passed to this function. The source is specified using the \p source_id
 * index parameter.
 *
 * @param[in,out] V_pq  Running total of source visibilities on the baseline.
 * @param[in] source_id Index of source in all arrays.
 * @param[in] I         Array of source Stokes I values, in Jy.
 * @param[in] Q         Array of source Stokes Q values, in Jy.
 * @param[in] U         Array of source Stokes U values, in Jy.
 * @param[in] V         Array of source Stokes V values, in Jy.
 * @param[in] J_p       Array of source Jones matrices for station p.
 * @param[in] J_q       Array of source Jones matrices for station q.
 * @param[in] smear     Smearing factor by which to modify source visibility.
 */
OSKAR_INLINE
void oskar_accumulate_baseline_visibility_for_source_f(float4c* V_pq,
        const int source_id, const float* I, const float* Q, const float* U,
        const float* V, const float4c* J_p, const float4c* J_q,
        const float smear)
{
    /* Construct source brightness matrix (ignoring c, as it's Hermitian). */
    float4c m1, m2;
    {
        float s_I, s_Q;
        s_I = I[source_id];
        s_Q = Q[source_id];
        m2.b.x = U[source_id];
        m2.b.y = V[source_id];
        m2.a.x = s_I + s_Q;
        m2.d.x = s_I - s_Q;
    }

    /* Multiply first Jones matrix with source brightness matrix. */
    m1 = J_p[source_id];
    oskar_multiply_complex_matrix_hermitian_in_place_f(&m1, &m2);

    /* Multiply result with second (Hermitian transposed) Jones matrix. */
    m2 = J_q[source_id];
    oskar_multiply_complex_matrix_conjugate_transpose_in_place_f(&m1, &m2);

    /* Multiply result by smearing term and accumulate. */
    V_pq->a.x += m1.a.x * smear;
    V_pq->a.y += m1.a.y * smear;
    V_pq->b.x += m1.b.x * smear;
    V_pq->b.y += m1.b.y * smear;
    V_pq->c.x += m1.c.x * smear;
    V_pq->c.y += m1.c.y * smear;
    V_pq->d.x += m1.d.x * smear;
    V_pq->d.y += m1.d.y * smear;
}

/**
 * @brief
 * Accumulates the visibility response on one baseline due to a single source
 * (double precision).
 *
 * @details
 * This function evaluates the visibility response for a single source on a
 * single baseline between stations p and q, accumulating the result with
 * the existing baseline visibility. It requires the final (collapsed)
 * Jones matrices for the source for stations p and q, and the unmodified
 * Stokes parameters of the source.
 *
 * The output visibility on the baseline is updated according to:
 *
 * V_pq = V_pq + J_p * B * J_q^H
 *
 * where the brightness matrix B is assembled as:
 *
 * B = [ I + Q    U + iV ]
 *     [ U - iV    I - Q ]
 *
 * Visibilities are updated for a single source, even though array pointers are
 * passed to this function. The source is specified using the \p source_id
 * index parameter.
 *
 * @param[in,out] V_pq  Running total of source visibilities on the baseline.
 * @param[in] source_id Index of source in all arrays.
 * @param[in] I         Array of source Stokes I values, in Jy.
 * @param[in] Q         Array of source Stokes Q values, in Jy.
 * @param[in] U         Array of source Stokes U values, in Jy.
 * @param[in] V         Array of source Stokes V values, in Jy.
 * @param[in] J_p       Array of source Jones matrices for station p.
 * @param[in] J_q       Array of source Jones matrices for station q.
 * @param[in] smear     Smearing factor by which to modify source visibility.
 */
OSKAR_INLINE
void oskar_accumulate_baseline_visibility_for_source_d(double4c* V_pq,
        const int source_id, const double* I, const double* Q, const double* U,
        const double* V, const double4c* J_p, const double4c* J_q,
        const double smear)
{
    /* Construct source brightness matrix (ignoring c, as it's Hermitian). */
    double4c m1, m2;
    {
        double s_I, s_Q;
        s_I = I[source_id];
        s_Q = Q[source_id];
        m2.b.x = U[source_id];
        m2.b.y = V[source_id];
        m2.a.x = s_I + s_Q;
        m2.d.x = s_I - s_Q;
    }

    /* Multiply first Jones matrix with source brightness matrix. */
    m1 = J_p[source_id];
    oskar_multiply_complex_matrix_hermitian_in_place_d(&m1, &m2);

    /* Multiply result with second (Hermitian transposed) Jones matrix. */
    m2 = J_q[source_id];
    oskar_multiply_complex_matrix_conjugate_transpose_in_place_d(&m1, &m2);

    /* Multiply result by smearing term and accumulate. */
    V_pq->a.x += m1.a.x * smear;
    V_pq->a.y += m1.a.y * smear;
    V_pq->b.x += m1.b.x * smear;
    V_pq->b.y += m1.b.y * smear;
    V_pq->c.x += m1.c.x * smear;
    V_pq->c.y += m1.c.y * smear;
    V_pq->d.x += m1.d.x * smear;
    V_pq->d.y += m1.d.y * smear;
}

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_ACCUMULATE_BASELINE_VISIBILITY_FOR_SOURCE_H_ */