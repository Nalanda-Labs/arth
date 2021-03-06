#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
namespace api::v1 {
/// cannot be Post. It is defined in drogon for the HTTP method Post.
/// It will give compiler errors
class Posts:public drogon::HttpController<Posts>
{
  public:
    METHOD_LIST_BEGIN

	ADD_METHOD_TO(Posts::createPost, "/api/v1/create-post?topic_id={size_t id}",
				  Post, Options);
	ADD_METHOD_TO(Posts::getPost, "/api/v1/post/{size_t id}", Get, Options);
	ADD_METHOD_TO(Posts::updatePost, "/api/v1/post/{size_t id}", Put, Options);
	ADD_METHOD_TO(Posts::acceptAsAnswer, "/api/v1/post/accept_as_ans/{size_t id}", Post, Options);
	ADD_METHOD_TO(Posts::unacceptAnswer, "/api/v1/post/unaccept_ans/{size_t id}", Post, Options);
	ADD_METHOD_TO(Posts::upvote, "/api/v1/post/upvote/{size_t id}", Post, Options);
	ADD_METHOD_TO(Posts::downvote, "/api/v1/post/downvote/{size_t id}", Post, Options);
	
    METHOD_LIST_END
   
    auto createPost(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, size_t topic_id) -> Task<>;
	void getPost(const HttpRequestPtr &req, Callback callback, size_t post_id);
	void updatePost(const HttpRequestPtr &req, Callback callback, size_t post_id);
	void acceptAsAnswer(const HttpRequestPtr &req, Callback callback, size_t post_id);
	void unacceptAnswer(const HttpRequestPtr &req, Callback callback, size_t post_id);
	void upvote(const HttpRequestPtr &req, Callback callback, size_t post_id);
	void downvote(const HttpRequestPtr &req, Callback callback, size_t post_id);
};
} // namespace api::v1
