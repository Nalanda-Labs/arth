/**
 *
 *  register.h
 *  This file is generated by drogon_ctl
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "util/drogon.h"

using namespace drogon;

namespace api::v1 
{
/**
 * @brief this class is created by the drogon_ctl command (drogon_ctl create controller -r arth::login).
 * this class is a restful API controller.
 */
class registration : public drogon::HttpController<registration>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    ADD_METHOD_TO(registration ::doRegister, "/api/v1/register", Post, Options);
    METHOD_LIST_END

    void doRegister(const HttpRequestPtr &req, Callback callback);
};
} // namespace api::v1
