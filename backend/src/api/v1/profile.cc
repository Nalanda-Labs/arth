#include <drogon/HttpAppFramework.h>
#include <trantor/utils/Logger.h>

#include "profile.h"
#include "util/arth.h"
#include "util/input_validation.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;


void profile::getProfile(const HttpRequestPtr &req, Callback callback, std::string &&username) {

    LOG_DEBUG << "username: " << username;

    Json::Value ret;

    if (!isUsernameValid(username)) {
        LOG_DEBUG << "Username not valid";
        ret["error"] = "Username not valid";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->execSqlAsync(        
            "select name, title, designation, location, email from users "
            "where username = $1", 

            [=] (const Result &r) mutable {
                if (r.size() != 1) {
                    ret["error"] = "User does not exist";
                    callback(jsonResponse(std::move(ret)));
                    return;
                }

                auto row = r[0];

                ret["username"] = username;
                ret["name"] = row["name"].as<std::string>();
                ret["title"] = row["title"].as<std::string>();
                ret["designation"] = row["designation"].as<std::string>();
                ret["location"] = row["location"].as<std::string>();

                auto customConfig = app().getCustomConfig();    
                const auto jwt_secret = customConfig.get("jwt_secret", "").asString();
                auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);    

                bool authenticated = optionalToken.has_value();

                if (authenticated) {
                    ret["email"] = row["email"].as<std::string>();                    
                }

                callback(jsonResponse(std::move(ret)));
            },

            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },

            username
        );
    }    
}