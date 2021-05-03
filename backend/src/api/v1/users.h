/**
 *
 *  topic.h
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "util/arth.h"

using namespace drogon;

namespace api
{
    namespace v1
    {
        /**
 * @brief this class is created by the drogon_ctl command (drogon_ctl create
 * controller -r arth::login). this class is a restful API controller.
 */
        class Users : public drogon::HttpController<Users>
        {
        public:
            METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(Users ::getUsers, "/api/v1/users/{std::string page}/", Get);
            METHOD_LIST_END
            auto getUsers(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const std::string& page) -> Task<>;
        };
    } // namespace v1
} // namespace api
