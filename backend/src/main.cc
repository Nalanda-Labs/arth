#include <drogon/drogon.h>

#include "api/v1/login.h"
#include "api/v1/register.h"

using namespace drogon;

int main() {
    std::cout << banner << std::endl;
    app().registerHandler("/api/v1/register", &api::v1::registration::doRegister);
    app().loadConfigFile("../config.json");
    app().run();    
}
