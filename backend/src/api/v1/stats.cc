#include "stats.h"

using namespace drogon;
using namespace drogon::orm;
using namespace api::v1;

void Stats::summary(const HttpRequestPtr &req, Callback callback, size_t userID) {
    LOG_DEBUG << "user id: " << userID;

    Json::Value ret;

    auto customConfig = app().getCustomConfig();
    int paginationSize = customConfig.get("profile_pagination_size", 5).asInt();
    {
        auto clientPtr = app().getFastDbClient("default");

        /// top replies
        clientPtr->execSqlAsync(
            "select id, title, description, tag_ids from topics "
            "where posted_by = $1 "
            "and op_id is null "
            "order by likes desc limit " + std::to_string(paginationSize), 
            [=] (const Result &r) mutable {
                LOG_DEBUG << "in top replies";

                ret["top_replies"] = Json::arrayValue;
                for (const auto& row : r) {
                    Json::Value json;
                    json["id"] = row["id"].as<std::string>();
                    json["title"] = row["title"].as<std::string>();
                    json["description"] = row["description"].as<std::string>();
                    
                    json["tag_ids"] = Json::arrayValue;
                    
                    for (const auto& tag : row["tag_ids"].asArray<std::string>()) {
                        /// tag is a std::shared_ptr. We want to append 
                        /// the value not the pointer. So deref tag
                        json["tag_ids"].append(*tag);
                    }
                    ret["top_replies"].append(json);
                }

                /// top topics
                clientPtr->execSqlAsync(
                    "select id, title, description, tag_ids from topics "
                    "where op_id = $1 "
                    "order by likes desc limit " + std::to_string(paginationSize), 
                    [=] (const Result &r) mutable {
                        LOG_DEBUG << "in top topics";

                        ret["top_topics"] = Json::arrayValue;
                        for (const auto& row : r) {
                            Json::Value json;
                            json["id"] = row["id"].as<std::string>();
                            json["title"] = row["title"].as<std::string>();
                            json["tag_ids"] = Json::arrayValue;
                            
                            for (const auto& tag : row["tag_ids"].asArray<std::string>()) {
                                /// tag is a std::shared_ptr. We want to append 
                                /// the value not the pointer. So deref tag
                                json["tag_ids"].append(*tag);
                            }
                            ret["top_topics"].append(json);
                        }

                        callback(jsonResponse(std::move(ret)));
                    },
                    [=](const DrogonDbException &e) mutable {
                        LOG_DEBUG << e.base().what();
                        ret["error"] = (std::string)e.base().what();
                        callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
                    },
                    userID
                );
            },
            [=](const DrogonDbException &e) mutable {
                LOG_DEBUG << e.base().what();
                ret["error"] = (std::string)e.base().what();
                callback(HttpResponse::newHttpJsonResponse(std::move(ret)));
            },
            userID
        );
    }
}