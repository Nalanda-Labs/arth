/**
 *
 *  login.cc
 *  This file is generated by drogon_ctl
 *
 */

#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "login.h"
#include "util/jwt_impl.h"
#include "util/password.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Login::doLogin(const HttpRequestPtr &req, Callback callback) {
    auto json = req->getJsonObject();
    Json::Value ret;

    std::string username = json->get("username", "").asString();
    std::string password = json->get("password", "").asString();

    LOG_DEBUG << "username: " << username;
    LOG_DEBUG << "password: " << password;

    if (username == "" || password == "") {
        LOG_DEBUG << "Some fields are empty";        
        ret["error"] = "None of the fields can be empty";
        auto resp = jsonResponse(std::move(ret));
        callback(resp);
        return;
    }

    {
        auto customConfig = app().getCustomConfig();

        std::string jwtSecret = customConfig.get("jwt_secret", "").asString();

        if (jwtSecret == "") {
            LOG_DEBUG << "JWT not configured properly";
            ret["error"] = "JWT not configured properly";
            callback(jsonResponse(std::move(ret)));
            return;
        }

        auto clientPtr = drogon::app().getFastDbClient("default");

        clientPtr->execSqlAsync(
            "select id, username, password_hash from users where username = $1",

            [=](const Result &r) mutable {
                if (r.size() != 1) {
                    LOG_DEBUG << "User does not exist";
                    /// Prevents a class or error where attacker is trying to 
                    /// guess usernames for a given password
                    ret["error"] = "Wrong username or password";
                    callback(jsonResponse(std::move(ret)));
                    return;          
                }

                auto row = r[0];
                auto password_hash = row["password_hash"].as<std::string>();

                if (verifyPassword(password, password_hash)) {
                    auto user_id = row["id"].as<int>();
                    auto username = row["username"].as<std::string>();

                    ret["jwt"] = signJWT(user_id, username, jwtSecret);
                    callback(jsonResponse(std::move(ret)));
                    return;
                }

                /// Prevents a class or error where attacker is trying to
                /// guess usernames for a given password
                ret["error"] = "Wrong username or password";
                callback(jsonResponse(std::move(ret)));
            },
            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },
            username);
    }
}
