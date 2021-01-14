/**
 *
 *  forgot_password.h
 *  This file is generated by drogon_ctl
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "util/arth.h"

using namespace drogon;

namespace api::v1 {
/**
 * @brief this class is created by the drogon_ctl command (drogon_ctl create
 * controller -r arth::login). this class is a restful API controller.
 */
class ForgotPassword : public drogon::HttpController<ForgotPassword> {
public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    ADD_METHOD_TO(ForgotPassword::forgotPassword, "/api/v1/forgot-password", Post, Options);
    ADD_METHOD_TO(ForgotPassword::changePassword, "/api/v1/change-password?token={string token}", Post, Options);
    METHOD_LIST_END

    void forgotPassword(const HttpRequestPtr &req, Callback callback);
    void changePassword(const HttpRequestPtr &req, Callback callback, const std::string &token);
};
} // namespace api::v1
