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

    if (decoded_jwt.username == "")
    {
        ret["error"] = "Invalid JWT.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    LOG_DEBUG << decoded_jwt.userID;

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transactionPtr) mutable {
            assert(transactionPtr);
            auto customConfig = app().getCustomConfig();
            std::string tags = "";

            auto j = *json;

            for (int i = 0; i < j["topic"]["tagList"].size(); i++)
            {
                auto tag = j["topic"]["tagList"][i].asString();
                LOG_DEBUG << tag;
                if (tag.length() > 64)
                {
                    LOG_DEBUG << "Too big tag: " + tag;
                    ret["error"] = "Too big tag: " + tag;
                    callback(jsonResponse(std::move(ret)));
                }
                tags += "'" + tag + "',"; // need to make it compatible for array syntax for SQL
            }

            tags = tags.substr(0, tags.size() - 1); // remove last comma
            LOG_DEBUG << tags;

            transactionPtr->execSqlAsync(
                "select * from tags where name in (" + tags + ")",
                [=](const Result &r) mutable {
                    if (r.size() != 0)
                    {
                        for (auto &r1 : r)
                        {
                            LOG_DEBUG << r1["id"].as<std::string>();
                        }

                        ret["error"] = "";
                        callback(jsonResponse(std::move(ret)));
                    }
                    else
                    {
                        LOG_DEBUG << "Failed";
                        ret["error"] = "Failed";
                        callback(jsonResponse(std::move(ret)));
                        return;
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