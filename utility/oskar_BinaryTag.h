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

#ifndef OSKAR_BINARY_TAG_H_
#define OSKAR_BINARY_TAG_H_

/**
 * @file oskar_BinaryTag.h
 */

#include "oskar_global.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold tag data from an OSKAR binary file.
 *
 * @details
 * This structure holds data for a single tag in an OSKAR binary file.
 * The tag is exactly 16 bytes long and contains the following data:
 *
 * Offset    Length    Description
 * ----------------------------------------------------------------------------
 *  0         4        The string "TAG" in ASCII format, with trailing zero.
 *  4         1        Tag identifier byte (enumerator).
 *  5         1        User tag identifier byte 1.
 *  6         1        User tag identifier byte 2.
 *  7         1        Data type (as used by oskar_Mem) of the data block.
 *  8         8        Block size in bytes, as little-endian 8-byte integer.
 */
struct oskar_BinaryTag
{
    char magic[4];           /**< Magic number (ASCII "TAG"). */
    unsigned char id;        /**< Tag identifier (enumerator). */
    unsigned char id_user_1; /**< User tag identifier byte 1. */
    unsigned char id_user_2; /**< User tag identifier byte 2. */
    unsigned char data_type; /**< Type (as oskar_Mem) of data block. */
    char size_bytes[8];      /**< Block size in bytes, as little-endian 8-byte integer. */
};
typedef struct oskar_BinaryTag oskar_BinaryTag;

/**
 * @brief Structure to hold tag index data from an OSKAR binary file.
 *
 * @details
 * This structure holds an index of tags found in an OSKAR binary file,
 * and the offset in bytes from the start of the file of each data block.
 * It can be used to find an item of the required type.
 */
struct oskar_BinaryTagIndex
{
    int num_tags;             /**< Size of tag arrays. */
    oskar_BinaryTag* tag;     /**< Array of tags in the file. */
    long* block_offset_bytes; /**< Array of block offsets from the start. */
};
typedef struct oskar_BinaryTagIndex oskar_BinaryTagIndex;

/* NOTE: To maintain binary data compatibility, do not modify any numbers
 * that appear in the list below! */
enum
{
    /* User tag identifier. */
    OSKAR_TAG_USER = 0,

    /* File data. */
    OSKAR_TAG_FILE_CREATION_DATE_STRING = 1,
    OSKAR_TAG_FILE_DESCRIPTION_STRING = 2,
    OSKAR_TAG_FILE_AUTHOR_STRING = 3,
    OSKAR_TAG_SETTINGS_FILE = 4,

    /* Sky model data. */
    OSKAR_TAG_NUM_SOURCES = 10,
    OSKAR_TAG_SOURCE_RA_RAD = 11,
    OSKAR_TAG_SOURCE_DEC_RAD = 12,
    OSKAR_TAG_SOURCE_I = 13,
    OSKAR_TAG_SOURCE_Q = 14,
    OSKAR_TAG_SOURCE_U = 15,
    OSKAR_TAG_SOURCE_V = 16,
    OSKAR_TAG_SOURCE_SPECTRAL_INDEX = 17,
    OSKAR_TAG_SOURCE_REF_FREQ_HZ = 18,
    OSKAR_TAG_SOURCE_MAJOR_AXIS_RAD = 19,
    OSKAR_TAG_SOURCE_MINOR_AXIS_RAD = 20,
    OSKAR_TAG_SOURCE_POSITION_ANGLE_RAD = 21,

    /* Telescope data. */
    OSKAR_TAG_TELESCOPE_LON_RAD = 30,
    OSKAR_TAG_TELESCOPE_LAT_RAD = 31,
    OSKAR_TAG_TELESCOPE_ALT_M = 32,
    OSKAR_TAG_NUM_STATIONS = 33,
    OSKAR_TAG_STATION_X_POSITONS = 34,
    OSKAR_TAG_STATION_Y_POSITONS = 35,
    OSKAR_TAG_STATION_Z_POSITONS = 36,
    OSKAR_TAG_STATION_COORD_SYS = 37,
    OSKAR_TAG_STATION_COORD_UNIT = 38,

    /* Observation data. */
    OSKAR_TAG_OBS_PHASE_CENTRE_RA_RAD = 60,
    OSKAR_TAG_OBS_PHASE_CENTRE_DEC_RAD = 61,
    OSKAR_TAG_OBS_START_TIME_MJD_UTC = 62,
    OSKAR_TAG_OBS_LENGTH_S = 63,

    /* Visibility data. */
    OSKAR_TAG_NUM_TIMES = 70,
    OSKAR_TAG_NUM_CHANNELS = 71,
    OSKAR_TAG_NUM_BASELINES = 72,
    OSKAR_TAG_NUM_POLARISATIONS = 73,
    OSKAR_TAG_VISIBILITY_DATA = 74,
    OSKAR_TAG_VISIBILITY_AXIS_ORDER = 75,
    OSKAR_TAG_VISIBILITY_POLARISATION_TYPE = 76,
    OSKAR_TAG_BASELINE_UU = 77,
    OSKAR_TAG_BASELINE_VV = 78,
    OSKAR_TAG_BASELINE_WW = 79,
    OSKAR_TAG_BASELINE_UNIT = 80,
    OSKAR_TAG_FREQ_START_HZ = 81,
    OSKAR_TAG_FREQ_INC_HZ = 82,
    OSKAR_TAG_FREQ_CHANNEL_BANDWIDTH_HZ = 83,
    OSKAR_TAG_TIME_INTERVAL_S = 84,
    OSKAR_TAG_TIME_EXPOSURE_S = 85,

    /* Image data. */
    OSKAR_TAG_NUM_PIXELS_WIDTH = 100,
    OSKAR_TAG_NUM_PIXELS_HEIGHT = 101,
    OSKAR_TAG_FITS_IMAGE_DATA = 103,

    /* Largest possible standard tag identifier. */
    OSKAR_TAG_LARGEST_STANDARD_TAG_ID = 255
};

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_BINARY_TAG_H_ */