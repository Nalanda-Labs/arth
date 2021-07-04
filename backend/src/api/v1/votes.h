/**
 *
 *  votes.h
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
        class Votes : public drogon::HttpController<Votes>
        {
        public:
            METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(Votes ::votes, "/api/v1/votes/", Post, Options);
            METHOD_LIST_END

            auto votes(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback) -> Task<>;
        };
    } // namespace v1
} // namespace api
