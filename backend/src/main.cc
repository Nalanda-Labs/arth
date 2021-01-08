#include <drogon/drogon.h>

using namespace drogon;

int main() {
    std::cout << banner << std::endl;
    app().loadConfigFile("config.json");
    app().run();
}