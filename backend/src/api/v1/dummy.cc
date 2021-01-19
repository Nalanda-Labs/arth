#include "dummy.h"
#include <cassert>
#include <cstddef>
#include <drogon/HttpAppFramework.h>
#include <json/value.h>
#include <sstream>
#include <string>

/**
 * @brief Returns a parameterized query containing placeholderCount parameters
 * Causes assertion error if placeholderCount is less than 1
 *
 * @param placeholderCount the number of placeholders
 * @param bindingStartCount the number to start parameter count from
 * @return std::string ($1, $2, $3, ...$n)
 */
std::string toPostgresParmeterizedSql(size_t placeholderCount,
                                      size_t bindingStartCount = 1) {
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

template <typename T>
void addVector(drogon::orm::internal::SqlBinder &binder, std::vector<T> vec) {
    for (auto &i : vec) {
        binder << i;
    }
}

void dummy::get(const HttpRequestPtr &req, Callback callback) {
    auto clientPtr = app().getFastDbClient();

    std::vector<std::string> vec = {"abc", "def", "ghi"};

    auto binder = *clientPtr << "select id from tags where name in " +
                                toPostgresParmeterizedSql(vec.size());
    addVector(binder, vec);

    binder 
    >> [=](const drogon::orm::Result &r) {
        Json::Value ret = Json::arrayValue;
        for (auto &i : r) {
        ret.append(i["id"].as<size_t>());
        }
        callback(jsonResponse(std::move(ret)));
        return;
    } 
    >> [=](const drogon::orm::DrogonDbException &e) {
        Json::Value ret;
        LOG_DEBUG << e.base().what();
        ret["error"] = (std::string)e.base().what();
        callback(jsonResponse(std::move(ret)));
        return;
    };
}
