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


#include <mex.h>
#include "utility/oskar_Mem.h"
#include "utility/oskar_BinaryTag.h"
#include "utility/oskar_binary_tag_index_query.h"
#include "utility/oskar_binary_file_read.h"
#include "utility/oskar_binary_tag_index_create.h"
#include "utility/oskar_binary_tag_index_free.h"
#include "utility/oskar_binary_stream_read.h"
#include "utility/oskar_get_error_string.h"
#include "utility/oskar_get_data_type_string.h"
#include "utility/oskar_mem_type_check.h"
#include "utility/oskar_vector_types.h"

#include <string.h>

#define STR_LENGTH 200

// MATLAB Entry function.
void mexFunction(int num_out, mxArray** out, int num_in, const mxArray** in)
{
    if (num_in < 1 || num_in > 4 || num_out > 1)
    {
        mexErrMsgTxt("Usage: record = oskar_binary_file_read_record(filename,"
                " [group = 0], [tag = 0], [index = 0])\n");
    }

    /* Local variables */
    int err = OSKAR_SUCCESS;
    char filename[STR_LENGTH];
    int group_id = 0, tag_id = 0, index_id = 0;
    char group_name[STR_LENGTH], tag_name[STR_LENGTH];
    group_name[0] = '\0';
    tag_name[0] = '\0';
    bool is_extended = false;

    /* Get args */
    mxGetString(in[0], filename, STR_LENGTH);

    if (num_in == 2 && mxIsChar(in[1]))
    {
        is_extended = true;
    }
    else if (num_in == 3 || num_in == 4)
    {
        if (mxIsChar(in[1]) && mxIsChar(in[2]))
            is_extended = true;
        else if (mxIsDouble(in[1]) && mxIsDouble(in[2]))
            is_extended = false;
        else if (mxIsInt32(in[1]) && mxIsInt32(in[2]))
            is_extended = false;
        else
            mexErrMsgTxt("ERROR: Specified Group & Tag must be of the same type"
                    " (scalar or string).");
    }

    if (num_in >= 2)
    {
        if (mxIsDouble(in[1]))
        {
            group_id = (int)mxGetScalar(in[1]);
        }
        else if (mxIsChar(in[1]))
        {
            mxGetString(in[1], group_name, STR_LENGTH);
        }
    }
    if (num_in >= 3)
    {
        if (mxIsDouble(in[2]))
        {
            tag_id = (int)mxGetScalar(in[2]);
            is_extended = false;
        }
        else if (mxIsChar(in[2]))
        {
            mxGetString(in[2], tag_name, STR_LENGTH);
            is_extended = true;
        }
    }
    if (num_in >= 4)
    {
        index_id = (int)mxGetScalar(in[3]);
    }

    if (is_extended)
        mexPrintf("= Reading record (tag.group.index) = %s.%s.%i\n",
                group_name, tag_name, index_id);
    else
        mexPrintf("= Reading record (tag.group.index) = %i.%i.%i\n",
                group_id, tag_id, index_id);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        mexErrMsgIdAndTxt("OSKAR:ERROR", "Unable to open file (%s)\n.", filename);
    }

    oskar_BinaryTagIndex* index = NULL;
    err = oskar_binary_tag_index_create(&index, file);
    if (err)
    {
        mexErrMsgIdAndTxt("OSKAR:ERROR", "ERROR: oskar_binary_tag_index_create() "
                "failed with code %i: %s.\n", err, oskar_get_error_string(err));
    }

    int idx = -1;
    for (int i = 0; i < index->num_tags; ++i)
    {
        if (is_extended && index->extended[i])
        {
            if (strcmp(index->name_group[i], group_name) == 0 &&
                    strcmp(index->name_tag[i], tag_name) == 0 &&
                    index->user_index[i] == index_id)
            {
                idx = i;
                break;
            }
        }
        else if (!is_extended && !index->extended[i])
        {
            if (index->id_group[i] == group_id &&
                    index->id_tag[i] == tag_id &&
                    index->user_index[i] == index_id)
            {
                idx = i;
                break;
            }
        }
    }

    if (idx == -1)
        mexErrMsgTxt("ERROR: Specified group, tag and index not found in binary file\n");

    int num_fields = 5;
    const char* fields[5] = {
            "type",
            "group",
            "tag",
            "index",
            "data"
    };
    out[0] = mxCreateStructMatrix(1, 1, num_fields, fields);
    const char* data_name = oskar_get_data_type_string(index->data_type[idx]);
    mxSetField(out[0], 0, fields[0], mxCreateString(data_name));
    if (index->extended[idx])
    {
        mxSetField(out[0], 0, fields[1],
                mxCreateString(index->name_group[idx]));
        mxSetField(out[0], 0, fields[2],
                mxCreateString(index->name_tag[idx]));
    }
    else
    {
        mxSetField(out[0], 0, fields[1],
                mxCreateDoubleScalar((double)index->id_group[idx]));
        mxSetField(out[0], 0, fields[2],
                mxCreateDoubleScalar((double)index->id_tag[idx]));
    }
    mxSetField(out[0], 0, fields[3],
            mxCreateDoubleScalar((double)index->user_index[idx]));
    mxArray* data_ = NULL;
    void* data = NULL;
    mwSize m = 0;
    switch (index->data_type[idx])
    {
        case OSKAR_CHAR:
        {
            data = malloc(index->data_size_bytes[idx]);
            break;
        }
        case OSKAR_INT:
        {
            m = index->data_size_bytes[idx] / sizeof(int);
            data_ = mxCreateNumericMatrix(m, 1, mxINT32_CLASS, mxREAL);
            data = mxGetData(data_);
            break;
        }
        case OSKAR_SINGLE:
        {
            m = index->data_size_bytes[idx] / sizeof(float);
            data_ = mxCreateNumericMatrix(m, 1, mxSINGLE_CLASS, mxREAL);
            data = mxGetData(data_);
            break;
        }
        case OSKAR_DOUBLE:
        {
            m = index->data_size_bytes[idx] / sizeof(double);
            data_ = mxCreateNumericMatrix(m, 1, mxDOUBLE_CLASS, mxREAL);
            data = mxGetData(data_);
            break;
        }
        case OSKAR_SINGLE_COMPLEX:
        {
            m = index->data_size_bytes[idx] / sizeof(float2);
            data_ = mxCreateNumericMatrix(m, 1, mxSINGLE_CLASS, mxCOMPLEX);
            data = malloc(index->data_size_bytes[idx]);
            break;
        }
        case OSKAR_DOUBLE_COMPLEX:
        {
            m = index->data_size_bytes[idx] / sizeof(double2);
            data_ = mxCreateNumericMatrix(m, 1, mxDOUBLE_CLASS, mxCOMPLEX);
            data = malloc(index->data_size_bytes[idx]);
            break;
        }
        case OSKAR_SINGLE_COMPLEX_MATRIX:
        {
            m = index->data_size_bytes[idx] / sizeof(float4c);
            mwSize dims[3] = {2, 2, m};
            data_ = mxCreateNumericArray(3, dims, mxSINGLE_CLASS, mxCOMPLEX);
            data = malloc(index->data_size_bytes[idx]);
            break;
        }
        case OSKAR_DOUBLE_COMPLEX_MATRIX:
        {
            m = index->data_size_bytes[idx] / sizeof(double4c);
            mwSize dims[3] = {2, 2, m};
            data_ = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxCOMPLEX);
            data = malloc(index->data_size_bytes[idx]);
            break;
        }
        default:
            mexErrMsgTxt("Unknown OSKAR data type");
            break;
    };

    if (index->extended[idx])
    {
        err = oskar_binary_stream_read_ext(file, &index,
                (unsigned char)index->data_type[idx],
                index->name_group[idx], index->name_tag[idx],
                index->user_index[idx], (size_t)index->data_size_bytes[idx],
                data);
    }
    else
    {
        err = oskar_binary_stream_read(file, &index,
                (unsigned char)index->data_type[idx],
                (unsigned char)group_id, (unsigned char)tag_id,
                index->user_index[idx], (size_t)index->data_size_bytes[idx],
                data);
    }
    if (err)
    {
        mexErrMsgIdAndTxt("OSKAR:ERROR", "ERROR: oskar_binary_file_read() "
                "failed with code %i: %s.\n", err, oskar_get_error_string(err));
    }

    /* If the data is a char array convert to MATLAB string (16bit char format). */
    if (index->data_type[idx] == OSKAR_CHAR)
    {
        data_ = mxCreateString((char*)data);
        free(data);
    }
    else if (index->data_type[idx] == OSKAR_SINGLE_COMPLEX)
    {
        float* re = (float*)mxGetPr(data_);
        float* im = (float*)mxGetPi(data_);
        for (unsigned i = 0; i < m; ++i)
        {
            re[i] = ((float2*)data)[i].x;
            im[i] = ((float2*)data)[i].y;
        }
        free(data);
    }
    else if (index->data_type[idx] == OSKAR_DOUBLE_COMPLEX)
    {
        double* re = mxGetPr(data_);
        double* im = mxGetPi(data_);
        for (unsigned i = 0; i < m; ++i)
        {
            re[i] = ((double2*)data)[i].x;
            im[i] = ((double2*)data)[i].y;
        }
        free(data);
    }
    else if (index->data_type[idx] == OSKAR_SINGLE_COMPLEX_MATRIX)
    {
        float* re = (float*)mxGetPr(data_);
        float* im = (float*)mxGetPi(data_);
        for (unsigned i = 0; i < m; ++i)
        {
            re[4*i+0] = ((float4c*)data)[i].a.x;
            im[4*i+0] = ((float4c*)data)[i].a.y;
            re[4*i+1] = ((float4c*)data)[i].b.x;
            im[4*i+1] = ((float4c*)data)[i].b.y;
            re[4*i+2] = ((float4c*)data)[i].c.x;
            im[4*i+2] = ((float4c*)data)[i].c.y;
            re[4*i+3] = ((float4c*)data)[i].d.x;
            im[4*i+3] = ((float4c*)data)[i].d.y;
        }
        free(data);
    }
    else if (index->data_type[idx] == OSKAR_DOUBLE_COMPLEX_MATRIX)
    {
        double* re = mxGetPr(data_);
        double* im = mxGetPi(data_);
        for (unsigned i = 0; i < m; ++i)
        {
            re[4*i+0] = ((double4c*)data)[i].a.x;
            im[4*i+0] = ((double4c*)data)[i].a.y;
            re[4*i+1] = ((double4c*)data)[i].b.x;
            im[4*i+1] = ((double4c*)data)[i].b.y;
            re[4*i+2] = ((double4c*)data)[i].c.x;
            im[4*i+2] = ((double4c*)data)[i].c.y;
            re[4*i+3] = ((double4c*)data)[i].d.x;
            im[4*i+3] = ((double4c*)data)[i].d.y;
        }
        free(data);
    }
    mxSetField(out[0], 0, fields[4], data_);

    fclose(file);
    err = oskar_binary_tag_index_free(&index);
    if (err)
    {
        mexErrMsgIdAndTxt("OSKAR:ERROR", "ERROR: oskar_binary_tag_index_free() "
                "failed with code %i: %s.\n", err, oskar_get_error_string(err));
    }
}

