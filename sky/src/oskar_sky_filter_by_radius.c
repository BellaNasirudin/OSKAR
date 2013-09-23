/*
 * Copyright (c) 2012-2013, The University of Oxford
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

#include <oskar_angular_distance.h>
#include <oskar_sky.h>
#include <oskar_mem.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_sky_filter_by_radius(oskar_Sky* sky, double inner_radius,
        double outer_radius, double ra0, double dec0, int* status)
{
    int type, location, num_sources;

    /* Check all inputs. */
    if (!sky || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Return immediately if no filtering should be done. */
    if (inner_radius == 0.0 && outer_radius >= 180.0)
        return;

    if (outer_radius < inner_radius)
    {
        *status = OSKAR_ERR_INVALID_ARGUMENT;
        return;
    }

    /* Get the type and location. */
    type = oskar_sky_type(sky);
    location = oskar_sky_location(sky);
    num_sources = oskar_sky_num_sources(sky);

    if (location == OSKAR_LOCATION_CPU)
    {
        int in = 0, out = 0;
        if (type == OSKAR_SINGLE)
        {
            float *ra_, *dec_, *I_, *Q_, *U_, *V_, *ref_, *spix_;
            float *l_, *m_, *n_, *maj_, *min_, *pa_, *a_, *b_, *c_, dist;
            ra_   = oskar_mem_float(oskar_sky_ra(sky), status);
            dec_  = oskar_mem_float(oskar_sky_dec(sky), status);
            I_    = oskar_mem_float(oskar_sky_I(sky), status);
            Q_    = oskar_mem_float(oskar_sky_Q(sky), status);
            U_    = oskar_mem_float(oskar_sky_U(sky), status);
            V_    = oskar_mem_float(oskar_sky_V(sky), status);
            ref_  = oskar_mem_float(oskar_sky_reference_freq(sky), status);
            spix_ = oskar_mem_float(oskar_sky_spectral_index(sky), status);
            l_    = oskar_mem_float(oskar_sky_l(sky), status);
            m_    = oskar_mem_float(oskar_sky_m(sky), status);
            n_    = oskar_mem_float(oskar_sky_n(sky), status);
            maj_  = oskar_mem_float(oskar_sky_fwhm_major(sky), status);
            min_  = oskar_mem_float(oskar_sky_fwhm_minor(sky), status);
            pa_   = oskar_mem_float(oskar_sky_position_angle(sky), status);
            a_    = oskar_mem_float(oskar_sky_gaussian_a(sky), status);
            b_    = oskar_mem_float(oskar_sky_gaussian_b(sky), status);
            c_    = oskar_mem_float(oskar_sky_gaussian_c(sky), status);

            for (in = 0, out = 0; in < num_sources; ++in)
            {
                dist = (float)oskar_angular_distance(ra_[in], ra0,
                        dec_[in], dec0);
                if (dist < inner_radius || dist > outer_radius) continue;
                ra_[out]   = ra_[in];
                dec_[out]  = dec_[in];
                I_[out]    = I_[in];
                Q_[out]    = Q_[in];
                U_[out]    = U_[in];
                V_[out]    = V_[in];
                ref_[out]  = ref_[in];
                spix_[out] = spix_[in];
                l_[out]    = l_[in];
                m_[out]    = m_[in];
                n_[out]    = n_[in];
                maj_[out]  = maj_[in];
                min_[out]  = min_[in];
                pa_[out]   = pa_[in];
                a_[out]    = a_[in];
                b_[out]    = b_[in];
                c_[out]    = c_[in];
                out++;
            }
        }
        else
        {
            double *ra_, *dec_, *I_, *Q_, *U_, *V_, *ref_, *spix_;
            double *l_, *m_, *n_, *maj_, *min_, *pa_, *a_, *b_, *c_, dist;
            ra_   = oskar_mem_double(oskar_sky_ra(sky), status);
            dec_  = oskar_mem_double(oskar_sky_dec(sky), status);
            I_    = oskar_mem_double(oskar_sky_I(sky), status);
            Q_    = oskar_mem_double(oskar_sky_Q(sky), status);
            U_    = oskar_mem_double(oskar_sky_U(sky), status);
            V_    = oskar_mem_double(oskar_sky_V(sky), status);
            ref_  = oskar_mem_double(oskar_sky_reference_freq(sky), status);
            spix_ = oskar_mem_double(oskar_sky_spectral_index(sky), status);
            l_    = oskar_mem_double(oskar_sky_l(sky), status);
            m_    = oskar_mem_double(oskar_sky_m(sky), status);
            n_    = oskar_mem_double(oskar_sky_n(sky), status);
            maj_  = oskar_mem_double(oskar_sky_fwhm_major(sky), status);
            min_  = oskar_mem_double(oskar_sky_fwhm_minor(sky), status);
            pa_   = oskar_mem_double(oskar_sky_position_angle(sky), status);
            a_    = oskar_mem_double(oskar_sky_gaussian_a(sky), status);
            b_    = oskar_mem_double(oskar_sky_gaussian_b(sky), status);
            c_    = oskar_mem_double(oskar_sky_gaussian_c(sky), status);

            for (in = 0, out = 0; in < num_sources; ++in)
            {
                dist = oskar_angular_distance(ra_[in], ra0, dec_[in], dec0);
                if (dist < inner_radius || dist > outer_radius) continue;
                ra_[out]   = ra_[in];
                dec_[out]  = dec_[in];
                I_[out]    = I_[in];
                Q_[out]    = Q_[in];
                U_[out]    = U_[in];
                V_[out]    = V_[in];
                ref_[out]  = ref_[in];
                spix_[out] = spix_[in];
                l_[out]    = l_[in];
                m_[out]    = m_[in];
                n_[out]    = n_[in];
                maj_[out]  = maj_[in];
                min_[out]  = min_[in];
                pa_[out]   = pa_[in];
                a_[out]    = a_[in];
                b_[out]    = b_[in];
                c_[out]    = c_[in];
                out++;
            }
        }

        /* Set the new size of the sky model. */
        oskar_sky_resize(sky, out, status);
    }
    else
        *status = OSKAR_ERR_BAD_LOCATION;
}

#ifdef __cplusplus
}
#endif