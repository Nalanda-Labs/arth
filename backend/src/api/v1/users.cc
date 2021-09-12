/**
 *
 *  login.cc
 *
 */

#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "users.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

auto Users::getUsers(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback) -> Task<>
{
    Json::Value ret;


    ret["users"] = Json::arrayValue;
    auto customConfig = app().getCustomConfig();
    int limit = customConfig.get("limit_per_page", 18).asInt64();
    auto json = req->getJsonObject();
    // get data from json to variables so that we do not need to operate on json
    Json::Value topic;
    std::string last_user = "";
    if (json->isMember("last_user"))
    {
        last_user = json->get("last_user", "").asString();
    }
    {
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {

            auto result = co_await clientPtr->execSqlCoro("select id, username, name, location, image_url from users where username > $1 order by reputation desc, username limit $2", last_user, (long)limit);
            if (result.size() == 0)
            {
                callback(jsonResponse(std::move(ret)));
            }
            else
            {
                for (auto &r : result)
                {
                    Json::Value user;

                    LOG_DEBUG << r["username"].as<std::string>();
                    user["id"] = r["id"].as<std::string>();
                    user["username"] = r["username"].as<std::string>();
                    user["name"] = r["name"].as<std::string>();
                    user["location"] = r["location"].as<std::string>();
                    user["image_url"] = r["image_url"].as<std::string>();

                    ret["users"].append(user);
                }
                
            }

            callback(jsonResponse(std::move(ret)));
        }
        catch (const DrogonDbException &e)
        {
            LOG_DEBUG << e.base().what();
            ret["error"] = (std::string)e.base().what();
            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
        }
    }

    co_return;
}
