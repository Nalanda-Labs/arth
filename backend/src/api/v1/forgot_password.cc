#include <cassert>

#include <chrono>
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <uuid/uuid.h>

#include "forgot_password.h"
#include "util/arth.h"
#include "util/datetime.h"
#include "util/emailutils.h"
#include "util/input_validation.h"
#include "util/password.h"

#include "plugins/email.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

std::string uuid()
{
    uuid_t uuid_token;
    uuid_generate(uuid_token);

    /// uuid is 16 bytes
    char uuid_buffer[40];
    uuid_unparse(uuid_token, uuid_buffer);

    return uuid_buffer;
}

void ForgotPassword::forgotPassword(const HttpRequestPtr &req,
                                    Callback callback)
{
    auto json = req->getJsonObject();

    Json::Value ret;
    if (json == nullptr)
    {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
    }

    std::string email = json->get("email", "").asString();
    std::string username = json->get("username", "").asString();

    LOG_DEBUG << "username: " << username;
    LOG_DEBUG << "email: " << email;
    EmailUtils::cleanEmail(email);
    LOG_DEBUG << "cleaned email: " << email;

    if (email == "" && username == "")
    {
        LOG_DEBUG << "Neither username nor email supplied";
        ret["error"] = "Neither username nor email supplied";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (username != "" && !isUsernameValid(username))
    {
        LOG_DEBUG << "Username should be less than 60 characters and should not "
                     "contain spaces";
        ret["error"] = "Username should be less than 60 characters and should not "
                       "contain spaces";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (email != "" && !isEmailValid(email))
    {
        LOG_DEBUG << "Email should be less than 256 characters and should not "
                     "contain spaces";
        ret["error"] = "Email should be less than 256 characters and should not "
                       "contain spaces";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transactionPtr) mutable {
            assert(transactionPtr);
            auto customConfig = app().getCustomConfig();
            LOG_DEBUG << "Hello";
            transactionPtr->execSqlAsync(
                "select id, email_verified from users where email = $1 or username = "
                "$2",
                [=](const Result &r) mutable {
                    if (r.size() != 1)
                    {
                        /// **Important**: Do not let the UI know that user does not
                        /// exist. It can be misused by competitors or hackers to know who
                        /// all are affiliated with us.
                        ret["message"] = "Email has been sent to " + email;

                        auto smtp = SMTPMail();
                        auto base_url = customConfig.get("base_url", "").asString();
                        smtp.sendEMail(
                            email,
                            "Password recovery for Arth",
                            std::string("Someone tried to reset password on ") + base_url + ". "
                                                                                            "If you find this error please ignore this email.",
                            customConfig);

                        LOG_WARN << "Non existant username or email requested for "
                                    "password recovery";

                        callback(jsonResponse(std::move(ret)));
                        return;
                    }

                    auto row = r[0];

                    auto email_verified = row["email_verified"].as<bool>();

                    if (!email_verified)
                    {
                        /// C and C++ concatenate such char* automatically.
                        /// This is const char * not std::string. std::string
                        /// is concatenated using operator +
                        ret["error"] =
                            "Email not verified. You need to "
                            "verify your email before trying to reset password";
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }

                    auto user_id = row["id"].as<size_t>();

                    std::string token = uuid();

                    /// Token is unique indexed in the database.
                    /// If uuid collides (highly highly unlikely),
                    /// then insert will fail and user will get an error.
                    /// They will try again and succeed (mostly).
                    transactionPtr->execSqlAsync(
                        "insert into email_tokens (user_id, email, token)"
                        "values ($1, $2, $3)",

                        [=](const Result &r) mutable {
                            auto smtp = SMTPMail();
                            auto base_url = customConfig["base_url"].asString();
                            smtp.sendEMail(email, "Password recovery for Arth",
                                           std::string("Click on the link " +
                                               base_url +
                                               "/verify-forgot-password?token=" + token +
                                               " to change your password."),
                                           customConfig);

                            ret["message"] = "Email has been sent to " + email;
                            callback(jsonResponse(std::move(ret)));
                            return;
                        },

                        [=](const DrogonDbException &e) mutable {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(jsonResponse(std::move(ret)));
                            return;
                        },

                        user_id, email, token);
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                    return;
                },
                email, username);
        });
    }
}

void ForgotPassword::changePassword(const HttpRequestPtr &req, Callback callback, const std::string &token)
{
    auto json = req->getJsonObject();
    Json::Value ret;

    if (json == nullptr)
    {
        LOG_DEBUG << "Invalid input";
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
        return;
    }
        
    auto newPassword = json->get("new_password", "").asString();

    LOG_DEBUG << "token: " << token << "\n";
    LOG_DEBUG << "new password: " << newPassword;

    if (token == "" || newPassword == "")
    {
        LOG_DEBUG << "Invalid input";
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (!isPasswordValid(newPassword))
    {
        LOG_DEBUG << "Invalid password";
        ret["error"] = "Password should be at least 16 and 256 characters";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transactionPtr) mutable {
            transactionPtr->execSqlAsync(
                "select user_id from email_tokens "
                "where expired = false "
                "and confirmed = false "
                "and token = $1 ",
                [=](const Result &r) mutable {
                    /// Token is unique. inserting a token that already exists wont happen.
                    /// So we dont need to check if more than one row is affected
                    if (r.size() != 1)
                    {
                        LOG_DEBUG << "Token doesn't exist or is expired. Please try resetting password again";
                        ret["error"] = "Token doesn't exist or is expired. Please try resetting password again";
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }

                    auto [passwordHash, salt] = PasswordUtils::hashPassword(newPassword, "");

                    LOG_DEBUG << "password_hash: " << passwordHash;
                    LOG_DEBUG << "salt: " << salt;

                    if (passwordHash.empty() || salt.empty())
                    {
                        LOG_ERROR << "Error hashing password";
                        ret["error"] = "An error occurred. Please contact support.";
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                        return;
                    }

                    /// we have already checked if one row is returned
                    auto userID = r[0]["user_id"].as<size_t>();

                    transactionPtr->execSqlAsync(
                        "update users "
                        "set password_hash = $1, salt = $2 "
                        "where id = $3 ",
                        [=](const Result &r) mutable {
                            LOG_DEBUG << "Password updated";
                            ret["message"] = "Password updated";
                            callback(jsonResponse(std::move(ret)));
                            return;
                        },
                        [=](const DrogonDbException &e) mutable {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(jsonResponse(std::move(ret)));
                        },
                        passwordHash, salt, userID);
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