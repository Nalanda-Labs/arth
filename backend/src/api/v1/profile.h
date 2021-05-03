#pragma once
#include <drogon/HttpController.h>

#include "util/arth.h"

using namespace drogon;

namespace api::v1 {
class Profile:public drogon::HttpController<Profile>
{
  public:    
    METHOD_LIST_BEGIN        
    ADD_METHOD_TO(Profile ::getProfile, "/api/v1/user/{user_id}/{username}", Get, Options);
    ADD_METHOD_TO(Profile ::updateUsername, "/api/v1/profile/{user_id}/username/{username}/", Post);
    ADD_METHOD_TO(Profile ::updateTitle, "/api/v1/profile/{user_id}/title/{title}/", Post);
    ADD_METHOD_TO(Profile ::updateName, "/api/v1/profile/{user_id}/name/{name}/", Post);
    ADD_METHOD_TO(Profile ::updateDesignation, "/api/v1/profile/{user_id}/designation/{designation}/", Post);
    ADD_METHOD_TO(Profile ::updateDesignation, "/api/v1/profile/{user_id}/designation/{designation}/", Post);
    METHOD_LIST_END

    std::string request_check(Json::Value &ret, const HttpRequestPtr &req, Callback& callback, auto& json, const long userID);
    void getProfile(const HttpRequestPtr &req, Callback callback, const long userID, const std::string& username);
    void updateUsername(const HttpRequestPtr &req, Callback callback, const long userID, const std::string& username);
    auto updateTitle(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const long user_id, const std::string &title) -> Task<>;
    auto updateName(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const long user_id, const std::string &name) -> Task<>;
    auto updateDesignation(const HttpRequestPtr req, std::function<void(const HttpResponsePtr&)> callback, const long user_id, const std::string &designation) -> Task<>;
};
}//namespace api::v1
