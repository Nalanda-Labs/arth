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
std::optional<Token> verifiedToken(const std::string &authHeader, const std::string &secret);

/**
 * @brief Returns a parameterized query containing placeholderCount parameters
 * Causes assertion error if placeholderCount is less than 1
 *
 * @param placeholderCount the number of placeholders
 * @param bindingStartCount the number to start parameter count from
 * @return std::string ($1, $2, $3, ...$n)
 */
std::string toPostgresParameterizedSql(size_t placeholderCount,
                                       size_t bindingStartCount = 1);

/**
 * @brief Add vector to binder. Please make sure that the order of 
 * arguments before and after the call match
 * 
 * @tparam T 
 * @param binder 
 * @param vec 
 */
template <typename T>
inline void addVector(drogon::orm::internal::SqlBinder &binder, std::vector<T> vec)
{
    for (auto &i : vec)
    {
        binder << i;
    }
}

inline void join(const std::vector<std::string> &v, char c, std::string &s)
{
    s.clear();

    for (std::vector<std::string>::const_iterator p = v.begin();
         p != v.end(); ++p)
    {
        s += '\'';
        s += *p;
        s += '\'';
        if (p != v.end() - 1) {
            s += c;
        }
    }
}

#endif