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

void Profile::getProfile(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username)
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


void Profile ::updateProfile(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username)
{
    auto json = req->getJsonObject();

    Json::Value ret;
    if (json == nullptr)
    {
        ret["error"] = "Malformed request";
        callback(jsonResponse(std::move(ret)));
        return;
    }

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
