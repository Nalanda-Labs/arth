/**
 *
 *  topic.cc
 *
 */

#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "util/arth.h"
#include "util/jwt_impl.h"

#include "topic.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Topic::createTopic(const HttpRequestPtr &req, Callback callback)
{
    std::string authHeader;
    Json::Value ret;

    try
    {
        authHeader = req->getHeader("Authorization");
        LOG_DEBUG << authHeader;
    }
    catch (std::exception &e)
    {
        LOG_DEBUG << e.what();

        ret["error"] = "Invalid JWT.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    std::string delimiter = " ";
    std::string bearer = authHeader.substr(0, authHeader.find(delimiter));
    std::string JWT = "";

    if (bearer == "Bearer")
    {
        JWT = authHeader.erase(0, bearer.length() + delimiter.length());
    }
    else
    {
        LOG_DEBUG << bearer;

        ret["error"] = "Invalid JWT.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto json = req->getJsonObject();
    if (json == nullptr)
    {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
    }

    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    auto decoded_jwt = verifyJWT(JWT, jwt_secret).value_or(Token{0, ""});
    auto user_id = decoded_jwt.userID;

    if (decoded_jwt.username == "" || user_id == 0)
    {
        ret["error"] = "Invalid JWT.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    LOG_DEBUG << decoded_jwt.userID;

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transPtr) mutable {
            assert(transPtr);
            auto customConfig = app().getCustomConfig();
            std::string tags = "";

            // get data from json to variables so that we do not need to operate on json
            auto topic = (*json)["topic"];
            auto title = topic.get("title", "").asString();
            auto body = topic.get("body", "").asString();
            auto tagList = topic["tagList"];

            if (title.length() < 10 || title.length() > 256)
            {
                LOG_DEBUG << "Bad title length " + std::to_string(title.length());
                ret["error"] = "Bad title length " + std::to_string(title.length());
                callback(jsonResponse(std::move(ret)));
                return;
            }
            if (body.length() < 20 || body.length() > 1000000)
            {
                LOG_DEBUG << "Bad body length " + std::to_string(body.length());
                ret["error"] = "Bad body length " + std::to_string(body.length());
                callback(jsonResponse(std::move(ret)));
                return;
            }
            if (tagList.size() < 1)
            {
                LOG_DEBUG << "At least one tag must be provided";
                ret["error"] = "At least one tag must be provided";
                callback(jsonResponse(std::move(ret)));
                return;
            }
            for (int i = 0; i < tagList.size(); i++)
            {
                auto tag = tagList[i].asString();
                LOG_DEBUG << tag;
                if (tag.length() > 32)
                {
                    LOG_DEBUG << "Too big tag: " + tag;
                    ret["error"] = "Max length for a tag is 32: " + tag;
                    callback(jsonResponse(std::move(ret)));
                    return;
                }
                tags += "'" + tag + "',"; // need to make it compatible for array syntax for SQL
            }

            tags = tags.substr(0, tags.size() - 1); // remove last comma
            LOG_DEBUG << tags;

            transPtr->execSqlAsync(
                "select * from tags where name in (" + tags + ")",
                [=](const Result &r) mutable {
                    if (r.size() != tagList.size())
                    {
                        LOG_DEBUG << "One of the supplied tags do not exist.";
                        ret["error"] = "One of the supplied tags do not exist.";
                        callback(jsonResponse(std::move(ret)));
                    }
                    else
                    {
                        transPtr->execSqlAsync(
                            "update tags set topic_count=topic_count + 1 where name in (" + tags + ")",
                            [=](const Result &r1) mutable {
                                transPtr->execSqlAsync(
                                    "insert into topics(title, description, posted_by, updated_by) values($1, $2, $3, $4) returning id",
                                    [=](const Result &r2) mutable {
                                        for (auto &row : r)
                                        {
                                            auto tag_id = row["id"].as<size_t>();

                                            transPtr->execSqlAsync(
                                                "insert into topic_tags(topic_id, tag_id) values($1, $2)",
                                                [=](const Result &r3) {
                                                },
                                                [=](const DrogonDbException &e) mutable {
                                                    LOG_DEBUG << e.base().what();
                                                    ret["error"] = (std::string)e.base().what();
                                                    callback(jsonResponse(std::move(ret)));
                                                    return;
                                                },
                                                tag_id, r2[0]["id"].as<size_t>());
                                        }

                                        LOG_DEBUG << "Topic added";
                                        ret["message"] = "Topic added";
                                        ret["topic_id"] = r2[0]["id"].as<size_t>();
                                        callback(jsonResponse(std::move(ret)));
                                        return;
                                    },
                                    [=](const DrogonDbException &e) mutable {
                                        LOG_DEBUG << e.base().what();
                                        ret["error"] = (std::string)e.base().what();
                                        callback(jsonResponse(std::move(ret)));
                                        return;
                                    },
                                    title, body, user_id, user_id);
                            },
                            [=](const DrogonDbException &e) mutable {
                                LOG_DEBUG << e.base().what();
                                ret["error"] = (std::string)e.base().what();
                                callback(jsonResponse(std::move(ret)));
                            });
                    }
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                });
        });
    }
}