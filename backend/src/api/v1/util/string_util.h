#ifndef UTIL_STRING_UTIL_H
#define UTIL_STRING_UTIL_H

#include <string>
#include <algorithm>


inline bool contains(const std::string &str, const std::string &substr) {
    return str.find(substr) != std::string::npos;
}


/**
 * @brief Trims the string from left
 * 
 * @param s 
 * @return std::string& 
 */
inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    return s;
}


/**
 * @brief Trims the string from right
 * 
 * @param s 
 * @return std::string 
 */
inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}


/**
 * @brief Trims the string
 * 
 * @param s 
 * @return std::string 
 */
inline std::string trim(std::string s) {
    return rtrim(ltrim(s));    
}

#endif