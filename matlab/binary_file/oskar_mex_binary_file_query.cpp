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

#include "matlab/common/oskar_matlab_common.h"

#include <mex.h>
#include <utility/oskar_BinaryTag.h>
#include <utility/oskar_binary_tag_index_create.h>
#include <utility/oskar_binary_tag_index_free.h>
#include <utility/oskar_get_data_type_string.h>
#include <utility/oskar_get_error_string.h>

// MATLAB Entry function.
void mexFunction(int num_out, mxArray** out, int num_in, const mxArray** in)
{
    int err = 0;
    if (num_in != 1 || num_out > 2)
    {
        oskar_matlab_usage("[index, headers]", "binary_file", "query",
                "<filename>", "Queries the contents of an OSKAR binary file "
                "returning a cell array of record indices and an array of "
                "record header structures.");
    }

    // Get input args
    char filename[100];
    mxGetString(in[0], filename, 100);

    // Run 'format long' command
    mxArray* arg = mxCreateString("long");
    mexCallMATLAB(0, NULL, 1, &arg, "format");

    FILE* file = fopen(filename, "r");
    if (!file)
        oskar_matlab_error("Unable to open file: '%s'", filename);

    oskar_BinaryTagIndex* index = NULL;
    oskar_binary_tag_index_create(&index, file, &err);
    if (err) oskar_matlab_error("oskar_binary_tag_index_create() failed with "
            "code %i: %s",err, oskar_get_error_string(err));

    mexPrintf("= Binary file header loaded (%i tags found).\n", index->num_tags);
    int num_fields = 5;
    const char* fields[5] = { "type", "group", "tag", "index", "data_size"};

    int num_records = index->num_tags;
    out[0] = mxCreateCellMatrix(num_records + 1, 6);
    out[1] = mxCreateStructMatrix(num_records, 1, num_fields, fields);

    mxSetCell(out[0], 0 * (num_records + 1), mxCreateString("[ID]"));
    mxSetCell(out[0], 1 * (num_records + 1), mxCreateString("[TYPE]"));
    mxSetCell(out[0], 2 * (num_records + 1), mxCreateString("[GROUP]"));
    mxSetCell(out[0], 3 * (num_records + 1), mxCreateString("[TAG]"));
    mxSetCell(out[0], 4 * (num_records + 1), mxCreateString("[INDEX]"));
    mxSetCell(out[0], 5 * (num_records + 1), mxCreateString("[BYTES]"));

    for (int i = 0; i < num_records; ++i)
    {
        mxSetCell(out[0], 0 * (num_records + 1) + i + 1, mxCreateDoubleScalar((double)i+1));
        const char* data_name = oskar_get_data_type_string(index->data_type[i]);
        mxSetField(out[1], i, fields[0], mxCreateString(data_name));
        mxSetCell(out[0], 1 * (num_records + 1) + i + 1, mxCreateString(data_name));
        if (index->extended[i])
        {
            mxSetField(out[1], i, fields[1],
                    mxCreateString(index->name_group[i]));
            mxSetCell(out[0], 2 * (num_records + 1) + i + 1, mxCreateString(index->name_group[i]));
            mxSetField(out[1], i, fields[2],
                    mxCreateString(index->name_tag[i]));
            mxSetCell(out[0], 3 * (num_records + 1) + i + 1, mxCreateString(index->name_tag[i]));
        }
        else
        {
            mxSetField(out[1], i, fields[1],
                    mxCreateDoubleScalar((double)index->id_group[i]));
            mxSetCell(out[0], 2 * (num_records + 1) + i + 1,
                    mxCreateDoubleScalar((double)index->id_group[i]));
            mxSetField(out[1], i, fields[2],
                    mxCreateDoubleScalar((double)index->id_tag[i]));
            mxSetCell(out[0], 3 * (num_records + 1) + i + 1,
                    mxCreateDoubleScalar((double)index->id_tag[i]));
        }
        mxSetField(out[1], i, fields[3],
                mxCreateDoubleScalar((double)index->user_index[i]));
        mxSetCell(out[0], 4 * (num_records + 1) + i + 1,
                mxCreateDoubleScalar((double)index->user_index[i]));
        mxSetField(out[1], i, fields[4],
                mxCreateDoubleScalar((double)index->data_size_bytes[i]));
        mxSetCell(out[0], 5 * (num_records + 1) + i + 1,
                mxCreateDoubleScalar((double)index->data_size_bytes[i]));
    }
    oskar_binary_tag_index_free(&index, &err);
    fclose(file);
}
