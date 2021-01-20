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

void Profile::getProfile(const HttpRequestPtr &req, Callback callback, std::string &&username)
{

    LOG_DEBUG << "username: " << username;

    Json::Value ret;

    if (!isUsernameValid(username))
    {
        LOG_DEBUG << "Username not valid";
        ret["error"] = "Username not valid";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->execSqlAsync(
            "select name, title, designation, location, email, image_url from users "
            "where username = $1",

            [=](const Result &r) mutable {
                if (r.size() != 1)
                {
                    ret["error"] = "User does not exist";
                    callback(jsonResponse(std::move(ret)));
                    return;
                }

                auto row = r[0];

                ret["username"] = username;
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

            username);
    }
}

struct User
{
    const size_t id;
    const std::string name, title, designation, email;

    User(const size_t id, const std::string name, const std::string title, const std::string designation, const std::string email)
        : id(id), name(name), title(title), designation(designation), email(email) {}
};

using DbResultCallback = std::function<void(const Result &)>;

/**
 * @brief Update user with old username to new username and using other details from user
 * 
 * @param transPtr transaction pointer to use
 * @param newUsername new username
 * @param oldUsername old username
 * @param user updated values of user
 * @param resultCallback callback containing result of update if update was successful
 * @param exceptionCallback callback returning the exeption if update failed
 */
void updateUser(
    TransactionPtr transPtr,
    const std::string newUsername,
    const std::string oldUsername,
    const User user,
    DbResultCallback resultCallback,
    DrogonDbExceptionCallback exceptionCallback);

void Profile ::updateProfile(const HttpRequestPtr &req, Callback callback, std::string &&username)
{
    auto json = req->getJsonObject();

    Json::Value ret;
    if (json == nullptr)
    {
        ret["error"] = "Malformed request";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto newUsername = json->get("newUsername", "").asString();
    auto name = json->get("name", "").asString();
    auto title = json->get("title", "").asString();
    auto designation = json->get("designation", "").asString();
    auto email = json->get("email", "").asString();

    LOG_DEBUG << "new username: " << newUsername;
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
    if (token.username != username)
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (username.empty() || newUsername.empty() || name.empty() || title.empty() || designation.empty() || email.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (!isUsernameValid(username) || !isUsernameValid(newUsername) || !isEmailValid(email))
    {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transPtr) mutable {
            User user{token.userID, name, title, designation, email};

            auto updateUser = [=]() mutable {
                auto usernameLower = newUsername;

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
                        LOG_DEBUG << "in result callback";
                        ret["message"] = "Changes saved";
                        callback(jsonResponse(std::move(ret)));
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                    },
                    newUsername, usernameLower, user.name, user.title, user.designation, user.email, user.id);
            };

            if (username != newUsername)
            {
                transPtr->execSqlAsync(
                    "select id from users where username = $1",
                    [=](const Result &r) mutable {
                        if (!r.empty() && newUsername != username)
                        {
                            ret["error"] = "Username is already taken";
                            callback(jsonResponse(std::move(ret)));
                            return;
                        }
                        updateUser();
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                    },
                    newUsername);
            }
            else
            {
                updateUser();
            }
        });
    }
}





void Profile::getStats(const HttpRequestPtr &req, Callback callback, std::string &&username) {
    LOG_DEBUG << "username: " << username;

    Json::Value ret;

    if (!isUsernameValid(username))
    {
        LOG_DEBUG << "Username not valid";
        ret["error"] = "Username not valid";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto customConfig = app().getCustomConfig();
    int paginationSize = customConfig.get("profile_pagination_size", 5).asInt();
    {
        auto clientPtr = app().getFastDbClient("default");

        /// top replies
        clientPtr->execSqlAsync(
            "select id, title, description, tag_ids from topics "
            "where posted_by = (select id from users where username = $1) "
            "and op_id is null "
            "order by likes desc limit " + std::to_string(paginationSize), 
            [=] (const Result &r) mutable {
                LOG_DEBUG << "in top replies";

                ret["top_replies"] = Json::arrayValue;
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
                }

                /// top topics
                clientPtr->execSqlAsync(
                    "select id, title, description, tag_ids from topics "
                    "where op_id = (select id from users where username = $1) "                    
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
                        }

                        callback(jsonResponse(std::move(ret)));
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                    },
                    username
                );
            },
            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },
            username
        );
    }
}