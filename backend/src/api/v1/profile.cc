#include <cstddef>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>
#include <trantor/utils/Logger.h>

#include "profile.h"
#include "util/arth.h"
#include "util/emailutils.h"
#include "util/input_validation.h"
#include "util/string_util.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Profile::getProfile(const HttpRequestPtr &req, Callback callback, const long userID, const std::string &username)
{
    LOG_DEBUG << "user id: " << userID;

    Json::Value ret;
    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    if (jwt_secret == "")
    {
        ret["error"] = "JWT not configured";
        callback(jsonResponse(std::move(ret)));
        return;
    }
    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);
    bool logged_in = false;
    if (optionalToken.has_value())
    {
        logged_in = true;
    }

    auto token = optionalToken.value();
    auto viewer_id = token.userID;

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

                if (authenticated && viewer_id == userID)
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
            userID);
    }
}

std::string Profile::request_check(Json::Value &ret, const HttpRequestPtr &req, Callback &callback, auto &json, const long userID)
{
    if (json == nullptr)
    {
        ret["error"] = "Malformed request";
        callback(jsonResponse(std::move(ret)));
        return "";
    }

    auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    if (jwt_secret == "")
    {
        ret["error"] = "JWT not configured";
        callback(jsonResponse(std::move(ret)));
        return "";
    }
    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);

    if (!optionalToken.has_value())
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return "";
    }

    auto token = optionalToken.value();
    if (token.userID != userID)
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return "";
    }

    return jwt_secret;
}

void Profile ::updateUsername(const HttpRequestPtr &req, Callback callback, const long userID, const std::string &username)
{
    auto json = req->getJsonObject();

    Json::Value ret;

    auto jwt_secret = request_check(ret, req, callback, json, userID);

    if (username.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (!isUsernameValid(username))
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
                        "username_lower = $2"
                        "where id = $3",
                        [=](const Result &r) mutable {
                            if (r.affectedRows() == 0)
                            {
                                ret["error"] = "user doesnt exist";
                            }

                            LOG_DEBUG << "in result callback";
                            ret["message"] = "Changes saved";

                            LOG_DEBUG << "Log in successful";

                            ret["jwt"] = signJWT(userID, usernameLower, jwt_secret);
                            callback(jsonResponse(std::move(ret)));
                            return;
                            callback(jsonResponse(std::move(ret)));
                        },
                        [=](const DrogonDbException &e) mutable {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                        },
                        username, usernameLower, userID);
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                },
                username);
        });
    }
}

auto Profile::updateTitle(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const long user_id, const std::string &title) -> Task<>
{
    auto json = req->getJsonObject();

    Json::Value ret;

    auto jwt_secret = request_check(ret, req, callback, json, user_id);

    if (title.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }

    {
        Json::Value ret;
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            co_await clientPtr->execSqlCoro("update users set title=$1 where id = $2", trim(title), (long)user_id);
            ret["message"] = "Title updated";
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        catch (const DrogonDbException &e)
        {
            LOG_DEBUG << e.base().what();
            ret["error"] = (std::string)e.base().what();
            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
        }
    }

    co_return;
}

auto Profile::updateName(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const long user_id, const std::string &name) -> Task<>
{
    auto json = req->getJsonObject();

    Json::Value ret;

    auto jwt_secret = request_check(ret, req, callback, json, user_id);

    if (name.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }

    {
        Json::Value ret;
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            co_await clientPtr->execSqlCoro("update users set name=$1 where id = $2", trim(name), (long)user_id);
            ret["message"] = "Name updated";
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        catch (const DrogonDbException &e)
        {
            LOG_DEBUG << e.base().what();
            ret["error"] = (std::string)e.base().what();
            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
        }
    }

    co_return;
}

auto Profile::updateDesignation(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, const long user_id, const std::string &designation) -> Task<>
{
    auto json = req->getJsonObject();

    Json::Value ret;

    auto jwt_secret = request_check(ret, req, callback, json, user_id);

    if (designation.empty())
    {
        ret["error"] = "Some or all of the parameters are empty";
        callback(jsonResponse(std::move(ret)));
        co_return;
    }

    {
        Json::Value ret;
        auto clientPtr = drogon::app().getFastDbClient();
        try
        {
            co_await clientPtr->execSqlCoro("update users set designation=$1 where id = $2", trim(designation), (long)user_id);
            ret["message"] = "Designation updated";
            callback(jsonResponse(std::move(ret)));
            co_return;
        }
        catch (const DrogonDbException &e)
        {
            LOG_DEBUG << e.base().what();
            ret["error"] = (std::string)e.base().what();
            callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
        }
    }

    co_return;
}