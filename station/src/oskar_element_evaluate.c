/*
 * Copyright (c) 2012-2014, The University of Oxford
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

#include <private_element.h>
#include <oskar_element.h>

#include <oskar_apply_element_taper_cosine.h>
#include <oskar_apply_element_taper_gaussian.h>
#include <oskar_evaluate_dipole_pattern.h>
#include <oskar_evaluate_geometric_dipole_pattern.h>
#include <oskar_convert_enu_direction_cosines_to_theta_phi.h>
#include <oskar_convert_ludwig3_to_theta_phi_components.h>
#include <oskar_find_closest_match.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288;
#endif

#define C_0 299792458.0

#ifdef __cplusplus
extern "C" {
#endif

void oskar_element_evaluate(const oskar_Element* model, oskar_Mem* output,
        double orientation_x, double orientation_y, int num_points,
        const oskar_Mem* x, const oskar_Mem* y, const oskar_Mem* z,
        double frequency_hz, oskar_Mem* theta, oskar_Mem* phi, int* status)
{
    int spline_x = 0, spline_y = 0, computed_angles = 0;
    double dipole_length_m;

    /* Check all inputs. */
    if (!model || !output || !x || !y || !z || !theta || !phi || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Check if spline data is present for x or y dipole. */
    spline_x = oskar_element_has_x_spline_data(model);
    spline_y = oskar_element_has_y_spline_data(model);

    /* Check that the output array is complex. */
    if (!oskar_mem_is_complex(output))
        *status = OSKAR_ERR_BAD_DATA_TYPE;

    /* Ensure there is enough space in the theta and phi work arrays. */
    if ((int)oskar_mem_length(theta) < num_points)
        oskar_mem_realloc(theta, num_points, status);
    if ((int)oskar_mem_length(phi) < num_points)
        oskar_mem_realloc(phi, num_points, status);

    /* Resize output array if required. */
    if ((int)oskar_mem_length(output) < num_points)
        oskar_mem_realloc(output, num_points, status);

    /* Check if safe to proceed. */
    if (*status) return;

    /* Check if element type is isotropic. */
    if (model->element_type == OSKAR_ELEMENT_TYPE_ISOTROPIC)
        oskar_mem_set_value_real(output, 1.0, 0, 0, status);

    /* Get the length of the dipole in metres. */
    dipole_length_m = model->dipole_length;
    if (model->dipole_length_units == OSKAR_ELEMENT_LENGTH_WAVELENGTHS)
    {
        dipole_length_m *= (C_0 / frequency_hz);
    }

    /* Evaluate polarised response if output array is matrix type. */
    if (oskar_mem_is_matrix(output))
    {
        double delta_phi_x, delta_phi_y;

        /* Check if spline data present for dipole X. */
        if (spline_x)
        {
            int freq_id;

            /* Compute modified theta and phi coordinates for dipole X. */
            delta_phi_x = M_PI/2.0 - orientation_x;
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_x, num_points, x, y, z, status);
            computed_angles = 1;

            /* Get the frequency index. */
            freq_id = oskar_find_closest_match_d(frequency_hz,
                    oskar_element_num_freq(model),
                    oskar_element_freqs_hz(model));

            /* Evaluate spline pattern for dipole X. */
            oskar_splines_evaluate(output, 0, 8, model->x_h_re[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 1, 8, model->x_h_im[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 2, 8, model->x_v_re[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 3, 8, model->x_v_im[freq_id],
                    num_points, theta, phi, status);

            /* Convert from Ludwig-3 to spherical representation. */
            oskar_convert_ludwig3_to_theta_phi_components(output,
                    0, 4, num_points, phi, status);
        }
        else if (model->element_type == OSKAR_ELEMENT_TYPE_DIPOLE)
        {
            /* Compute modified theta and phi coordinates for dipole X. */
            delta_phi_x = orientation_x - M_PI/2.0; /* TODO check the order. */
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_x, num_points, x, y, z, status);
            computed_angles = 1;

            /* Evaluate dipole pattern for dipole X. */
            oskar_evaluate_dipole_pattern(output, num_points, theta, phi,
                    frequency_hz, dipole_length_m, 1, status);
        }
        else if (model->element_type == OSKAR_ELEMENT_TYPE_GEOMETRIC_DIPOLE)
        {
            /* Compute modified theta and phi coordinates for dipole X. */
            delta_phi_x = orientation_x - M_PI/2.0; /* TODO check the order. */
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_x, num_points, x, y, z, status);
            computed_angles = 1;

            /* Evaluate dipole pattern for dipole X. */
            oskar_evaluate_geometric_dipole_pattern(output, num_points,
                    theta, phi, 1, status);
        }

        /* Check if spline data present for dipole Y. */
        if (spline_y)
        {
            int freq_id;

            /* Compute modified theta and phi coordinates for dipole Y. */
            delta_phi_y = M_PI/2.0 - orientation_y;
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_y, num_points, x, y, z, status);
            computed_angles = 1;

            /* Get the frequency index. */
            freq_id = oskar_find_closest_match_d(frequency_hz,
                    oskar_element_num_freq(model),
                    oskar_element_freqs_hz(model));

            /* Evaluate spline pattern for dipole Y. */
            oskar_splines_evaluate(output, 4, 8, model->y_h_re[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 5, 8, model->y_h_im[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 6, 8, model->y_v_re[freq_id],
                    num_points, theta, phi, status);
            oskar_splines_evaluate(output, 7, 8, model->y_v_im[freq_id],
                    num_points, theta, phi, status);

            /* Convert from Ludwig-3 to spherical representation. */
            oskar_convert_ludwig3_to_theta_phi_components(output,
                    2, 4, num_points, phi, status);
        }
        else if (model->element_type == OSKAR_ELEMENT_TYPE_DIPOLE)
        {
            /* Compute modified theta and phi coordinates for dipole Y. */
            delta_phi_y = orientation_y - M_PI/2.0; /* TODO check the order. */
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_y, num_points, x, y, z, status);
            computed_angles = 1;

            /* Evaluate dipole pattern for dipole Y. */
            oskar_evaluate_dipole_pattern(output, num_points, theta, phi,
                    frequency_hz, dipole_length_m, 0, status);
        }
        else if (model->element_type == OSKAR_ELEMENT_TYPE_GEOMETRIC_DIPOLE)
        {
            /* Compute modified theta and phi coordinates for dipole Y. */
            delta_phi_y = orientation_y - M_PI/2.0; /* TODO check the order. */
            oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                    delta_phi_y, num_points, x, y, z, status);
            computed_angles = 1;

            /* Evaluate dipole pattern for dipole Y. */
            oskar_evaluate_geometric_dipole_pattern(output, num_points,
                    theta, phi, 0, status);
        }
    }

    /* Compute theta values for tapering, if not already done. */
    if (model->taper_type != OSKAR_ELEMENT_TAPER_NONE && !computed_angles)
    {
        oskar_convert_enu_direction_cosines_to_theta_phi(theta, phi,
                0, num_points, x, y, z, status);
        computed_angles = 1;
    }

    /* Apply element tapering, if specified. */
    if (model->taper_type == OSKAR_ELEMENT_TAPER_COSINE)
    {
        oskar_apply_element_taper_cosine(output, num_points,
                model->cosine_power, theta, status);
    }
    else if (model->taper_type == OSKAR_ELEMENT_TAPER_GAUSSIAN)
    {
        oskar_apply_element_taper_gaussian(output, num_points,
                model->gaussian_fwhm_rad, theta, status);
    }
}

#ifdef __cplusplus
}
#endif