/*
 * SPDX-FileCopyrightText: 2025 Mao-Pao-Tong Workshop
 * SPDX-License-Identifier: MPL-2.0
 */
#pragma
#include <string>
namespace fog
{
    struct StringUtil
    {
        static bool starts_with(const std::string &str, const std::string &prefix)
        {
            return str.length() >= prefix.length() &&
                   str.compare(0, prefix.length(), prefix) == 0;
        }
        static bool ends_with(const std::string &str, const std::string &suffix)
        {
            return str.length() >= suffix.length() &&
                   str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
        }
        static std::vector<std::string> split(const std::string &str, const char delimiter)
        {
            std::vector<std::string> ret;
            size_t start = 0, end = 0;

            while ((end = str.find(delimiter, start)) != std::string::npos)
            {
                ret.push_back(str.substr(start, end - start));
                start = end + 1;
            }
            ret.push_back(str.substr(start)); // 剩余部分

            return ret;
        }
    };
};
