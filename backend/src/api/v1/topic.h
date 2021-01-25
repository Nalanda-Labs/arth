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
        class Topic : public drogon::HttpController<Topic>
        {
        public:
            METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(Topic ::createTopic, "/api/v1/t/create-topic/", Post, Options);
            ADD_METHOD_TO(Topic ::getTopic, "/api/v1/t/{size_t id}/{string slug}", Get, Options);
            // without the / at end get-discussion will be captured by getTopic route
            ADD_METHOD_TO(Topic ::getDiscussion, "/api/v1/t/{size_t id}/get-discussion/?"
                                                 "time={string created_at}&limit={uint limit}",
                          Get, Options);
            METHOD_LIST_END

            void createTopic(const HttpRequestPtr &req, Callback callback);
            void getTopic(const HttpRequestPtr &req, Callback callback, const size_t &tid, const std::string &slug);
            void getDiscussion(const HttpRequestPtr &req, Callback callback, const size_t &tid,
                               const std::string &created_at, const size_t &limit);
        };
    } // namespace v1
} // namespace api
