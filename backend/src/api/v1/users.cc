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

auto Users::getUsers(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const std::string &page = "1") -> Task<>
{
    Json::Value ret;


    ret["users"] = Json::arrayValue;
    auto customConfig = app().getCustomConfig();
    auto page_no = atol(page.c_str());
    int limit = customConfig.get("limit_per_page", 18).asInt64();
    LOG_DEBUG << limit;
    LOG_DEBUG << (page_no - 1) * limit;
    {
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {

            auto result = co_await clientPtr->execSqlCoro("select count(1) over (), id, username, name, location, image_url from users limit $1 offset $2", (long)limit, (long)limit * (page_no - 1));
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
                    user["count"] = r["count"].as<std::string>();

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
