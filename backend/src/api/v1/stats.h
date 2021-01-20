#pragma once
#include <drogon/HttpController.h>

#include "util/arth.h"

using namespace drogon;

namespace api::v1 {
class Stats:public drogon::HttpController<Stats>
{
  public:    
    METHOD_LIST_BEGIN            
	ADD_METHOD_TO(Stats::summary, "/api/v1/stats/summary/{user_id}", Get);
    METHOD_LIST_END

	void summary(const HttpRequestPtr &req, Callback callback, size_t userID);
};
}//n