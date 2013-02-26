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

#include <mex.h>

#include <oskar_global.h>
#include <interferometry/oskar_Visibilities.h>
#include <utility/oskar_get_error_string.h>
#include <interferometry/oskar_visibilities_free.h>
#include <interferometry/oskar_visibilities_write.h>
#include <interferometry/oskar_visibilities_init.h>
#include <utility/oskar_Log.h>
#include <utility/oskar_mem_init.h>

#include "matlab/visibilities/lib/oskar_mex_vis_from_matlab_struct.h"
#include "matlab/common/oskar_matlab_common.h"

void mexFunction(int num_out, mxArray** /*out*/, int num_in, const mxArray** in)
{
    if (num_in != 2 || num_out > 0)
    {
        oskar_matlab_usage(NULL, "visibilities", "write", "<file name>, <vis>",
                "Writes an OSKAR visibilities binary file from the specified"
                "OSKAR MATLAB visibilities structure.");
    }

    int status = OSKAR_SUCCESS;

    // Extract arguments from MATLAB mxArray objects.
    const char* filename = mxArrayToString(in[0]);
    oskar_Visibilities vis;
    oskar_mex_vis_from_matlab_struct(&vis, in[1]);

    oskar_visibilities_write(&vis, 0, filename, &status);
    if (status)
    {
        oskar_matlab_error("oskar_visibilities_write() failed: %s",
                oskar_get_error_string(status));
    }

    oskar_visibilities_free(&vis, &status);
    if (status)
    {
        oskar_matlab_error("Faild to write visibilities file: %s",
                oskar_get_error_string(status));
    }
}
