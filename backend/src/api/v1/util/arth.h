#ifndef UTIL_DROGON_H
#define UTIL_DROGON_H

#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include <drogon/drogon_callbacks.h>

using TransactionPtr = const std::shared_ptr<drogon::orm::Transaction> &;
using Callback = std::function<void(const drogon::HttpResponsePtr &)> &&;

inline drogon::HttpResponsePtr jsonResponse(Json::Value &&data) {
    return drogon::HttpResponse::newHttpJsonResponse(data);
}

#endif