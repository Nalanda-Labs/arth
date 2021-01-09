#include <drogon/drogon.h>

using namespace drogon;

int main() {
    //app().registerHandler("/api/v1/register", &api::v1::registration::doRegister);
    app().loadConfigFile("../config.json");
    app().run();    
}
