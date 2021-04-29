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
    ADD_METHOD_TO(Profile ::updateUsername, "/api/v1/profile/{user_id}/username/{username}/", Post);
    // ADD_METHOD_TO(Profile ::updateTitle, "/api/v1/profile/{user_id}/title/{title}", Post);
    METHOD_LIST_END

    void getProfile(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username);
    void updateUsername(const HttpRequestPtr &req, Callback callback, const size_t userID, const std::string username);
    // auto updateTitle(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const std::string &title) -> Task<>;
};
}//namespace api::v1
