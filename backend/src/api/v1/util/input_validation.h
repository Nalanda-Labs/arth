#ifndef UTIL_INPUT_VALIDATION_H
#define UTIL_INPUT_VALIDATION_H

#include <algorithm>
#include <string>

#include "string_util.h"
#include "emailutils.h"


inline bool isUsernameValid(const std::string &username) {    
    return !username.empty() && !contains(username, " ") && username.length() < 60;
}


inline bool isEmailValid(const std::string &email) {    
    auto position = email.find('@');

    /// every email has @. If not found, email is not valid
    if (position == std::string::npos) {
        return false;
    }

    /// every email has . after @. If not found, return false
    if (email.find('.', position) == std::string::npos) {
        return false;
    }

    return !email.empty() && !contains(email, " ") && email.length() < 256;
}


inline bool isPasswordValid(const std::string &password) {
    return password.length() >= 16 && password.length() < 256;
}

#endif
