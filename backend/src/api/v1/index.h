/**
 *
 *  index.h
 *  This file is generated by drogon_ctl
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "util/arth.h"

using namespace drogon;

namespace api::v1
{
    /**
 * @brief this class is created by the drogon_ctl command (drogon_ctl create
 * controller -r arth::login). this class is a restful API controller.
 */
    class Index : public drogon::HttpController<Index>
    {
    public:
        METHOD_LIST_BEGIN
        // use METHOD_ADD to add your custom processing function here;
        // ADD_METHOD_TO(Index::index, "/api/v1/topics", Get);
        ADD_METHOD_TO(Index::index, "/api/v1/topics/{size_t page}", Get);
        METHOD_LIST_END

        void index(const HttpRequestPtr &req, Callback callback, const std::string &page);
    };
} // namespace api::v1
