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

auto Tags::getAllTags(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback) -> Task<>
{
    Json::Value ret;

    ret["tags"] = Json::arrayValue;
    const long limit = 40;
    auto json = req->getJsonObject();
    // get data from json to variables so that we do not need to operate on json
    Json::Value topic;
    std::string last_tag = "";
    if (json->isMember("last_tag"))
    {
        last_tag = json->get("last_tags", "").asString();
    }
    auto topic_count = 0L;
    if (json->isMember("topic_count"))
    {
        topic_count = json->get("topic_count", 0).asInt64();
    }
    if(topic_count == 0) {
        topic_count = INT64_MAX;
    }
    LOG_DEBUG << last_tag;
    LOG_DEBUG << topic_count;
    {
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            auto result = co_await clientPtr->execSqlCoro("select * from tags where name > $1 and topic_count< $2 order by topic_count desc, name limit $3", last_tag, topic_count, limit);
            LOG_DEBUG << result.size();
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