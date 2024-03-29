/**
 *
 *  tags.h
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
        class Tags : public drogon::HttpController<Tags>
        {
        public:
            METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            ADD_METHOD_TO(Tags ::getTags, "/api/v1/get-tags/", Post, Options);
            ADD_METHOD_TO(Tags ::getAllTags, "/api/v1/tags/", Post);
            ADD_METHOD_TO(Tags::getTopicsByTag, "/api/v1/topics/tagged/{std::string tag}", Post);
            ADD_METHOD_TO(Tags ::getTag, "/api/v1/tags/edit/{std::string tag}", Get);
            ADD_METHOD_TO(Tags ::updateTagInfo, "/api/v1/tags/edit/{std::string tag}/", Post);
            METHOD_LIST_END

            void getTags(const HttpRequestPtr &req, Callback callback);
            auto getAllTags(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback) -> Task<>;
            auto getTopicsByTag(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const std::string& tag) -> Task<>;
            auto getTag(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const std::string& tag) -> Task<>;
            auto updateTagInfo(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const std::string& tag) -> Task<>;

        };
    } // namespace v1
} // namespace api