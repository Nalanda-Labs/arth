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

    if (!optionalToken.has_value())
    {
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
            // get data from json to variables so that we do not need to operate on json
            auto topic = (*json)["topic"];
            auto title = topic.get("title", "").asString();
            auto body = topic.get("body", "").asString();

            std::vector<std::string> tagList;

            // transform each element in tagList to string
            std::transform(
                topic["tagList"].begin(), 
                topic["tagList"].end(), 
                std::back_inserter(tagList), 
                [](auto tag) {
                    return tag.asString();
                }
            );

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
            for (std::string &tag : tagList)
            {
                if (tag.length() > 32)
                {
                    LOG_DEBUG << "Too big tag: " + tag;
                    ret["error"] = "Max length for a tag is 32: " + tag;
                    callback(jsonResponse(std::move(ret)));
                    return;
                }
                for (int j = 0; j < tag.length(); j++)
                {
                    if (tag[j] >= 'A' && tag[j] <= 'Z')
                    {
                        tag[j] += 32; // make it uppercase. do not apply transform of c++
                    }
                }
                LOG_DEBUG << tag;
            }

            auto binder = *transPtr << "select * from tags where name in " + toPostgresParmeterizedSql(tagList.size());
            addVector(binder, tagList);
            binder 
            >> [=](const Result &r) mutable {
                    if (r.size() != tagList.size())
                    {
                        LOG_DEBUG << "One of the supplied tags do not exist.";
                        ret["error"] = "One of the supplied tags do not exist.";
                        callback(jsonResponse(std::move(ret)));
                    }
                    else
                    {
                        auto binder = *transPtr << "update tags set topic_count=topic_count + 1 where name in " + toPostgresParmeterizedSql(tagList.size());
                        addVector(binder, tagList);

                        binder
                        >> [=](const Result &r1) mutable {
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
                                            r2[0]["id"].as<size_t>(), tag_id);
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
                                    }
                                    if (prev_dash)
                                        slug = slug.substr(0, slug.size() - 1);

                                    LOG_DEBUG << slug;
                                    ret["slug"] = slug;
                                    ret["id"] = r2[0]["id"].as<std::string>();
                                    LOG_DEBUG << r2[0]["id"].as<std::string>();
                                    callback(jsonResponse(std::move(ret)));
                                    return;
                                },
                                [=](const DrogonDbException &e) mutable {
                                    LOG_DEBUG << e.base().what();
                                    ret["error"] = (std::string)e.base().what();
                                    callback(jsonResponse(std::move(ret)));
                                    return;
                                },
                                title, body, user_id, user_id
                            );
                        }
                    >> [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(jsonResponse(std::move(ret)));
                    };
                }
            }
            >> [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
            };            
        });
    }
}

void Topic::getTopic(const HttpRequestPtr &req, Callback callback, const size_t &tid, const std::string &slug)
{
    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        Json::Value ret;

        clientPtr->newTransactionAsync([=](const std::shared_ptr<Transaction> &transPtr) mutable {
            transPtr->execSqlAsync(
                "select t.title, t.description, t.visible, t.created_at, t.posted_by, t.updated_at, t.votes, users.username, users.image_url from topics t left join users on t.posted_by=users.id where t.id=$1",
                [=](const Result &r) mutable {
                    if (r.size() == 0)
                    {
                        ret["error"] = "No topics found.";
                        callback(jsonResponse(std::move(ret)));
                    }
                    else
                    {
                        ret["title"] = r[0]["title"].as<std::string>();
                        Json::Value topic;
                        topic["description"] = r[0]["description"].as<std::string>();
                        topic["visible"] = r[0]["visible"].as<bool>();                        
                        topic["posted_by"] = r[0]["posted_by"].as<std::string>();
                        topic["created_at"] = r[0]["created_at"].as<std::string>();
                        topic["updated_at"] = r[0]["updated_at"].as<std::string>();
                        topic["username"] = r[0]["username"].as<std::string>();
                        topic["votes"] = r[0]["votes"].as<std::string>();
                        topic["image_url"] = r[0]["image_url"].as<std::string>();
                        ret["topic"] = topic;

                        transPtr->execSqlAsync(
                            "select name from tags left join topic_tags on topic_tags.tag_id=tags.id where topic_tags.topic_id=$1",
                            [=](const Result &r1) mutable {
                                ret["tags"] = Json::arrayValue;

                                for (auto &row : r1)
                                {
                                    Json::Value tag;

                                    LOG_DEBUG << row["name"].as<std::string>();
                                    tag["name"] = row["name"].as<std::string>();
                                    ret["tags"].append(tag);
                                }
                                callback(jsonResponse(std::move(ret)));
                                return;
                            },
                            [=](const DrogonDbException &e) mutable {
                                LOG_DEBUG << e.base().what();
                                ret["error"] = (std::string)e.base().what();
                                callback(jsonResponse(std::move(ret)));
                                return;
                            },
                            tid
                        );
                    }
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                },
                tid
            );
        });
    }
}

void Topic::getDiscussion(const HttpRequestPtr &req, Callback callback, const size_t &tid,
                          const std::string &created_at, const size_t &limit)
{
    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        Json::Value ret;
        LOG_DEBUG << tid;
        LOG_DEBUG << created_at;

        // never use offset in cockroachdb it does not work well in terms of execution speed
        clientPtr->newTransactionAsync([=](const std::shared_ptr<Transaction> &transPtr) mutable {
            transPtr->execSqlAsync(
                "select count(*) over(), t.id, t.description, t.visible, t.created_at, t.posted_by, t.updated_at, t.votes, users.username, users.image_url from topics t left join users on t.posted_by=users.id  where t.op_id=$1  and t.created_at > $2 order by t.created_at asc limit $3",
                [=](const Result &rows) mutable {
                    ret["topics"] = Json::arrayValue;

                    LOG_DEBUG << rows.size();
                    for (auto &r : rows)
                    {
                        Json::Value topic;
                        topic["description"] = r["description"].as<std::string>();
                        topic["visible"] = r["visible"].as<bool>();
                        topic["posted_by"] = r["posted_by"].as<std::string>();
                        topic["created_at"] = r["created_at"].as<std::string>();
                        topic["updated_at"] = r["updated_at"].as<std::string>();
                        topic["topic_id"] = r["id"].as<std::string>();
                        topic["username"] = r["username"].as<std::string>();
                        topic["votes"] = r["votes"].as<std::string>();
                        topic["image_url"] = r["image_url"].as<std::string>();

                        ret["topics"].append(topic);
                    }

                    callback(jsonResponse(std::move(ret)));
                    return;
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                    return;
                },
                tid, created_at, limit);
        });
    }
}
