/**
 *
 *  register.cc
 *  This file is generated by drogon_ctl
 *
 */

#include "register.h"
#include <string>

using namespace api::v1;

void registration::doRegister(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    Json::Value ret;

    LOG_DEBUG << "name: " << json->get("name", "").asString();
    LOG_DEBUG << "email: " << json->get("email", "").asString();
    LOG_DEBUG << "username: " << json->get("username", "").asString();
    LOG_DEBUG << "password: " << json->get("password", "").asString();

    // if (json)
    // {
    //     auto clientPtr = drogon::app().getFastDbClient("arth");
    //     auto transPtr = clientPtr->newTransaction();
    //     transPtr->execSqlAsync(
    //         "select * from users where =$1",
    //         [](const Result &r) {
    //             if (r.size() > 0)
    //             {
    //                 ret["errors"] = "User exists";
    //             } else {
    //                 *transPtr = "insert into users (useranme)"
    //             }
    //         },
    //         [](const DrogonDbException &e) {
    //             std::cerr << "error:" << e.base().what() << std::endl;
    //         },
    //         "default");
    // }

    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}
