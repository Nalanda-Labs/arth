#include <drogon/drogon.h>

using namespace drogon;

int main() {
    std::cout << banner << std::endl;
    // app().addListener("::1", 8848); //ipv6
    app().addListener("0.0.0.0", 8000);
    if (app().supportSSL())
    {
        drogon::app()
            .setSSLFiles("server.pem", "server.pem")
            .addListener("0.0.0.0", 8849, true);
    }
    app().run();
}