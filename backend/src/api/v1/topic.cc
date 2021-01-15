/**
 *
 *  topic.cc
 *
 */

#include <string>

#include <drogon/HttpClient.h>
#include <trantor/utils/Logger.h>

#include "util/arth.h"
#include "util/input_validation.h"
#include "util/jwt_impl.h"

#include "topic.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Topic::createTopic(const HttpRequestPtr &req, Callback callback)
{
    Json::Value ret;

    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);    

    if (!optionalToken.has_value()) {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    Token jwt = optionalToken.value();

    auto user_id = jwt.userID;
    LOG_DEBUG << user_id;

    auto json = req->getJsonObject();
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
                if (tag.length() > 32)
                {
                    LOG_DEBUG << "Too big tag: " + tag;
                    ret["error"] = "Max length for a tag is 32: " + tag;
                    callback(jsonResponse(std::move(ret)));
                    return;
                }
                for (int j = 0; i<tag.length(); j++) {
                    if(tag[i] >= 'A' && tag[i] <= 'Z') {
                        tag[i] += 32; // make it uppercase. do not apply transform of c++
                    }
                }
                LOG_DEBUG << tag;
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
                                    "insert into topics(title, description, posted_by, updated_by) values($1, $2, $3, $4) returning *",
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
                                        std::string slug;
                                        bool prev_dash = false;

                                        for (auto &c : title)
                                        {
                                            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
                                            {
                                                slug += c;
                                                prev_dash = false;
                                            }
                                            else if (c >= 'A' and c <= 'Z')
                                            {
                                                slug += c;
                                                prev_dash = false;
                                            }
                                            else if (c == ' ' || c == ',' || c == '.' || c == '/' || c == '\\' || c == '-' || c == '_' || c == '=')
                                            {
                                                if ((!prev_dash) && (slug.length() > 0))
                                                {
                                                    slug += '-';
                                                    prev_dash = true;
                                                }
                                            }
                                            else if (c > 160)
                                            {
                                                slug += c;
                                                prev_dash = false;
                                            }
                                        }
                                        if (prev_dash)
                                            slug = slug.substr(0, slug.size() - 1);

                                        LOG_DEBUG << slug;
                                        ret["slug"] = slug;
                                        ret["id"] = r2[0]["id"].as<Json::UInt64>();
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