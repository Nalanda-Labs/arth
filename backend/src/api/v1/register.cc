/**
 *
 *  register.cc
 *  This file is generated by drogon_ctl
 *
 */

#include <string>

#include <drogon/HttpClient.h>

#include "util/emailutils.h"
#include "util/datetime.h"
#include "util/password.h"
#include "plugins/email.h"
#include "register.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

const std::string registration::emailBody(const std::string &username, const std::string &base_url, const std::string &token)
{
    std::string link = base_url + "/verify-registration-email?token=" + token;

    std::string body = "Dear " + username + ",\n\n" +
                       "Thank you for registering at arth. Please click link " +
                       link + " to verify your email.\n\n\
                       Thanks,\n Team Arth ";

    return std::move(body);
}

void registration::doRegister(const HttpRequestPtr &req, Callback callback)
{
    auto json = req->getJsonObject();
    Json::Value ret;

    if (json == nullptr)
    {
        ret["error"] = "Malformed request.";
        auto resp = jsonResponse(std::move(ret));
        callback(resp);

        return;
    }

    auto name = json->get("name", "").asString();
    auto email = json->get("email", "").asString();
    auto username = json->get("username", "").asString();
    auto password = json->get("password", "").asString();
    Json::Value token;
    if (json->isMember("token"))
        token = json->get("token", "");

    LOG_DEBUG << "name: " << name;
    LOG_DEBUG << "email: " << email;
    LOG_DEBUG << "username: " << username;
    LOG_DEBUG << "password: " << password;

    // always validate user input. never rely on what is coming from the other side
    if (name == "" || username == "" || email == "" || password == "")
    {
        ret["error"] = "None of the fields can be empty.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    auto customConfig = app().getCustomConfig();

    if (token.isMember("isTrusted"))
    {
        std::string secret = "";
        if (customConfig.isMember("recaptcha_secret") && customConfig.get("recaptcha_secret", "") != "")
        {
            secret = customConfig.get("recaptcha_secret", "").asString();
        }
        else
        {
            ret["error"] = "Recaptcha improperly configured.";
            callback(jsonResponse(std::move(ret)));
            return;
        }

        auto client = HttpClient::newHttpClient(
            "https://www.google.com/");
        auto req = HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath("/recaptcha/api/siteverify");
        req->setParameter("secret", secret);
        req->setParameter("response", token.get("isTrusted", false).asString());

        client->sendRequest(
            req,
            [ret = ret, callback = callback](ReqResult result, const HttpResponsePtr &response) mutable {
                LOG_DEBUG << "received response!";
                // auto headers=response.
                std::shared_ptr<Json::Value> res = response->getJsonObject();
                if (res->isMember("success") && not res->get("success", false))
                {
                    ret["error"] = "Recaptcha test failed.";
                    callback(jsonResponse(std::move(ret)));
                    return;
                }
            });
    }
    else
    {
        ret["error"] = "Recaptcha token not found.";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](const std::shared_ptr<Transaction> &transPtr) mutable {
            assert(transPtr);
            EmailUtils::cleanEmail(email);

            if (email == "")
            {
                ret["error"] = "Invalid email.";
                callback(jsonResponse(std::move(ret)));
            }
            else
            {
                transPtr->execSqlAsync(
                    "select * from users where username=$1 or email=$2",
                    [=](const Result &r) mutable {
                        if (r.size() > 0)
                        {
                            LOG_DEBUG << "User exists";
                            ret["error"] = "User exists";
                            callback(jsonResponse(std::move(ret)));
                            return;
                        }
                        else
                        {
                            auto username_lower = username;

                            transform(username_lower.begin(), username_lower.end(), username_lower.begin(), ::tolower);
                            LOG_DEBUG << username_lower;
                            auto created_at = DateTime::getLocalDateTimeISOFormat();
                            auto password_hash = PasswordUtils::hashPassword(password, "");

                            LOG_DEBUG << std::get<0>(password_hash);
                            LOG_DEBUG << std::get<1>(password_hash);
                            if (std::get<0>(password_hash) != "" && std::get<1>(password_hash) != "")
                            {
                                uint8_t t[16];
                                arc4random_buf((void *)t, 16); // it takes a void* so we cast it
                                std::ostringstream convert;
                                for (int i = 0; i < 16; i++)
                                {
                                    convert << (int)t[i];
                                }

                                std::string ec = convert.str();
                                std::string token = Base64::encode(ec);

                                *transPtr << "insert into users(username, created_at, updated_at, username_lower, email, trust_level, \
                                              password_hash, salt, email_verification_code) values($1, $2, $3, $4, $5, 0, $6, $7, $8);"
                                          << username << created_at << created_at << username_lower << email << std::get<0>(password_hash) << std::get<1>(password_hash) << token >>
                                    [=](const Result &r) mutable {
                                        auto smtp = SMTPMail();
                                        // TODO: move subject string to translation file
                                        auto msgid = smtp.sendEmail(
                                            customConfig.get("smtp_server", "").asString(),
                                            customConfig.get("smtp_port", 25).asInt(),
                                            customConfig.get("admin_email", "").asString(),
                                            email,
                                            "Registration at arth",
                                            emailBody(username, customConfig.get("base_url", "").asString(), token),
                                            customConfig.get("admin_username", "").asString(),
                                            customConfig.get("password", "").asString(), nullptr);
                                        LOG_DEBUG << msgid;
                                        ret["username"] = username_lower;

                                        // we cannot check whether email has been sent or not
                                        // so we will need to fix this in another way in app
                                        ret["mesage"] = "An email has been sent to you. Please verify your email.";
                                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                                        return;
                                    } >>
                                    [=](const DrogonDbException &e) mutable {
                                        LOG_ERROR << "err:" << e.base().what();
                                        ret["error"] = (std::string)e.base().what();
                                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                                        return;
                                    };
                            }
                            else
                            {
                                LOG_ERROR << "Error hashing password";
                                ret["error"] = "An error occurred. Please contact support.";
                                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                                return;
                            }
                        }
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(jsonResponse(std::move(ret)));
                    },
                    username, email);
            }
        });
    }
}

void registration::verifyEmail(const HttpRequestPtr &req, Callback callback, const std::string &token)
{
    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        Json::Value ret;

        clientPtr->newTransactionAsync([=](const std::shared_ptr<Transaction> &transPtr) mutable {
            transPtr->execSqlAsync(
                "select * from users where email_verification_code=$1",
                [=](const Result &r) mutable {
                    if (r.size() == 1)
                    {
                        *transPtr << "update users set email_varified=true, email_verification_code='' where email_verification_code=$1" << token >>
                            [=](const Result &r) mutable {
                                ret["message"] = "Email verified. Login to continue.";
                                callback(jsonResponse(std::move(ret)));
                                return;
                            } >>
                            [=](const DrogonDbException &e) mutable {
                                LOG_DEBUG << e.base().what();
                                ret["error"] = (std::string)e.base().what();
                                callback(jsonResponse(std::move(ret)));
                                return;
                            };
                    }
                    else
                    {
                        LOG_DEBUG << "Token not found.";
                        ret["error"] = "Token not found.";
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                },
                token);
        });
    }
}