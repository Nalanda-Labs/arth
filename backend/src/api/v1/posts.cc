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

void Posts::getPost(const HttpRequestPtr &req, Callback callback, size_t post_id) {
	LOG_DEBUG << "Get post";
	Json::Value ret;
	
	auto clientPtr = app().getFastDbClient("default");
	LOG_DEBUG << clientPtr->connectionInfo();
	LOG_DEBUG << clientPtr->hasAvailableConnections();
	LOG_DEBUG << "Got DB client";
	clientPtr->execSqlAsync(
	    "select * from topics where id = $1",
		[=] (const Result &r) mutable {
			LOG_DEBUG << "Debug got database result";
			if (r.empty()) {
				ret["error"] = "Post does not exist";
				callback(jsonResponse(std::move(ret)));
				return;
			}

			auto row = r[0];
			/// JS cannot handle 64 bit ints
			ret["id"] = row["id"].as<std::string>();
			ret["title"] = row["title"].as<std::string>();
			ret["description"] = row["description"].as<std::string>();
			ret["posted_by"] = row["posted_by"].as<std::string>();
			/// No timestamp type in json :(
			ret["created_at"] = row["created_at"].as<std::string>();
			ret["updated_at"] = row["updated_at"].as<std::string>();
			ret["visible"] = row["visible"].as<bool>();
			ret["op_id"] = row["op_id"].as<std::string>();
			ret["updated_by"] = row["updated_by"].as<std::string>();
			ret["upvotes"] = row["upvotes"].as<int>();
			ret["reply_to"] = row["reply_to"].as<std::string>();

			auto tags = row["tag_ids"].asArray<std::string>();

			ret["tag_ids"] = Json::arrayValue;
			for (const std::shared_ptr<std::string> &tag : tags) {
				ret["tag_ids"].append(*tag);
			}

			callback(jsonResponse(std::move(ret)));
		},
		[=](const DrogonDbException &e) mutable {
            LOG_DEBUG << e.base().what();
			ret["error"] = (std::string)e.base().what();
			callback(jsonResponse(std::move(ret)));
        },
		post_id
	);

	LOG_DEBUG << "Testing";
}

void Posts::updatePost(const HttpRequestPtr &req, Callback callback, size_t post_id) {
	auto json = req->getJsonObject();
	Json::Value ret;
	if (json == nullptr) {
		ret["error"] = "Malformed request";
		callback(jsonResponse(std::move(ret)));
		return;
	}

	auto title = json->get("title", "").asString();
	auto description = json->get("description", "").asString();

	if (title.empty() || description.empty()) {
		ret["error"] = "Invalid input";
		callback(jsonResponse(std::move(ret)));
		return;
	}

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

	{
		auto clientPtr = app().getFastDbClient("default");
		clientPtr->newTransactionAsync([=] (TransactionPtr transPtr) mutable {
			/// Could have been a simple update for now.
			/// But when we add badge and reputations, we will need this anyway.
			transPtr->execSqlAsync(
			    "select posted_by from topics where id = $1",
				[=] (const Result &r) mutable {
					if (r.empty()) {
						ret["error"] = "Post does not exist";
						callback(jsonResponse(std::move(ret)));
						return;
					}

					auto posted_by = r[0]["posted_by"].as<size_t>();
					
					if (posted_by != jwt.userID) {
						ret["error"] = "You do not have the privileges to update this post";
						callback(jsonResponse(std::move(ret)));
						return;
					}

					transPtr->execSqlAsync(
					    "update topics "
						"set title = $1, description = $2 "
						"where id = $3",
						[=] (const Result &r) mutable {
							ret["success"] = true;
							callback(jsonResponse(std::move(ret)));							
						},
						[=] (const DrogonDbException &e) mutable {
							LOG_DEBUG << e.base().what();
							ret["error"] = (std::string)e.base().what();
							callback(jsonResponse(std::move(ret)));
						},
						title, description, post_id
					);
				},			
				[=] (const DrogonDbException &e) mutable {
					LOG_DEBUG << e.base().what();
					ret["error"] = (std::string)e.base().what();
					callback(jsonResponse(std::move(ret)));
				},
				post_id
			);
		});
	}
}


void Posts::acceptAsAnswer(const HttpRequestPtr &req, Callback callback, size_t post_id) {
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

    auto userID = optionalToken.value().userID;

	{
		auto clientPtr = app().getFastDbClient("default");
		clientPtr->newTransactionAsync([=] (TransactionPtr transPtr) mutable {
			transPtr->execSqlAsync(
				"select id, posted_by from topics where id = (select op_id from topics where id = $1)",
				[=] (const Result &result) mutable {
					if (result.empty()) {
						ret["error"] = "Post does not exist";
						callback(jsonResponse(std::move(ret)));
						return;						
					}

					auto original_post_id = result[0]["id"].as<size_t>();
					auto posted_by = result[0]["posted_by"].as<size_t>();
					
					if (posted_by != userID) {
						ret["error"] = "You cannot accept answer as you are not the origianl author of the topic";
						callback(jsonResponse(std::move(ret)));
						return;
					}

					transPtr->execSqlAsync(
						"select id from topics where op_id = $1 and accepted = true",
						[=] (const Result &result) mutable {
							if (!result.empty()) {
								ret["error"] = "This topic already has an accepted answer";
								callback(jsonResponse(std::move(ret)));
								return;
							}

							transPtr->execSqlAsync(
								"update topics set accepted = true where id = $1",
								[=] (const Result &result) mutable {	
									ret["success"] = true;
									callback(jsonResponse(std::move(ret)));
								},
								[=] (const DrogonDbException &e) mutable {
									LOG_DEBUG << e.base().what();
									ret["error"] = (std::string)e.base().what();
									callback(jsonResponse(std::move(ret)));
								},
								post_id
							);
						},
						[=] (const DrogonDbException &e) mutable {
						   LOG_DEBUG << e.base().what();
						   ret["error"] = (std::string)e.base().what();
						   callback(jsonResponse(std::move(ret)));
						},
						original_post_id
					);
				},
				[=] (const DrogonDbException &e) mutable {
				   LOG_DEBUG << e.base().what();
				   ret["error"] = (std::string)e.base().what();
				   callback(jsonResponse(std::move(ret)));
				},
				post_id
			);		   
		});
	}
}

void Posts::upvote(const HttpRequestPtr &req, Callback callback, size_t post_id) {
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

    auto userID = optionalToken.value().userID;

	{
		auto clientPtr = app().getFastDbClient("default");

		clientPtr->newTransactionAsync([=] (TransactionPtr transPtr) mutable {
			transPtr->execSqlAsync(
				"select id from topics where id = $1",
				[=] (const Result &result) mutable {
					if (result.empty()) {
						ret["error"] = "Post does not exist";
						callback(jsonResponse(std::move(ret)));
						return;
					}
					
					transPtr->execSqlAsync(						
						"select user_id from upvotes where topic_id = $1 and user_id = $2",
						[=] (const Result &result) mutable {
							if (!result.empty()) {
								ret["error"] = "Post already upvoted";
								callback(jsonResponse(std::move(ret)));
								return;
							}
			  

							transPtr->execSqlAsync(
								"insert into upvotes(topic_id, user_id) values ($1, $2)",
								[=] (const Result &result) mutable {
									transPtr->execSqlAsync(
										"delete from downvotes where topic_id = $1 and user_id = $2",
										[=] (const Result &result) mutable {
											/// If someone downvotes and then upvotes, 2 votes should be added
											int upvotesToAdd = result.affectedRows() + 1;
											transPtr->execSqlAsync(
												"update topics set upvotes = upvotes + $1 where id = $2",
												[=] (const Result &result) mutable {
													ret["success"] = true;
													callback(jsonResponse(std::move(ret)));
												},
												[=] (const DrogonDbException &e) mutable {
													LOG_DEBUG << e.base().what();
													ret["error"] = (std::string)e.base().what();
													callback(jsonResponse(std::move(ret)));
												},				
												upvotesToAdd, post_id
											);																						
										},
										[=] (const DrogonDbException &e) mutable {
											LOG_DEBUG << e.base().what();
											ret["error"] = (std::string)e.base().what();
											callback(jsonResponse(std::move(ret)));
										},
										post_id, userID
									);									
								},
								[=] (const DrogonDbException &e) mutable {
									LOG_DEBUG << e.base().what();
									ret["error"] = (std::string)e.base().what();
									callback(jsonResponse(std::move(ret)));
								},
								post_id, userID
							);
						},
						[=] (const DrogonDbException &e) mutable {
						   LOG_DEBUG << e.base().what();
						   ret["error"] = (std::string)e.base().what();
						   callback(jsonResponse(std::move(ret)));
						},
						post_id, userID
						);
				},
				[=] (const DrogonDbException &e) mutable {
				   LOG_DEBUG << e.base().what();
				   ret["error"] = (std::string)e.base().what();
				   callback(jsonResponse(std::move(ret)));
				},
				post_id
			);
			
		});
	}
}

void Posts::downvote(const HttpRequestPtr &req, Callback callback, size_t post_id) {
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

    auto userID = optionalToken.value().userID;

	{
		auto clientPtr = app().getFastDbClient("default");

		clientPtr->newTransactionAsync([=] (TransactionPtr transPtr) mutable {
			transPtr->execSqlAsync(
				"select id from topics where id = $1",
				[=] (const Result &result) mutable {
					if (result.empty()) {
						ret["error"] = "Post does not exist";
						callback(jsonResponse(std::move(ret)));
						return;
					}
					
					transPtr->execSqlAsync(						
						"select user_id from downvotes where topic_id = $1 and user_id = $2",
						[=] (const Result &result) mutable {
							if (!result.empty()) {
								ret["error"] = "Post already downvoted";
								callback(jsonResponse(std::move(ret)));
								return;
							}
			  
							transPtr->execSqlAsync(
								"insert into downvotes(topic_id, user_id) values ($1, $2)",
								[=] (const Result &result) mutable {
									transPtr->execSqlAsync(
										"delete from upvotes where topic_id = $1 and user_id = $2",
										[=] (const Result &result) mutable {
											/// If someone upvotes and then downvoted, two votes should be subtracted
											int upvotesToSubtract = result.affectedRows() + 1;
											transPtr->execSqlAsync(
												"update topics set upvotes = upvotes - $1 where id = $2",
												[=] (const Result &result) mutable {
													ret["success"] = true;
													callback(jsonResponse(std::move(ret)));
												},
												[=] (const DrogonDbException &e) mutable {
													LOG_DEBUG << e.base().what();
													ret["error"] = (std::string)e.base().what();
													callback(jsonResponse(std::move(ret)));
												},				
												upvotesToSubtract, post_id
											);											
										},
										[=] (const DrogonDbException &e) mutable {
											LOG_DEBUG << e.base().what();
											ret["error"] = (std::string)e.base().what();
											callback(jsonResponse(std::move(ret)));
										}, post_id, userID
									);
									
								},
								[=] (const DrogonDbException &e) mutable {
									LOG_DEBUG << e.base().what();
									ret["error"] = (std::string)e.base().what();
									callback(jsonResponse(std::move(ret)));
								},
								post_id, userID
							);
						},
						[=] (const DrogonDbException &e) mutable {
						   LOG_DEBUG << e.base().what();
						   ret["error"] = (std::string)e.base().what();
						   callback(jsonResponse(std::move(ret)));
						},
						post_id, userID
						);
				},
				[=] (const DrogonDbException &e) mutable {
				   LOG_DEBUG << e.base().what();
				   ret["error"] = (std::string)e.base().what();
				   callback(jsonResponse(std::move(ret)));
				},
				post_id
			);			
		});
	}
}
