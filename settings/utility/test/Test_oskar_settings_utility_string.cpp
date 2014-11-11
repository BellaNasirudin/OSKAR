/*
 * Copyright (c) 2014, The University of Oxford
 * All rights reserved.
 *
 * This file is part of the OSKAR package.
 * Contact: oskar at oerc.ox.ac.uk
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

#include <iostream>
#include <climits>
#include <oskar_settings_utility_string.hpp>

using namespace std;

TEST(oskar_settings_utility, string_trim)
{
    string s;
    s = "   hello there\t  ";
    ASSERT_STREQ("hello there", oskar_settings_utility_string_trim(s).c_str());
}


TEST(oskar_settings_utility, string_get_type_params)
{
    string s;
    vector<string> p;
    {
        s = "2,10";
        p = oskar_settings_utility_string_get_type_params(s);
        ASSERT_EQ(2u, p.size());
        ASSERT_STREQ("2", p[0].c_str());
        ASSERT_STREQ("10", p[1].c_str());
    }
    {
        s = "2,\"10,20\",3";
        p = oskar_settings_utility_string_get_type_params(s);
        ASSERT_EQ(3u, p.size());
        ASSERT_STREQ("2",     p[0].c_str());
        ASSERT_STREQ("10,20", p[1].c_str());
        ASSERT_STREQ("3",     p[2].c_str());
    }
}


TEST(oskar_settings_utility, string_to_int)
{
    string s;
    bool ok = false;
    {
        s = "123456789";
        ASSERT_EQ(123456789, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_TRUE(ok);
    }
    {
        s = "";
        ASSERT_EQ(0, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_FALSE(ok);
    }
    {
        s = "-123456789";
        ASSERT_EQ(-123456789, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_TRUE(ok);
    }
    {
        s = "2147483647";
        ASSERT_EQ(INT_MAX, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_TRUE(ok);
    }
    {
        s = "2147483648";
        ASSERT_EQ(0, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_FALSE(ok);
    }

    {
        s = "-2147483647";
        ASSERT_EQ(-INT_MAX, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_TRUE(ok);
    }
    {
        s = "-2147483648";
        ASSERT_EQ(0, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_FALSE(ok);
    }
    {
        s = "   1  ";
        ASSERT_EQ(1, oskar_settings_utility_string_to_int(s, &ok));
        // Note this fails due to checking for trailing characters after the int
        ASSERT_FALSE(ok);
    }
    {
        s = "hello";
        ASSERT_EQ(0, oskar_settings_utility_string_to_int(s, &ok));
        ASSERT_FALSE(ok);
    }
}

TEST(oskar_settings_utility, int_to_string)
{
    int i;
    {
        i = 23;
        ASSERT_STREQ("23", oskar_settings_utility_int_to_string(i).c_str());
    }
    {
        i = -123456790;
        ASSERT_STREQ("-123456790", oskar_settings_utility_int_to_string(i).c_str());
    }
}
