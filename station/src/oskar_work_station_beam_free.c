/*
 * Copyright (c) 2012, The University of Oxford
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

#include "station/oskar_work_station_beam_free.h"
#include "utility/oskar_mem_free.h"

#ifdef __cplusplus
extern "C" {
#endif

void oskar_work_station_beam_free(oskar_WorkStationBeam* work, int* status)
{
    /* Check all inputs. */
    if (!work || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    oskar_mem_free(&work->horizon_mask, status);
    oskar_mem_free(&work->theta_modified, status);
    oskar_mem_free(&work->phi_modified, status);
    oskar_mem_free(&work->hor_x, status);
    oskar_mem_free(&work->hor_y, status);
    oskar_mem_free(&work->hor_z, status);
    oskar_mem_free(&work->rel_l, status);
    oskar_mem_free(&work->rel_m, status);
    oskar_mem_free(&work->rel_n, status);
    oskar_mem_free(&work->weights, status);
    oskar_mem_free(&work->weights_error, status);
    oskar_mem_free(&work->E, status);
    oskar_mem_free(&work->G_matrix, status);
    oskar_mem_free(&work->G_scalar, status);
}

#ifdef __cplusplus
}
#endif
