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

#include "oskar_global.h"

#include <cuda_runtime_api.h>
#include "apps/lib/oskar_Settings.h"
#include "apps/lib/oskar_set_up_sky.h"
#include "apps/lib/oskar_set_up_telescope.h"
#include "apps/lib/oskar_set_up_visibilities.h"
#include "apps/lib/oskar_write_ms.h"
#include "interferometry/oskar_evaluate_baseline_uvw.h"
#include "interferometry/oskar_interferometer.h"
#include "interferometry/oskar_SimTime.h"
#include "interferometry/oskar_TelescopeModel.h"
#include "interferometry/oskar_Visibilities.h"
#include "sky/oskar_SkyModel.h"
#include "utility/oskar_exit.h"
#include "utility/oskar_Mem.h"

#define TIMER_ENABLE 1
#include "utility/timer.h"

#include <cstdio>
#include <cstdlib>
#include <QtCore/QByteArray>
#include <QtCore/QTime>

int main(int argc, char** argv)
{
    // Parse command line.
    int err = 0;
    if (argc != 2)
    {
        fprintf(stderr, "ERROR: Missing command line arguments.\n");
        fprintf(stderr, "Usage:  $ oskar_sim [settings file]\n");
        return EXIT_FAILURE;
    }

    // Load the settings file.
    oskar_Settings settings;
    if (!settings.load(QString(argv[1]))) return EXIT_FAILURE;
    settings.print();
    int type = settings.double_precision() ? OSKAR_DOUBLE : OSKAR_SINGLE;
    const oskar_SimTime* times = settings.obs().sim_time();

    // Get the sky model and telescope model and copy both to GPU (slow step).
    oskar_SkyModel *sky_cpu, *sky_gpu;
    oskar_TelescopeModel *tel_cpu, *tel_gpu;
    sky_cpu = oskar_set_up_sky(settings);
    tel_cpu = oskar_set_up_telescope(settings);
    if (tel_cpu == NULL) oskar_exit(OSKAR_ERR_FILE_IO);
    sky_gpu = new oskar_SkyModel(sky_cpu, OSKAR_LOCATION_GPU);
    tel_gpu = new oskar_TelescopeModel(tel_cpu, OSKAR_LOCATION_GPU);

    // Create the global visibility structure on the CPU.
    oskar_Visibilities* vis_global = oskar_set_up_visibilities(settings,
            tel_cpu, type | OSKAR_COMPLEX | OSKAR_MATRIX);

    // Run the simulation.
    QTime timer;
    timer.start();
    int n_channels = settings.obs().num_channels();
    for (int c = 0; c < n_channels; ++c)
    {
        // Get a pointer to the visibility channel data.
        oskar_Mem vis_amp;
        printf("\n--> Simulating channel (%d / %d).\n", c + 1, n_channels);
        vis_global->get_channel_amps(&vis_amp, c);

        // Simulate data for this channel.
        err = oskar_interferometer(&vis_amp, sky_gpu, tel_gpu, times,
                settings.obs().frequency(c));
        if (err) oskar_exit(err);
    }

    if (settings.sky().noise_model().toUpper() == "VLA_MEMO_146")
    {
        printf("== Adding Gaussian visibility noise.\n");
        err = vis_global->evaluate_sky_noise_stddev(tel_cpu,
                settings.sky().noise_spectral_index());
        if (err) oskar_exit(err);
        err = vis_global->add_sky_noise(vis_global->sky_noise_stddev,
            settings.sky().noise_seed());
        if (err) oskar_exit(err);
    }

    printf("\n=== Simulation completed in %f sec.\n", timer.elapsed() / 1.0e3);

    // Compute baseline u,v,w coordinates for simulation.
    err = oskar_evaluate_baseline_uvw(vis_global, tel_cpu, times);
    if (err) oskar_exit(err);

    // Write global visibilities to disk.
    if (!settings.obs().oskar_vis_filename().isEmpty())
    {
        QByteArray outname = settings.obs().oskar_vis_filename().toAscii();
        printf("--> Writing visibility file: '%s'\n", outname.constData());
        err = vis_global->write(outname);
        if (err) oskar_exit(err);
    }

#ifndef OSKAR_NO_MS
    // Write Measurement Set.
    if (!settings.obs().ms_filename().isEmpty())
    {
        QByteArray outname = settings.obs().ms_filename().toAscii();
        printf("--> Writing Measurement Set: '%s'\n", outname.constData());
        err = oskar_write_ms(outname, vis_global, tel_cpu, (int)OSKAR_TRUE);
        if (err) oskar_exit(err);
    }
#endif

    // Delete data structures.
    delete vis_global;
    delete sky_cpu;
    delete sky_gpu;
    delete tel_gpu;
    delete tel_cpu;
    cudaDeviceReset();

    return EXIT_SUCCESS;
}
