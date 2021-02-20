#pragma once
#include <drogon/HttpController.h>

#include "util/arth.h"

using namespace drogon;

namespace api::v1 {
class Profile:public drogon::HttpController<Profile>
{
  public:    
    METHOD_LIST_BEGIN        
    ADD_METHOD_TO(Profile ::getProfile, "/api/v1/users/{user_id}/{username}", Get, Options);
    ADD_METHOD_TO(Profile ::updateProfile, "/api/v1/users/{user_id}/{username}", Put);
    METHOD_LIST_END

    void getProfile(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username);
    void updateProfile(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username);
};
}//namespace api::v1
