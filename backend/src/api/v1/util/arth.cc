#include "arth.h"

/**
 * @brief Verifies the token in authHeader. If verification is successful, 
 * return token. Else return empty optional
 * 
 * @param authHeader the value of "Authorization" header 
 * @param secret The secret used to verify JWT 
 * @return std::optional<Token> 
 */
std::optional<Token> verifiedToken(const std::string &authHeader, const std::string &secret) {
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

/**
 * @brief Returns a parameterized query containing placeholderCount parameters
 * Causes assertion error if placeholderCount is less than 1
 *
 * @param placeholderCount the number of placeholders
 * @param bindingStartCount the number to start parameter count from
 * @return std::string ($1, $2, $3, ...$n)
 */
std::string toPostgresParameterizedSql(size_t placeholderCount,
                                      size_t bindingStartCount) {
    assert(placeholderCount > 0);

    std::stringstream sql;
    sql << "(";

    /// needed to add the last element without comma
    size_t i = 1;

    /// we need the parameter to start from one
    /// we are not iterating the last time because that
    for (; i < placeholderCount; i++) {
        sql << '$' << i << ',';
    };

    sql << '$' << i << ')';
    return sql.str();
}
