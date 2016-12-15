/*
 * Copyright (c) 2014-2016, The University of Oxford
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

#include "oskar_OptionParser.h"
#include "utility/oskar_dir.h"
#include "utility/oskar_get_error_string.h"
#include "telescope/station/element/oskar_element.h"
#include "log/oskar_log.h"
#include "utility/oskar_version_string.h"

#include "oskar_settings_log.h"
#include "oskar_SettingsTree.hpp"
#include "oskar_SettingsDeclareXml.hpp"
#include "oskar_SettingsFileHandlerIni.hpp"

#include "apps/xml/oskar_fit_element_data_xml_all.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstdio>

using namespace oskar;
using std::vector;
using std::string;
using std::pair;

static string construct_element_pathname(string output_dir,
        int port, int element_type_index, double frequency_hz);

int main(int argc, char** argv)
{
    int status = 0;
    string output;
    vector<pair<string, string> > failed_keys;

    oskar_OptionParser opt("oskar_fit_element_data", oskar_version_string());
    opt.addRequired("settings file");
    opt.addFlag("-q", "Suppress printing.", false, "--quiet");
    if (!opt.check_options(argc, argv)) return OSKAR_ERR_INVALID_ARGUMENT;
    const char* settings_file = opt.getArg(0);

    // Create the log.
    int file_priority = OSKAR_LOG_MESSAGE;
    int term_priority = opt.isSet("-q") ? OSKAR_LOG_WARNING : OSKAR_LOG_STATUS;
    oskar_Log* log = oskar_log_create(file_priority, term_priority);
    oskar_log_message(log, 'M', 0, "Running binary %s", argv[0]);

    // Load the settings file.
    oskar_log_section(log, 'M', "Loading settings file '%s'", settings_file);
    SettingsTree s;
    settings_declare_xml(&s, oskar_fit_element_data_XML_STR);
    SettingsFileHandlerIni handler;
    s.set_file_handler(&handler);

    // Warn about settings failures.
    if (!s.load(failed_keys, settings_file))
    {
        oskar_log_error(log, "Failed to read settings file.");
        oskar_log_free(log);
        return OSKAR_ERR_FILE_IO;
    }
    for (size_t i = 0; i < failed_keys.size(); ++i)
        oskar_log_warning(log, "Ignoring '%s'='%s'",
                failed_keys[i].first.c_str(), failed_keys[i].second.c_str());

    // Log the relevant settings.
    oskar_log_set_keep_file(log, 0);
    oskar_settings_log(&s, log);

    // Get the main settings.
    s.begin_group("element_fit");
    string input_cst_file = s.to_string("input_cst_file", &status);
    string input_scalar_file = s.to_string("input_scalar_file", &status);
    string output_dir = s.to_string("output_directory", &status);
    string pol_type = s.to_string("pol_type", &status);
    // string coordinate_system = s.to_string("coordinate_system", &status);
    int element_type_index = s.to_int("element_type_index", &status);
    double frequency_hz = s.to_double("frequency_hz", &status);
    double average_fractional_error =
            s.to_double("average_fractional_error", &status);
    double average_fractional_error_factor_increase =
            s.to_double("average_fractional_error_factor_increase", &status);
    int ignore_below_horizon = s.to_int("ignore_data_below_horizon", &status);
    int ignore_at_pole = s.to_int("ignore_data_at_pole", &status);
    int port = pol_type == "X" ? 1 : pol_type == "Y" ? 2 : 0;

    // Check that the input and output files have been set.
    if ((input_cst_file.empty() && input_scalar_file.empty()) ||
            output_dir.empty())
    {
        oskar_log_error(log, "Specify input and output file names.");
        return OSKAR_ERR_FILE_IO;
    }

    // Create an element model.
    oskar_Element* element = oskar_element_create(OSKAR_DOUBLE,
            OSKAR_CPU, &status);

    // Load the CST text file for the correct port, if specified (X=1, Y=2).
    if (!input_cst_file.empty())
    {
        oskar_log_line(log, 'M', ' ');
        oskar_log_message(log, 'M', 0, "Loading CST element pattern: %s",
                input_cst_file.c_str());
        oskar_element_load_cst(element, log, port, frequency_hz,
                input_cst_file.c_str(), average_fractional_error,
                average_fractional_error_factor_increase,
                ignore_at_pole, ignore_below_horizon, &status);

        // Construct the output file name based on the settings.
        if (port == 0)
        {
            output = construct_element_pathname(output_dir, 1,
                    element_type_index, frequency_hz);
            oskar_element_write(element, log, output.c_str(), 1,
                    frequency_hz, &status);
            output = construct_element_pathname(output_dir, 2,
                    element_type_index, frequency_hz);
            oskar_element_write(element, log, output.c_str(), 2,
                    frequency_hz, &status);
        }
        else
        {
            output = construct_element_pathname(output_dir, port,
                    element_type_index, frequency_hz);
            oskar_element_write(element, log, output.c_str(), port,
                    frequency_hz, &status);
        }
    }

    // Load the scalar text file, if specified.
    if (!input_scalar_file.empty())
    {
        oskar_log_message(log, 'M', 0, "Loading scalar element pattern: %s",
                input_scalar_file.c_str());
        oskar_element_load_scalar(element, log, frequency_hz,
                input_scalar_file.c_str(), average_fractional_error,
                average_fractional_error_factor_increase,
                ignore_at_pole, ignore_below_horizon, &status);

        // Construct the output file name based on the settings.
        output = construct_element_pathname(output_dir, 0,
                element_type_index, frequency_hz);
        oskar_element_write(element, log, output.c_str(), 0,
                frequency_hz, &status);
    }

    // Check for errors.
    if (status)
    {
        oskar_log_error(log, "Run failed with code %i: %s.", status,
                oskar_get_error_string(status));
    }

    // Free memory.
    oskar_element_free(element, &status);
    oskar_log_free(log);

    return status;
}


static string construct_element_pathname(string output_dir,
        int port, int element_type_index, double frequency_hz)
{
    std::ostringstream stream;
    stream << "element_pattern_fit_";
    if (port == 0)
        stream << "scalar_";
    else if (port == 1)
        stream << "x_";
    else if (port == 2)
        stream << "y_";

    // Append the element type index.
    stream << element_type_index << "_";

    // Append the frequency in MHz.
    stream << std::fixed << std::setprecision(0)
    << std::setfill('0') << std::setw(3) << frequency_hz / 1.0e6;

    // Append the file extension.
    stream << ".bin";

    // Construct the full path.
    char* path = oskar_dir_get_path(output_dir.c_str(), stream.str().c_str());
    string p = string(path);
    free(path);
    return p;
}

