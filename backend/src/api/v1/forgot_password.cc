#include <cassert>

#include <chrono>
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <uuid/uuid.h>

#include "forgot_password.h"
#include "util/arth.h"
#include "util/emailutils.h"
#include "util/input_validation.h"
#include "util/datetime.h"

#include "plugins/email.h"


using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

std::string uuid() {
  uuid_t uuid_token;
  uuid_generate(uuid_token);

  /// uuid is 16 bytes
  char uuid_buffer[17];
  uuid_unparse(uuid_token, uuid_buffer);

  return uuid_buffer;
}

void ForgotPassword::forgotPassword(const HttpRequestPtr &req,
                                    Callback callback) {
    auto json = req->getJsonObject();

    Json::Value ret;
    if (json == nullptr) {
        ret["error"] = "Invalid input";
        callback(jsonResponse(std::move(ret)));
    }    

    std::string email = json->get("email", "").asString();
    std::string username = json->get("username", "").asString();

    LOG_DEBUG << "username: " << username;
    LOG_DEBUG << "email: " << email;
    EmailUtils::cleanEmail(email);
    LOG_DEBUG << "cleaned email: " << email;

    if (email == "" && username == "") {
        LOG_DEBUG << "Neither username nor email supplied";
        ret["error"] = "Neither username nor email supplied";
        callback(jsonResponse(std::move(ret)));
        return;
    } 

    if (email != "" && username != "") {
        LOG_DEBUG << "Both username and email supplied";
        ret["error"] = "Both username and email supplied";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (username != "" && !isUsernameValid(username)) {
        LOG_DEBUG << "Username should be less than 60 characters and should not contain spaces";
        ret["error"] = "Username should be less than 60 characters and should not contain spaces";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    if (email != "" && !isEmailValid(email)) {
        LOG_DEBUG << "Email should be less than 256 characters and should not contain spaces";
        ret["error"] = "Email should be less than 256 characters and should not contain spaces";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    {
        auto clientPtr = drogon::app().getFastDbClient("default");
        clientPtr->newTransactionAsync([=](TransactionPtr transactionPtr) mutable {
            assert(transactionPtr);
            auto customConfig = app().getCustomConfig();

            transactionPtr->execSqlAsync(
                "select id, email_verified from users where email = $1 or username = $2",
                [=](const Result &r) mutable {                    

                    if (r.size() != 1) {
                        /// **Important**: Do not let the UI know that user does not exist. 
                        /// It can be misused by competitors or hackers to know who all are
                        /// affiliated with us. 
                        ret["message"] = "Email has been sent to " + email;

                        auto smtp = SMTPMail();
                        smtp.sendEMail(
                            email, 
                            "Password recovery for Arth", 
                            "The username or email provided for password "
                            "recovery does not exist in our database. Please "
                            "try again using the registered username or email", 
                            customConfig
                        );

                        LOG_WARN << "Non existant username or email requested for password recovery";

                        callback(jsonResponse(std::move(ret)));                                                
                        return;
                    }                    

                    auto row = r[0];

                    auto email_verified = row["email_verified"].as<bool>();

                    if (!email_verified) {
                        /// C and C++ concatenate such char* automatically. 
                        /// This is const char * not std::string. std::string 
                        /// is concatenated using operator +
                        ret["error"] = "Email not verified. You need to "
                        "verify your email before trying to reset password";
                        callback(jsonResponse(std::move(ret)));                                                
                        return;
                    }

                    auto user_id = row["id"].as<size_t>();                    

                    std::string token = uuid();
                    
                    std::string createdAt = DateTime::getLocalDateTimeISOFormat();
                    
                    transactionPtr->execSqlAsync(
                        "insert into email_tokens (user_id, email, token, created_at, updated_at) "
                        "values ($1, $2, $3, $4, $5)",

                        [=](const Result &r) mutable {                                                    
                            auto smtp = SMTPMail();
                            auto base_url = customConfig["base_url"].asString();
                            smtp.sendEMail(
                                email, 
                                "Password recovery for Arth", 
                                std::string("Click on the link <a href=\"") 
                                + base_url + "/verify-forgot-password?token=" + token + "\"> "
                                + "to change your password.",
                                customConfig
                            );

                            ret["message"] = "Email has been sent to " + email;
                            callback(jsonResponse(std::move(ret)));                            
                        },

                        [=](const DrogonDbException &e) mutable {
                            LOG_DEBUG << e.base().what();
                            ret["error"] = (std::string)e.base().what();
                            callback(jsonResponse(std::move(ret)));
                        },

                        user_id, email, token, createdAt, createdAt
                    );
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                },
                email, username);
        });
    }
}