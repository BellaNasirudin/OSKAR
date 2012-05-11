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

#ifndef OSKAR_ELEMENT_MODEL_LOAD_CST_H_
#define OSKAR_ELEMENT_MODEL_LOAD_CST_H_

/**
 * @file oskar_element_model_load_cst.h
 */

#include "oskar_global.h"
#include "station/oskar_ElementModel.h"
#include "station/oskar_SettingsElementFit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * Loads an antenna pattern from an ASCII text file produced by CST.
 *
 * @details
 * This function loads antenna pattern data from a text file and fills the
 * provided data structure.
 *
 * The data file must contain eight columns, in the following order:
 * - <theta, deg>
 * - <phi, deg>
 * - <abs dir>
 * - <abs theta>
 * - <phase theta, deg>
 * - <abs phi>
 * - <phase phi, deg>
 * - <ax. ratio>
 *
 * This is the format exported by the CST (Computer Simulation Technology)
 * package.
 *
 * Amplitude values in dBi are detected, and converted to linear format
 * on loading.
 *
 * The theta dimension is assumed to be the fastest varying.
 *
 * @param[out] data     Pointer to data structure to fill.
 * @param[in]  filename Data file name.
 * @param[in]  port     Port 1 or 2 (port number to load).
 * @param[in]  settings Pointer to settings structure used for surface fitting.
 */
OSKAR_EXPORT
int oskar_element_model_load_cst(oskar_ElementModel* data, int port,
        const char* filename, const oskar_SettingsElementFit* settings);

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_ELEMENT_MODEL_LOAD_CST_H_ */