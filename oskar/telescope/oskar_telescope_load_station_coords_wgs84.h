/*
 * Copyright (c) 2016, The University of Oxford
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

#ifndef OSKAR_TELESCOPE_LOAD_STATION_COORDS_WGS84_H_
#define OSKAR_TELESCOPE_LOAD_STATION_COORDS_WGS84_H_

/**
 * @file oskar_telescope_load_station_coords_wgs84.h
 */

#include <oskar_global.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Loads a telescope coordinate file that specifies the station locations
 * with respect to the centre of the Earth.
 *
 * @details
 * A telescope station coordinate file is an ASCII text file containing
 * three columns of comma- or space-separated values that represent the station
 * (x,y,z) coordinates in the WGS84 (longitude, latitude) system.
 *
 * @param[in,out] telescope  Telescope model structure to be populated.
 * @param[in] filename       File name path to a telescope coordinate file.
 * @param[in] longitude      Telescope centre longitude, in radians.
 * @param[in] latitude       Telescope centre latitude, in radians.
 * @param[in] altitude       Telescope centre altitude, in metres.
 * @param[in,out] status     Status return code.
 */
OSKAR_EXPORT
void oskar_telescope_load_station_coords_wgs84(oskar_Telescope* telescope,
        const char* filename, double longitude, double latitude,
        double altitude, int* status);

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_TELESCOPE_LOAD_STATION_COORDS_WGS84_H_ */
