/**
 *
 *  topic.cc
 *
 */

#include <string>
#include <vector>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "index.h"
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
        last_tag = json->get("last_tag", "").asString();
    }
    std::string topic_count1 = "";
    if (json->isMember("topic_count"))
    {
        topic_count1 = json->get("topic_count", "0").asString();
    }
    long topic_count = 0L;
    if(topic_count1 == "0") {
        topic_count = INT64_MAX;
    }
    LOG_DEBUG << last_tag;
    LOG_DEBUG << topic_count;
    {
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            auto result = co_await clientPtr->execSqlCoro("select * from tags where name > $1 and topic_count< $2 order by topic_count desc, name limit $3", last_tag, topic_count, limit);
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

auto Tags::getTopicsByTag(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const std::string& tag) -> Task<>
{
    Json::Value ret;

    auto customConfig = app().getCustomConfig();
    int limit = customConfig.get("limit_per_page", 1).asInt64();
    LOG_DEBUG << limit;
    limit =10;

    auto json = req->getJsonObject();
    // get data from json to variables so that we do not need to operate on json
    Json::Value topic;
    std::string updated_at = "";
    if (json->isMember("updated_at"))
    {
        updated_at = json->get("updated_at", "").asString();
    }
    if(updated_at == "") {
        // if we do not get updated_at in request then use value of now for it
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);
        std::ostringstream ss;
        ss << std::put_time(gmtime(&itt), "%FT%TZ");
        updated_at = ss.str();
    }

    {
        ret["topics"] = Json::arrayValue;
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            // we do keyset pagination for infinite scrolling
            
            auto result = co_await clientPtr->execSqlCoro("select t.id, t.visible1, t.title, t.created_at , t.posted_by, t.updated_at, t.votes, "
                                                          "t.views, t.slug, users.username, users.id as uid, array_agg(topic_tags.tag_id) as "
                                                          "tag_id, array_agg(tags.name) as tags, t.answer_count from topics t left join users "
                                                          "on t.posted_by=users.id left join topic_tags on topic_tags.topic_id=t.id  left join "
                                                          "tags on topic_tags.tag_id = tags.id where t.id in(select topic_id from topic_tags "
                                                          "left join tags on tags.id=topic_tags.tag_id where name=$1) and t.updated_at < $2 group by "
                                                          "t.id, users.id order by updated_at desc limit $3",
                                                          tag, updated_at, (long)limit);
            if (result.size() == 0)
            {
                callback(jsonResponse(std::move(ret)));
            }
            else
            {
                for (auto &r : result)
                {
                    Json::Value topic;

                    topic["id"] = r["id"].as<std::string>();
                    topic["visible"] = r["visible1"].as<bool>();
                    topic["title"] = r["title"].as<std::string>();
                    topic["created_at"] = r["created_at"].as<std::string>();
                    topic["updated_at"] = r["updated_at"].as<std::string>();
                    topic["votes"] = r["votes"].as<std::string>();
                    topic["views"] = r["views"].as<std::string>();
                    topic["slug"] = r["slug"].as<std::string>();
                    topic["username"] = r["username"].as<std::string>();
                    topic["uid"] = r["uid"].as<std::string>();
                    topic["tid"] = r["tag_id"].as<std::string>();
                    topic["tags"] = r["tags"].as<std::string>();
                    topic["answers"] = r["answer_count"].as<std::string>();

                    ret["topics"].append(topic);
                }
            }

            callback(jsonResponse(std::move(ret)));
        }
        catch (const DrogonDbException &err)
        {
            // Exception works as sync interfaces.
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(err.base().what());
            callback(resp);
        }
    }

    co_return;
}
