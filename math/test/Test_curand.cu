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

#include <gtest/gtest.h>

#include <private_random_state.h>
#include <oskar_random_state.h>
#include <oskar_get_error_string.h>
#include <oskar_mem.h>

#include <cuda.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <omp.h>

using namespace std;

#include <curand_kernel.h>

// Test CURAND kernel.
__global__
void test_curand_generate(double* values, int num_values,
        int num_per_thread, curandStateXORWOW* state, int num_states)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid >= num_states) return;

    for (int i = 0; i < num_per_thread; ++i)
    {
        int idx = num_per_thread * tid + i;
        if (idx >= num_values) continue;
        values[idx] = curand_normal_double(&state[tid]);
    }
}


TEST(curand, test)
{
    int offset = 0;
    int seed   = 0;
    int num_threads = 100;
    int num_values_per_thread = 1;
    int device_offset = 0;
    int status = 0;
    FILE* file = NULL;
    const char* filename = "temp_test_curand.txt";
    file = fopen(filename, "w");

    int num_values = num_threads * num_values_per_thread;
    int num_states = num_threads;
    double* d_values;
    double* h_values;
    cudaMalloc(&d_values, num_values * sizeof(double));
    h_values = (double*) malloc(num_values * sizeof(double));

    int num_blocks = (num_values + num_threads - 1) / num_threads;

    // Create and initialise the CURAND states.
    oskar_RandomState* random_state = oskar_random_state_create(num_states,
            seed, offset, device_offset, &status);

    if (file)
    {
        fprintf(file, "--------\n");
        fprintf(file, "num_values            = %i\n", num_values);
        fprintf(file, "num_values_per_thread = %i\n", num_values_per_thread);
        fprintf(file, "num_blocks            = %i\n", num_blocks);
        fprintf(file, "num_threads           = %i\n", num_threads);
        fprintf(file, "num states            = %i\n", num_states);
        fprintf(file, "\n");
        fprintf(file, "offset                = %i\n", offset);
        fprintf(file, "seed                  = %i\n", seed);
        fprintf(file, "--------\n");
    }

    // Generate some random numbers.
    int num_sets = 3;
    for (int j = 0; j < num_sets; ++j)
    {
        test_curand_generate
            OSKAR_CUDAK_CONF(num_blocks, num_threads)
            (d_values, num_values, num_values_per_thread, random_state->state,
                    num_states);
        cudaMemcpy(h_values, d_values, num_values * sizeof(double), cudaMemcpyDeviceToHost);
        if (file)
        {
            for (int i = 0; i < num_values; ++i)
            {
                fprintf(file, "%i %f\n", i, h_values[i]);
            }
            fprintf(file, "\n");
        }
    }

    if (file) fclose(file);
    if (d_values) cudaFree(d_values);
    if (h_values) free(h_values);
    oskar_random_state_free(random_state, &status);
}


TEST(curand, test_state_allocation)
{
    FILE* file = NULL;
    const char* filename = "temp_test_curand_1.txt";
    file = fopen(filename, "w");

    // Allocate a number of CURAND states.
    int offset = 0;
    int seed = 0;
    int num_states = (int)2e5;
    int status = 0;
    oskar_RandomState* random_state = oskar_random_state_create(num_states,
            seed, offset, 0, &status);
    ASSERT_EQ(0, status) << oskar_get_error_string(status);

    int num_iter = 1;
    int num_blocks  = 2;
    int num_threads = 20;
    int num_per_thread = 1;
    int num_values = num_blocks * num_threads * num_per_thread;
    oskar_Mem d_values(OSKAR_DOUBLE, OSKAR_LOCATION_GPU, num_values);
    double* d_values_ = oskar_mem_double(&d_values, &status);

    for (int i = 0; i < num_iter; ++i)
    {
        test_curand_generate
        OSKAR_CUDAK_CONF(num_blocks, num_threads)
        (d_values_, num_values, num_per_thread, random_state->state, num_states);

        oskar_Mem h_values(&d_values, OSKAR_LOCATION_CPU);

        if (file)
        {
            for (int i = 0; i < num_values; ++i)
            {
                fprintf(file, "%i %f\n", i, ((double*)h_values.data)[i]);
            }
        }
    }
    if (file) fclose(file);
    oskar_random_state_free(random_state, &status);
}


TEST(curand, test_multi_device)
{
    int num_devices = 0;
    cudaGetDeviceCount(&num_devices);
    omp_set_num_threads(min(num_devices, 2));
    int use_device[4] = {0, 1, 2, 3};

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int error = (int)cudaSetDevice(use_device[thread_id]);
        EXPECT_EQ(0, error) << oskar_get_error_string(error);

        int device_id = 0;
        cudaGetDevice(&device_id);

        char filename[100];
        sprintf(filename, "temp_test_curand_device_%i.txt", device_id);
        FILE* file = NULL;

        // Allocate a number of curand states.
        int seed = 0;
        int num_states = (int)2e5;
        oskar_RandomState* d_states = oskar_random_state_create(num_states,
                seed, 0, 0, &error);

        EXPECT_EQ(0, error) << oskar_get_error_string(error);

        int num_iter = 2;
        int num_blocks  = 1;
        int num_threads = 2;
        int num_per_thread = 2;
        int num_values = num_blocks * num_threads * num_per_thread;
        oskar_Mem d_values(OSKAR_DOUBLE, OSKAR_LOCATION_GPU, num_values);
        double* d_values_ = oskar_mem_double(&d_values, &error);

        file = fopen(filename, "w");

        for (int i = 0; i < num_iter; ++i)
        {
            test_curand_generate
                OSKAR_CUDAK_CONF(num_blocks, num_threads)
                (d_values_, num_values, num_per_thread, d_states->state, num_states);

            oskar_Mem h_values(&d_values, OSKAR_LOCATION_CPU);

            if (file)
            {
                for (int i = 0; i < num_values; ++i)
                {
                    fprintf(file, "%i %f\n", i, ((double*)h_values.data)[i]);
                }
            }
        }
        fclose(file);
        oskar_random_state_free(d_states, &error);
        EXPECT_EQ(0, error) << oskar_get_error_string(error);
    }
}