#pragma once
#include <drogon/HttpController.h>

#include "util/arth.h"

using namespace drogon;

namespace api::v1 {
class profile:public drogon::HttpController<profile>
{
  public:    
    METHOD_LIST_BEGIN    
    /// Post because user may require authentication. 
    /// Get requests with bodies are not standard.
    ADD_METHOD_TO(profile ::getProfile, "/api/v1/profile/{string username}", Get, Options); 
    METHOD_LIST_END

    void getProfile(const HttpRequestPtr &req, Callback callback, std::string &&username);
};
}//namespace api::v1
