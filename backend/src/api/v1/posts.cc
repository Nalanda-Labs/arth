#include "util/arth.h"
#include "util/input_validation.h"
#include "util/jwt_impl.h"

#include "posts.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

size_t toSizeT(std::string str) {
    // string to size_t
    std::stringstream ss;
    ss << str;		
    size_t temp;
    ss >> temp;
    return temp;
}

void Posts::createPost(const HttpRequestPtr &req, Callback callback, const size_t topic_id) {
	Json::Value ret;
	auto customConfig = app().getCustomConfig();
    auto jwt_secret = customConfig.get("jwt_secret", "").asString();

    auto optionalToken = verifiedToken(req->getHeader("Authorization"), jwt_secret);

    if (!optionalToken.has_value())
    {
        ret["error"] = "Authentication Error";
        callback(jsonResponse(std::move(ret)));
        return;
    }

    Token jwt = optionalToken.value();

    auto user_id = jwt.userID;
    LOG_DEBUG << user_id;
	
	auto json = req->getJsonObject();

	if (json == nullptr) {
		ret["error"] = "Malformed Request";
		callback(jsonResponse(std::move(ret)));
	}

	const std::string body = json->get("body", "").asString();
	/// reply_to is actually a size_t but js cannot represent numbers > 2^53 - 1.
	/// So it needs to be passed as string. Database knows the type of column,
	/// so it will implicitly convert it to int
	std::string replyTo = json->get("reply_to", "").asString();
	
	if (body.empty()) {
		ret["error"] = "Invalid input";
		callback(jsonResponse(std::move(ret)));
		return;
	}

	std::optional<size_t> optionalReplyTo;

	if (!replyTo.empty()) {    
        optionalReplyTo = toSizeT(replyTo);
	}

	{
		auto clientPtr = app().getFastDbClient("default");

        auto insertTopic = [=] () mutable {
            clientPtr->execSqlAsync(
                "select id, op_id from topics where id = $1",
                [=] (const Result &r) mutable {
                    if (r.size() == 0) {
                        ret["error"] = "Topic or post does not exist";
                        callback(jsonResponse(std::move(ret)));
                        return;
                    }

                    auto row = r[0];

                    size_t op_id;

                    bool isTopic = row["op_id"].isNull();
                    if (isTopic) {
                        op_id = row["id"].as<size_t>();
                    } else {
                        op_id = row["op_id"].as<size_t>();
                    }

                    *clientPtr << "insert into topics(description, posted_by, op_id, reply_to) "
                                "values ($1, $2, $3, $4) returning id"
                            << body << user_id << op_id << std::move(optionalReplyTo)
                            >> [=] (const Result &r) mutable {
                                auto post_id = r[0]["id"].as<std::string>();
                                ret["post_id"] = post_id;
                                callback(jsonResponse(std::move(ret)));
                            } 
                            >> [=](const DrogonDbException &e) mutable {
                                LOG_DEBUG << e.base().what();
                                ret["error"] = (std::string)e.base().what();
                                callback(jsonResponse(std::move(ret)));
                            };
                },
                [=](const DrogonDbException &e) mutable {
                    LOG_DEBUG << e.base().what();
                    ret["error"] = (std::string)e.base().what();
                    callback(jsonResponse(std::move(ret)));
                },
                topic_id
            );            
        };

        if (!optionalReplyTo.has_value()) {
            insertTopic();
            return;
        }

        clientPtr->execSqlAsync(
            "select id from users where id = $1", 
            [=] (const Result &r) mutable {
                if (r.empty()) {
                    ret["error"] = "User does not exist";
                    callback(jsonResponse(std::move(ret)));
                    return;
                }

                insertTopic();
            }, 
            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(jsonResponse(std::move(ret)));
            }, 
            optionalReplyTo.value()
        );
	}
	
}

