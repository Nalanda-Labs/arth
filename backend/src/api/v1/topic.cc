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

    if (jwt_secret == "")
    {
        ret["error"] = "JWT not configured";
        callback(jsonResponse(std::move(ret)));
        return;
    }

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
    // get data from json to variables so that we do not need to operate on json
    Json::Value topic;
    if (json->isMember("topic"))
    {
        topic = (*json)["topic"];
    }
    else
    {
        ret["error"] = "Malformed request.";
        callback(jsonResponse(std::move(ret)));
        return;
    }
    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync(
            [=](TransactionPtr transPtr) mutable
            {
                assert(transPtr);
                auto customConfig = app().getCustomConfig();
                auto title = topic.get("title", "").asString();
                auto body = topic.get("body", "").asString();

                std::vector<std::string> tagList;

                // transform each element in tagList to string
                std::transform(
                    topic["tagList"].begin(),
                    topic["tagList"].end(),
                    std::back_inserter(tagList),
                    [](auto tag)
                    {
                        return tag.asString();
                    });

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
                            tag[j] += 32; // make it lowercasecase. do not apply transform of c++
                        }
                    }
                    LOG_DEBUG << tag;
                }

                auto binder = *transPtr << "select * from tags where name in " + toPostgresParameterizedSql(tagList.size());
                addVector(binder, tagList);
                binder >> [=](const Result &r) mutable
                {
                    if (r.size() != tagList.size())
                    {
                        LOG_DEBUG << "One of the supplied tags do not exist.";
                        ret["error"] = "One of the supplied tags do not exist.";
                        callback(jsonResponse(std::move(ret)));
                    }
                    else
                    {
                        auto binder = *transPtr << "update tags set topic_count=topic_count + 1 where name in " + toPostgresParameterizedSql(tagList.size());
                        addVector(binder, tagList);

                        binder >> [=](const Result &r1) mutable
                        {
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

                            transPtr->execSqlAsync(
                                "insert into topics(title, description, posted_by, updated_by, slug) values($1, $2, $3, $4, $5) returning *",
                                [=](const Result &r2) mutable
                                {
                                    for (auto &row : r)
                                    {
                                        auto tag_id = row["id"].as<size_t>();

                                        transPtr->execSqlAsync(
                                            "insert into topic_tags(topic_id, tag_id) values($1, $2)",
                                            [=](const Result &r3) {
                                            },
                                            [=](const DrogonDbException &e) mutable
                                            {
                                                LOG_DEBUG << e.base().what();
                                                ret["error"] = (std::string)e.base().what();
                                                callback(jsonResponse(std::move(ret)));
                                                return;
                                            },
                                            r2[0]["id"].as<size_t>(), tag_id);
                                    }

                                    LOG_DEBUG << "Topic added";

                                    LOG_DEBUG << slug;
                                    ret["slug"] = slug;
                                    ret["id"] = r2[0]["id"].as<std::string>();
                                    LOG_DEBUG << r2[0]["id"].as<std::string>();
                                    callback(jsonResponse(std::move(ret)));
                                    return;
                                },
                                [=](const DrogonDbException &e) mutable
                                {
                                    LOG_DEBUG << e.base().what();
                                    ret["error"] = (std::string)e.base().what();
                                    callback(jsonResponse(std::move(ret)));
                                    return;
                                },
                                title, body, user_id, user_id, slug);
                        } >> [=](const DrogonDbException &e) mutable
                        {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(jsonResponse(std::move(ret)));
                        };
                    }
                } >> [=](const DrogonDbException &e) mutable
                {
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

        clientPtr->newTransactionAsync(
            [=](const std::shared_ptr<Transaction> &transPtr) mutable
            {
                transPtr->execSqlAsync(
                    "select t.title, t.description, t.visible1, t.created_at, t.posted_by, t.updated_at, "
                    "t.votes, users.username, users.image_url from topics t left join users on "
                    "t.posted_by=users.id where t.id=$1",
                    [=](const Result &r) mutable
                    {
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
                            topic["visible"] = r[0]["visible1"].as<bool>();
                            topic["posted_by"] = r[0]["posted_by"].as<std::string>();
                            topic["created_at"] = r[0]["created_at"].as<std::string>();
                            topic["updated_at"] = r[0]["updated_at"].as<std::string>();
                            topic["username"] = r[0]["username"].as<std::string>();
                            topic["votes"] = r[0]["votes"].as<std::string>();
                            topic["image_url"] = r[0]["image_url"].as<std::string>();
                            ret["topic"] = topic;

                            transPtr->execSqlAsync(
                                "select name from tags left join topic_tags on topic_tags.tag_id=tags.id where topic_tags.topic_id=$1",
                                [=](const Result &r1) mutable
                                {
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
                                [=](const DrogonDbException &e) mutable
                                {
                                    LOG_DEBUG << e.base().what();
                                    ret["error"] = (std::string)e.base().what();
                                    callback(jsonResponse(std::move(ret)));
                                    return;
                                },
                                tid);
                        }
                    },
                    [=](const DrogonDbException &e) mutable
                    {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(jsonResponse(std::move(ret)));
                    },
                    tid);
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
        clientPtr->newTransactionAsync(
            [=](const std::shared_ptr<Transaction> &transPtr) mutable
            {
                transPtr->execSqlAsync(
                    "select count(1) over(), t.id, t.description, t.visible1, t.created_at, t.posted_by, t.updated_at,"
                    "t.votes, t.answer_accepted, users.username, users.image_url from topics t left join users on t.posted_by=users.id  "
                    "where t.op_id=$1  and t.created_at > $2 order by t.created_at asc limit $3",
                    [=](const Result &rows) mutable
                    {
                        ret["topics"] = Json::arrayValue;

                        LOG_DEBUG << rows.size();
                        for (auto &r : rows)
                        {
                            Json::Value topic;
                            topic["description"] = r["description"].as<std::string>();
                            topic["visible"] = r["visible1"].as<bool>();
                            topic["posted_by"] = r["posted_by"].as<std::string>();
                            topic["created_at"] = r["created_at"].as<std::string>();
                            topic["updated_at"] = r["updated_at"].as<std::string>();
                            topic["topic_id"] = r["id"].as<std::string>();
                            topic["username"] = r["username"].as<std::string>();
                            topic["votes"] = r["votes"].as<std::string>();
                            topic["image_url"] = r["image_url"].as<std::string>();
                            topic["answer_accepted"] = r["answer_accepted"].as<bool>();

                            ret["topics"].append(topic);
                        }

                        callback(jsonResponse(std::move(ret)));
                        return;
                    },
                    [=](const DrogonDbException &e) mutable
                    {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(jsonResponse(std::move(ret)));
                        return;
                    },
                    tid, created_at, limit);
            });
    }
}

auto Topic::editTopic(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const std::string &tid) -> Task<>
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
    if (json->isMember("topic"))
    {
        topic = (*json)["topic"];
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
        auto title = topic.get("title", "").asString();
        auto body = topic.get("body", "").asString();

        std::vector<std::string> tagList;

        // transform each element in tagList to string
        std::transform(
            topic["tagList"].begin(),
            topic["tagList"].end(),
            std::back_inserter(tagList),
            [](auto tag)
            {
                return tag.asString();
            });
        // we do not check for title as title may be empty
        if (title != "" && ((title.length() < 10 || title.length() > 256)))
        {
            LOG_DEBUG << "Bad title length " + std::to_string(title.length());
            ret["error"] = "Bad title length " + std::to_string(title.length());
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        if (body.length() < 20 || body.length() > 1000000)
        {
            LOG_DEBUG << "Bad body length " + std::to_string(body.length());
            ret["error"] = "Bad body length " + std::to_string(body.length());
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        // we do not check for taglist as it may be emty
        if (tagList.size())
        {
            // this means topic being edited has tags in them
            for (std::string &tag : tagList)
            {
                if (tag.length() > 32)
                {
                    LOG_DEBUG << "Too big tag: " + tag;
                    ret["error"] = "Max length for a tag is 32: " + tag;
                    callback(jsonResponse(std::move(ret)));
                    co_return;
                }
                for (int j = 0; j < tag.length(); j++)
                {
                    if (tag[j] >= 'A' && tag[j] <= 'Z')
                    {
                        tag[j] += 32; // make it lowercase. do not apply transform of c++
                    }
                }
                LOG_DEBUG << tag;
            }
            // TODO: fix this with internal::orm::sql_binder so that sql injection does not happen
            std::stringstream binder;
            std::string s;

            join(tagList, ',', s);
            binder << "select * from tags where name in (" << s << ")";
            try
            {
                auto r = co_await transPtr->execSqlCoro(binder.str());
                auto r1 = co_await transPtr->execSqlCoro("select * from tags where id in (select tag_id from topic_tags where topic_id=$1)", tid);
                if (r.size() != tagList.size())
                {
                    LOG_DEBUG << "One of the supplied tags do not exist.";
                    ret["error"] = "One of the supplied tags do not exist.";
                    callback(jsonResponse(std::move(ret)));
                }
                else
                {
                    // decrease the count for existing tags and delete topic references
                    for (auto &row : r1)
                    {
                        co_await transPtr->execSqlCoro("update tags set topic_count=topic_count - 1 where name=$1", row["name"].as<std::string>());
                        co_await transPtr->execSqlCoro("delete from topic_tags where topic_id=$1 and tag_id=$2", tid, row["id"].as<size_t>());
                    }
                    // increase the count for new tags
                    for (auto &tag : r)
                    {
                        co_await transPtr->execSqlCoro("update tags set topic_count=topic_count + 1 where name=$1", tag["name"].as<std::string>());
                        co_await transPtr->execSqlCoro("insert into topic_tags(topic_id, tag_id) values($1, $2)", tid, tag["id"].as<size_t>());
                    }
                }
            }
            catch (const DrogonDbException &e)
            {
                ret["error"] = (std::string)e.base().what();
                callback(jsonResponse(std::move(ret)));
                co_return;
            }
        }
        // this will come with those topics who have tags
        if (title != "")
        {
            try
            {
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
                co_await transPtr->execSqlCoro("update topics set title=$1, slug=$1 where id=$3", title, slug, tid);
            }
            catch (const DrogonDbException &e)
            {
                ret["error"] = (std::string)e.base().what();
                callback(jsonResponse(std::move(ret)));
                co_return;
            }
        }
        // this is to be set irrespective of tags and body
        try
        {
            co_await transPtr->execSqlCoro("update topics set description=$1 where id=$2", body, tid);
            std::string id;
            std::string slug;
            // we assume that topic being edited is not original topic
            // in this case row will be empty
            auto row = co_await transPtr->execSqlCoro("select id, slug from topics where op_id=0 and id=$1", tid);
            LOG_DEBUG << row.size();
            if (row.size())
            { // topic is original topic
                for (auto &r : row)
                {
                    id = r["id"].as<std::string>();
                    slug = r["slug"].as<std::string>();
                }
            }
            else
            {
                auto row = co_await transPtr->execSqlCoro("select id, slug from topics where id in (select op_id from topics where op_id!=0 and id=$1)", tid);
                if (row.size())
                { // topic is original topic
                    for (auto &r : row)
                    {
                        id = r["id"].as<std::string>();
                        slug = r["slug"].as<std::string>();
                    }
                }
            }
            ret["id"] = id;
            ret["slug"] = slug;
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        catch (const DrogonDbException &e)
        {
            ret["error"] = (std::string)e.base().what();
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
    }
    co_return;
}

auto Topic::acceptAnswer(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const std::string &tid, const std::string &aid) -> Task<>
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

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        auto transPtr = co_await clientPtr->newTransactionCoro();

        try
        {
            auto r = co_await transPtr->execSqlCoro("select posted_by from topics where id=$1", tid);
            if (r.size() != 0)
            {
                auto topic_user_id = r[0]["posted_by"].as<int64_t>();
                if (user_id == topic_user_id)
                {
                    ret["error"] = "You cannot accept your own answer.";
                    callback(jsonResponse(std::move(ret)));
                    co_return;
                }
            }
            r = co_await transPtr->execSqlCoro("select * from topics where op_id=$1 and answer_accepted=true", tid);
            auto answer_accepted_reputation = customConfig.get("answer_accepted_reputation", 15).asInt64();
            auto answer_accepting_reputation = customConfig.get("answer_accepting_reputation", 2).asInt64();
            if (r.size() != 0)
            {
                auto id = r[0]["id"].as<size_t>();
                auto receiving_user = r[0]["posted_by"].as<size_t>();
                co_await transPtr->execSqlCoro("update topics set answer_accepted=false where id=$1", id);
                // fetch the user's whose reputation is to be decreased
                auto r1 = co_await transPtr->execSqlCoro("select reputation from users where id=$1 limit 1", receiving_user);
                LOG_DEBUG << r1.size();
                if (r1.size() != 0)
                {
                    auto reputation = r1[0]["reputation"].as<int64_t>();
                    LOG_DEBUG << "Hello";
                    if (reputation - answer_accepted_reputation < 1)
                    {
                        co_await transPtr->execSqlCoro("update users set reputation = 1 where id=$1", receiving_user);
                    }
                    else
                    {
                        co_await transPtr->execSqlCoro("update users set reputation = reputation - $2 where id=$1", receiving_user, answer_accepted_reputation);
                    }
                    r1 = co_await transPtr->execSqlCoro("select reputation from users where id=$1 limit 1", user_id);
                    LOG_DEBUG << "Hello1";
                    reputation = r1[0]["reputation"].as<int64_t>();
                    if (reputation - answer_accepting_reputation < 1)
                    {
                        co_await transPtr->execSqlCoro("update users set reputation = 1 where id=$1", user_id);
                    }
                    else
                    {
                        co_await transPtr->execSqlCoro("update users set reputation = reputation - $2 where id=$1", user_id, answer_accepted_reputation);
                    }
                }
                std::stringstream sstream(aid);
                size_t result;
                sstream >> result;
                if (id == result)
                {
                    ret["success"] = true;
                    callback(jsonResponse(std::move(ret)));
                    co_return;
                }
            }
            r = co_await transPtr->execSqlCoro("select * from topics where id=$1", aid);
            auto receiving_user = r[0]["posted_by"].as<size_t>();
            co_await transPtr->execSqlCoro("update users set reputation = reputation + $2 where id=$1", receiving_user, answer_accepted_reputation);
            co_await transPtr->execSqlCoro("update users set reputation = reputation + $2 where id=$1", user_id, answer_accepting_reputation);
            co_await transPtr->execSqlCoro("update topics set answer_accepted=true where id=$1", aid);
            ret["success"] = true;
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        catch (const DrogonDbException &e)
        {
            ret["error"] = (std::string)e.base().what();
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
    }
    co_return;
}