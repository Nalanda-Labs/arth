/**
 *
 *  login.cc
 *  This file is generated by drogon_ctl
 *
 */

#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "index.h"
#include "util/arth.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

auto Index::index(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback) -> Task<>
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
            
            auto result = co_await clientPtr->execSqlCoro("select t.id, t.visible1, t.title, t.created_at , t.posted_by, t.updated_at, t.votes, t.views, t.slug, \
                                                          users.username, users.id as uid, array_agg(topic_tags.tag_id) as tag_id, array_agg(tags.name) as tags, t.answer_count from topics t left \
                                                          join users on t.posted_by=users.id left join topic_tags on topic_tags.topic_id=t.id left join \
                                                          tags on topic_tags.tag_id = tags.id where t.op_id=0 and t.updated_at < $1 group by t.id, users.id order by \
                                                          t.updated_at desc limit $2",
                                                          updated_at, (long)limit);
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