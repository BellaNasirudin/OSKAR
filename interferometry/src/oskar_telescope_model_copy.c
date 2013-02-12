/*
 * Copyright (c) 2011-2013, The University of Oxford
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

#include "interferometry/oskar_telescope_model_copy.h"
#include "interferometry/oskar_telescope_model_location.h"
#include "interferometry/oskar_telescope_model_type.h"
#include "interferometry/oskar_TelescopeModel.h"
#include "station/oskar_station_model_copy.h"
#include "station/oskar_station_model_init.h"
#include "utility/oskar_mem_copy.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_telescope_model_copy(oskar_TelescopeModel* dst,
        const oskar_TelescopeModel* src, int* status)
{
    int i = 0;

    /* Check all inputs. */
    if (!src || !dst || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Ensure there is enough room in the station array. */
    if (dst->num_stations < src->num_stations)
    {
        int type, location;
        type = oskar_telescope_model_type(dst);
        location = oskar_telescope_model_location(dst);
        dst->station = realloc(dst->station,
                src->num_stations * sizeof(oskar_StationModel));

        /* Initialise any newly created stations. */
        for (i = dst->num_stations; i < src->num_stations; ++i)
        {
            oskar_station_model_init(&dst->station[i], type, location, 0,
                    status);
        }
    }

    /* Copy the meta-data. */
    dst->num_stations = src->num_stations;
    dst->max_station_size = src->max_station_size;
    dst->max_station_depth = src->max_station_depth;
    dst->coord_units = src->coord_units;
    dst->identical_stations = src->identical_stations;
    dst->use_common_sky = src->use_common_sky;
    dst->seed_time_variable_station_element_errors =
            src->seed_time_variable_station_element_errors;
    dst->longitude_rad = src->longitude_rad;
    dst->latitude_rad = src->latitude_rad;
    dst->altitude_m = src->altitude_m;
    dst->ra0_rad = src->ra0_rad;
    dst->dec0_rad = src->dec0_rad;
    dst->wavelength_metres = src->wavelength_metres;
    dst->bandwidth_hz = src->bandwidth_hz;
    dst->time_average_sec = src->time_average_sec;

    /* Copy each station. */
    for (i = 0; i < src->num_stations; ++i)
    {
        oskar_station_model_copy(&(dst->station[i]), &(src->station[i]),
                status);
    }

    /* Copy the coordinates. */
    oskar_mem_copy(&dst->station_x, &src->station_x, status);
    oskar_mem_copy(&dst->station_y, &src->station_y, status);
    oskar_mem_copy(&dst->station_z, &src->station_z, status);
    oskar_mem_copy(&dst->station_x_hor, &src->station_x_hor, status);
    oskar_mem_copy(&dst->station_y_hor, &src->station_y_hor, status);
    oskar_mem_copy(&dst->station_z_hor, &src->station_z_hor, status);
}

#ifdef __cplusplus
}
#endif
