#pragma once
#include <drogon/HttpController.h>

#include "util/arth.h"

using namespace drogon;

class dummy:public drogon::HttpController<dummy>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(dummy::get, "/api/v1/dummy", Get);
    METHOD_LIST_END    

    void get(const HttpRequestPtr& req, Callback callback);
};
