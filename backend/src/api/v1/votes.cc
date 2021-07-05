/**
 *
 *  topic.cc
 *
 */

#include <drogon/orm/SqlBinder.h>
#include <iterator>
#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "util/arth.h"
#include "util/input_validation.h"
#include "util/jwt_impl.h"

#include "votes.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

auto Votes::votes(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback) -> Task<>
{
    Json::Value ret;

    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);

    if (jwt_secret == "")
    {
        ret["error"] = "JWT not configured";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }

    if (!optionalToken.has_value())
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }

    Token jwt = optionalToken.value();

    auto user_id = jwt.userID;
    LOG_DEBUG << user_id;

    auto json = req->getJsonObject();
    Json::Value topic;
    if (json->isMember("data"))
    {
        topic = (*json)["data"];
    }
    else
    {
        ret["error"] = "Malformed request.";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }
    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        auto transPtr = co_await clientPtr->newTransactionCoro();
        // get data from json to variables so that we do not need to operate on json
        auto vote = topic.get("vote", 0).asInt64();
        auto topic_id = topic.get("id", 0).asString();

        try {
            auto r = co_await transPtr->execSqlCoro("select * from votes where topic_id=$1 and user_id=$2", topic_id, user_id);
            if(r.size() == 0) {
                co_await transPtr->execSqlCoro("insert into votes(topic_id, user_id, vote) values($1, $2, $3)", topic_id, user_id, vote);
                co_await transPtr->execSqlCoro("update topics set votes = votes + $1 where id= $2", vote, topic_id);
                ret["success"] = true;
                callback(jsonResponse(std::move(ret)));
                co_return;
            } else {
                auto vote_casted = r[0]["vote"].as<int64_t>();
                if(vote == vote_casted) {
                    ret["error"] = "You can cast upvote/downvote only once.";
                    callback(jsonResponse(std::move(ret)));
                    co_return;
                } else {
                    co_await transPtr->execSqlCoro("update votes set vote = vote + $1 where topic_id=$2 and user_id=$3", vote, topic_id, user_id);
                    co_await transPtr->execSqlCoro("update topics set votes = votes + $1 where id= $2", vote, topic_id);
                    ret["success"] = true;
                    callback(jsonResponse(std::move(ret)));
                    co_return;
                }
            }
        } catch (const DrogonDbException &e) {
            ret["error"] = (std::string)e.base().what();
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
    }
    co_return;
}