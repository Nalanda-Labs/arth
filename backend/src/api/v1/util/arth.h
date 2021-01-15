#ifndef UTIL_DROGON_H
#define UTIL_DROGON_H

#include <cstddef>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include <drogon/drogon_callbacks.h>
#include <drogon/HttpController.h>
#include <stdexcept>
#include <trantor/utils/Logger.h>

#include "jwt_impl.h"
#include "string_util.h"


using TransactionPtr = const std::shared_ptr<drogon::orm::Transaction> &;
using Callback = std::function<void(const drogon::HttpResponsePtr &)> &&;

inline drogon::HttpResponsePtr jsonResponse(Json::Value &&data)
{
    return drogon::HttpResponse::newHttpJsonResponse(data);
}


/**
 * @brief Verifies the token in authHeader. If verification is successful, 
 * return token. Else return empty optional
 * 
 * @param authHeader the value of "Authorization" header 
 * @param secret The secret used to verify JWT 
 * @return std::optional<Token> 
 */
inline std::optional<Token> verifiedToken(const std::string &authHeader, const std::string &secret) {
    Json::Value ret;
    if (authHeader.empty()) {
        return {};
    }

    const std::string delimiter = " ";
    
    // ensure space exist. If space doesn't exist, return error. 
    size_t delimiterPos = authHeader.find(delimiter);    
    if (delimiterPos == std::string::npos) {        
        return {};
    }

    /// ensure there is at least one character after auth method 
    /// so that substring doesn't throw
    if (authHeader.size() == delimiterPos) {        
        return {};
    }

    const std::string authMethod = authHeader.substr(0, delimiterPos);

    LOG_DEBUG << "auth method: " << authMethod;
    std::string jwt = "";

    /// Add an if condition for each authentication method 
    /// Call hanlder if verified. Otherwise return error using
    /// callback and don't call hanlder.

    if (authMethod == "Bearer") {
        jwt = authHeader.substr(authMethod.length() + delimiter.length());
        
        auto optionalToken = verifyJWT(jwt, secret);        
        return optionalToken;
    }

    return {};
}

#endif