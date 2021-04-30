/**
 *
 *  topic.cc
 *
 */

#include <string>
#include <vector>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "util/arth.h"
#include "util/jwt_impl.h"

#include "tags.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Tags::getTags(const HttpRequestPtr &req, Callback callback)
{
    Json::Value ret;

    auto json = req->getJsonObject();
    if (json == nullptr)
    {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
    }

    auto tag = json->get("tag", "").asString();

    if (tag.empty())
    {
        ret["tags"] = Json::arrayValue;
        callback(jsonResponse(std::move(ret)));
        return;
    }

    ret["tags"] = Json::arrayValue;

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transPtr) mutable {
            transPtr->execSqlAsync(
                "select name from tags where name like $1",
                [=](const Result &rows) mutable {
                    if (rows.size() == 0)
                    {
                        ret["tags"] = Json::arrayValue;
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }
                    else
                    {
                        ret["tags"] = Json::arrayValue;

                        for (auto &r : rows)
                        {
                            Json::Value tag;
                            tag["name"] = r["name"].as<std::string>();
                            ret["tags"].append(tag);
                            callback(jsonResponse(std::move(ret)));
                            return;
                        }
                    }
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                    return;
                },
                /// match any tag that starts with tag
                tag + "%");
        });
    }
}

auto Tags::getAllTags(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const std::string &page) -> Task<>
{
    Json::Value ret;

    ret["tags"] = Json::arrayValue;
    const long limit = 40;
    auto page_no = atol(page.c_str());

    {
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            auto result = co_await clientPtr->execSqlCoro("select count(1) over (), * from tags limit $1 offset $2", limit, limit * (page_no - 1));

            if (result.size() == 0)
            {
                callback(jsonResponse(std::move(ret)));
            }
            else
            {
                for (auto &r : result)
                {
                    Json::Value tag;

                    tag["id"] = r["id"].as<std::string>();
                    tag["name"] = r["name"].as<std::string>();
                    tag["topic_count"] = r["topic_count"].as<std::string>();
                    tag["info"] = r["info"].as<std::string>();
                    tag["count"] = r["count"].as<std::string>();

                    ret["tags"].append(tag);
                }
                callback(jsonResponse(std::move(ret)));    
            }
        }
        catch (const DrogonDbException &e)
        {
            // Exception works as sync interfaces.
            LOG_DEBUG << e.base().what();
            ret["error"] = (std::string)e.base().what();
            callback(jsonResponse(std::move(ret)));
        }

        co_return;
    }
}