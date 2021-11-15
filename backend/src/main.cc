#include <drogon/drogon.h>

using namespace drogon;

int main() {
	app().loadConfigFile("/home/shiv/arth/backend/config.json");
	auto customConfig = app().getCustomConfig();
	auto index_name = customConfig.get("es_index_name", "arth").asString();
	auto es_host = customConfig.get("es_host", "http://'localhost:9200").asString();
	// create the index by sending put request. we do not care for response as it
	// will work only first time and rest of time it will fail
	// we will use elasticsearch's rest API for connecting and querying it
	auto client = HttpClient::newHttpClient(es_host);
	auto req = HttpRequest::newHttpRequest();
	req->setMethod(drogon::Put);
	req->setPath("/" + index_name);
	client->sendRequest(
			req, [](ReqResult, const HttpResponsePtr &response)
			{
					LOG_INFO << "receive response!";
					// LOG_INFO << response->getBody();
			});
	app().run();
}
