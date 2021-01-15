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
            ADD_METHOD_TO(Topic ::getTopic, "/api/v1/t/{string id}/{string slug}", Get, Options);
            METHOD_LIST_END

            void createTopic(const HttpRequestPtr &req, Callback callback);
            void getTopic(const HttpRequestPtr &req, Callback callback, const std::string& id, const std::string &slug);
        };
    } // namespace v1
} // namespace api