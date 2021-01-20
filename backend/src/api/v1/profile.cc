#include <cstddef>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>
#include <trantor/utils/Logger.h>

#include "profile.h"
#include "util/arth.h"
#include "util/emailutils.h"
#include "util/input_validation.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Profile::getProfile(const HttpRequestPtr &req, Callback callback, size_t userID)
{
    LOG_DEBUG << "user id: " << userID;

    Json::Value ret;

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->execSqlAsync(
            "select username, name, title, designation, location, email, image_url from users "
            "where id = $1",

            [=](const Result &r) mutable {
                if (r.size() != 1)
                {
                    ret["error"] = "User does not exist";
                    callback(jsonResponse(std::move(ret)));
                    return;
                }

                auto row = r[0];

                ret["username"] = row["username"].as<std::string>();
                ret["name"] = row["name"].as<std::string>();
                ret["title"] = row["title"].as<std::string>();
                ret["designation"] = row["designation"].as<std::string>();
                ret["location"] = row["location"].as<std::string>();
                ret["image_url"] = row["image_url"].as<std::string>();

                auto customConfig = app().getCustomConfig();
                const auto jwt_secret = customConfig.get("jwt_secret", "").asString();
                auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);

                bool authenticated = optionalToken.has_value();

                if (authenticated)
                {
                    ret["email"] = row["email"].as<std::string>();
                }

                callback(jsonResponse(std::move(ret)));
            },

            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },
            userID
        );
    }
}


void Profile ::updateProfile(const HttpRequestPtr &req, Callback callback, size_t userID)
{
    auto json = req->getJsonObject();

    Json::Value ret;
    if (json == nullptr)
    {
        ret["error"] = "Malformed request";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto username = json->get("username", "").asString();
    auto name = json->get("name", "").asString();
    auto title = json->get("title", "").asString();
    auto designation = json->get("designation", "").asString();
    auto email = json->get("email", "").asString();

    LOG_DEBUG << "new username: " << username;
    LOG_DEBUG << "name: " << name;
    LOG_DEBUG << "title: " << title;
    LOG_DEBUG << "designation: " << designation;
    LOG_DEBUG << "email: " << email;
    EmailUtils::cleanEmail(email);
    LOG_DEBUG << "cleaned email: " << email;

    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    if (jwt_secret == "")
    {
        ret["error"] = "JWT not configured";
        callback(jsonResponse(std::move(ret)));
        return;
    }
    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);

    if (!optionalToken.has_value())
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto token = optionalToken.value();
    if (token.userID != userID)
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (username.empty() || name.empty() || title.empty() || designation.empty() || email.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (!isUsernameValid(username) || !isEmailValid(email))
    {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transPtr) mutable {
            transPtr->execSqlAsync(
                "select id from users where username = $1",
                [=](const Result &r) mutable {
                    if (!r.empty() && r[0]["id"].as<size_t>() != userID)
                    {
                        ret["error"] = "Username is already taken";
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }
                    auto usernameLower = username;

                    transform(usernameLower.begin(), usernameLower.end(), usernameLower.begin(), ::tolower);
                    LOG_DEBUG << "username lower: " << usernameLower;

                    transPtr->execSqlAsync(
                        "update users "
                        "set username = $1, "
                        "username_lower = $2, "
                        "name = $3, "
                        "title = $4, "
                        "designation = $5, "
                        "email = $6 "
                        "where id = $7",
                        [=](const Result &r) mutable {

                            if (r.affectedRows() == 0) {
                                ret["error"] = "user doesnt exist";
                            }

                            LOG_DEBUG << "in result callback";
                            ret["message"] = "Changes saved";
                            callback(jsonResponse(std::move(ret)));
                        },
                        [=](const DrogonDbException &e) mutable {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                        },
                        username, usernameLower, name, title, designation, email, userID
                    );
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                },
                username
            );
        });
    }
}


void Profile::summary(const HttpRequestPtr &req, Callback callback, size_t userID) {
    LOG_DEBUG << "user id: " << userID;

    Json::Value ret;

    auto customConfig = app().getCustomConfig();
    int paginationSize = customConfig.get("profile_pagination_size", 5).asInt();
    {
        auto clientPtr = app().getFastDbClient("default");

        /// top replies
        clientPtr->execSqlAsync(
            "select id, title, description, tag_ids from topics "
            "where posted_by = $1 "
            "and op_id is null "
            "order by likes desc limit " + std::to_string(paginationSize), 
            [=] (const Result &r) mutable {
                LOG_DEBUG << "in top replies";

                ret["top_replies"] = Json::arrayValue;
                for (const auto& row : r) {
                    Json::Value json;
                    json["id"] = row["id"].as<size_t>();
                    json["title"] = row["title"].as<std::string>();
                    json["description"] = row["description"].as<std::string>();
                    
                    json["tag_ids"] = Json::arrayValue;
                    
                    for (const auto& tag : row["tag_ids"].asArray<size_t>()) {
                        /// tag is a std::shared_ptr. We want to append 
                        /// the value not the pointer. So deref tag
                        json["tag_ids"].append(*tag);
                    }
                    ret["top_replies"].append(json);
                }

                /// top topics
                clientPtr->execSqlAsync(
                    "select id, title, description, tag_ids from topics "
                    "where op_id = $1 "
                    "order by likes desc limit " + std::to_string(paginationSize), 
                    [=] (const Result &r) mutable {
                        LOG_DEBUG << "in top topics";

                        ret["top_topics"] = Json::arrayValue;
                        for (const auto& row : r) {
                            Json::Value json;
                            json["id"] = row["id"].as<size_t>();
                            json["title"] = row["title"].as<std::string>();
                            json["tag_ids"] = Json::arrayValue;
                            
                            for (const auto& tag : row["tag_ids"].asArray<size_t>()) {
                                /// tag is a std::shared_ptr. We want to append 
                                /// the value not the pointer. So deref tag
                                json["tag_ids"].append(*tag);
                            }
                            ret["top_topics"].append(json);
                        }

                        callback(jsonResponse(std::move(ret)));
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                    },
                    userID
                );
            },
            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },
            userID
        );
    }
}