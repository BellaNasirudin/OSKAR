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

#include "station/test/evaluate_station_beam_test.h"

#include "oskar_global.h"
#include "station/oskar_evaluate_station_beam.h"
#include "station/oskar_evaluate_beam_horizontal_lmn.h"
#include "utility/oskar_get_error_string.h"
#include "utility/oskar_mem_init.h"
#include "utility/oskar_Mem.h"

#include <cmath>
#include <cstdio>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef c_0
#define c_0 299792458.0
#endif

void Evaluate_Station_Beam_Test::test_fail_conditions()
{


}


void Evaluate_Station_Beam_Test::evalute_test_pattern()
{
    int error = 0;

    double gast = 0.0;

    // Construct a station model.
    double frequency = c_0;
    int num_antennas_1d = 10;
    double station_size_m = 100.0;
    double separation_m = station_size_m / num_antennas_1d;
    double station_centre_offset_m = station_size_m / 2.0 - separation_m / 2.0;
    int num_antennas = num_antennas_1d * num_antennas_1d;
    oskar_StationModel station_cpu(OSKAR_SINGLE, OSKAR_LOCATION_CPU, num_antennas);

    // Station coordinates.
    station_cpu.longitude = 0.0;
    station_cpu.latitude  = M_PI_2;
    station_cpu.altitude  = 0.0;

    station_cpu.coord_units = OSKAR_METRES;
    for (int j = 0; j < num_antennas_1d; ++j)
    {
        for (int i = 0; i < num_antennas_1d; ++i)
        {
            int idx = j * num_antennas_1d + i;
            double x = i * separation_m - station_centre_offset_m;
            double y = j * separation_m - station_centre_offset_m;
            ((float*)(station_cpu.x.data))[idx] = x;
            ((float*)(station_cpu.y.data))[idx] = y;
            ((float*)(station_cpu.z.data))[idx] = 0.0;
        }
    }

    // Copy the station structure to the gpu and scale the coordinates to wavenumbers.
    oskar_StationModel station_gpu(&station_cpu, OSKAR_LOCATION_GPU);
    station_gpu.scale_coords_to_wavenumbers(frequency);

    // Station beam direction.
    station_cpu.ra0  = 0.0;
    station_cpu.dec0 = M_PI_2;

    // Declare an E Jones work buffer (FIXME name in context of not using with
    // evaluate E).
    oskar_WorkE work_cpu;

    // Evaluate horizontal l,m,n for beam phase centre.
    error = oskar_evaluate_beam_hoizontal_lmn(&work_cpu, &station_gpu, gast);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);

    // Evalute horizontal l,m positions at which to generate the beam pattern.
    int image_size = 100;
    double fov_deg = 30.0;
    int num_pixels = image_size * image_size;

    error = oskar_mem_init(&work_cpu.hor_l, OSKAR_SINGLE, OSKAR_LOCATION_CPU,
            num_pixels, OSKAR_TRUE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);
    error = oskar_mem_init(&work_cpu.hor_m, OSKAR_SINGLE, OSKAR_LOCATION_CPU,
            num_pixels, OSKAR_TRUE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);

    float* lm = (float*)malloc(image_size * sizeof(float));
    // linspace.
    double lm_max = sin(fov_deg * M_PI / 180.0);
    double lm_inc = (2.0 * lm_max) / (image_size - 1);
    for (int i = 0; i < image_size; ++i)
    {
        lm[i] = -lm_max + i * lm_inc;
    }
    // meshgrid.
    for (int j = 0; j < image_size; ++j)
    {
        for (int i = 0; i < image_size; ++i)
        {
            ((float*)work_cpu.hor_l.data)[j * image_size + i] = lm[i];
            ((float*)work_cpu.hor_m.data)[i * image_size + j] = lm[image_size - 1 - i];
        }
    }
    free(lm);

    // Copy horizontal l,m work array buffers to GPU.
    oskar_WorkE work_gpu;
    error = oskar_mem_init(&work_gpu.hor_l, OSKAR_SINGLE, OSKAR_LOCATION_GPU,
            num_pixels, OSKAR_TRUE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);
    error = oskar_mem_init(&work_gpu.hor_m, OSKAR_SINGLE, OSKAR_LOCATION_GPU,
            num_pixels, OSKAR_TRUE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);
    error = work_cpu.hor_l.copy_to(&work_gpu.hor_l);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);
    error = work_cpu.hor_m.copy_to(&work_gpu.hor_m);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);

    // Allocate weights work array.
    error = oskar_mem_init(&work_gpu.weights, OSKAR_SINGLE_COMPLEX,
            OSKAR_LOCATION_GPU, num_antennas, OSKAR_TRUE);

    // Declare memory for the beam pattern.
    oskar_Mem beam_pattern(OSKAR_SINGLE_COMPLEX, OSKAR_LOCATION_GPU, num_pixels);

    error = oskar_evaluate_station_beam(&beam_pattern, &station_gpu, &work_gpu);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(oskar_get_error_string(error), 0, error);

    // Copy beam pattern back to CPU.
    oskar_Mem beam_pattern_cpu(&beam_pattern, OSKAR_LOCATION_CPU);

    // todo: save beam to file for plotting.
    const char* filename = "temp_test_beam_pattern.txt";
    FILE* file = fopen(filename, "w");
    for (int i = 0; i < num_pixels; ++i)
    {
        fprintf(file, "%10.3f,%10.3f,%10.3f,%10.3f\n",
                ((float*)work_cpu.hor_l)[i],
                ((float*)work_cpu.hor_m)[i],
                ((float2*)(beam_pattern_cpu.data))[i].x,
                ((float2*)(beam_pattern_cpu.data))[i].y);
    }
    fclose(file);
}


void Evaluate_Station_Beam_Test::performance_test()
{
}





