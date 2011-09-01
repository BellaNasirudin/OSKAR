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

#ifndef OSKAR_HA_DEC_TO_HOR_LMN_H_
#define OSKAR_HA_DEC_TO_HOR_LMN_H_

/**
 * @file oskar_ha_dec_to_hor_lmn.h
 */

#include "oskar_windows.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Equatorial to horizontal 3D direction cosines (single precision).
 *
 * @details
 * This function computes the l,m,n direction cosines of the specified points
 * in the horizontal coordinate system.
 *
 * Points where hor_n is negative are below the local horizon.
 *
 * @param[in] n      The number of sources in the sky model.
 * @param[in] ha     The input source Hour Angles in radians.
 * @param[in] dec    The input source Declinations in radians.
 * @param[in] lat    The geographic latitude of the observer.
 * @param[out] hor_l The source l-direction-cosines in the horizontal system.
 * @param[out] hor_m The source m-direction-cosines in the horizontal system.
 * @param[out] hor_n The source n-direction-cosines in the horizontal system.
 */
DllExport
int oskar_ha_dec_to_hor_lmn_f(int n, const float* ha, const float* dec,
		float lat, float* hor_l, float* hor_m, float* hor_n);

/**
 * @brief
 * Equatorial to horizontal 3D direction cosines (double precision).
 *
 * @details
 * This function computes the l,m,n direction cosines of the specified points
 * in the horizontal coordinate system.
 *
 * Points where hor_n is negative are below the local horizon.
 *
 * @param[in] n      The number of sources in the sky model.
 * @param[in] ha     The input source Hour Angles in radians.
 * @param[in] dec    The input source Declinations in radians.
 * @param[in] lat    The geographic latitude of the observer.
 * @param[out] hor_l The source l-direction-cosines in the horizontal system.
 * @param[out] hor_m The source m-direction-cosines in the horizontal system.
 * @param[out] hor_n The source n-direction-cosines in the horizontal system.
 */
DllExport
int oskar_ha_dec_to_hor_lmn_d(int n, const double* ha, const double* dec,
		double lat, double* hor_l, double* hor_m, double* hor_n);

#ifdef __cplusplus
}
#endif

#endif // OSKAR_HA_DEC_TO_HOR_LMN_H_