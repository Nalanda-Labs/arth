#include <exception>
#include <jwt-cpp/jwt.h>
#include <optional>

struct Token {
    size_t userID;
    std::string username;

    Token(size_t userID, std::string username)
        : userID(userID), username(username) {}
};

inline std::optional<Token> verifyJWT(const std::string &token,
                                      const std::string &secret,
                                      const std::string issuer = "arth") {

  auto verifier = jwt::verify()
                      .allow_algorithm(jwt::algorithm::hs256{secret})
                      .with_issuer(issuer);  

	try {
		/// throws exception if token is invalid
		auto decoded = jwt::decode(token);

		/// throws exception if verification fails
		verifier.verify(decoded);

        size_t user_id = decoded.get_payload_claim("user_id").as_int();
        std::string username = decoded.get_payload_claim("username").as_string();

        return Token(user_id, username);
	} catch(std::exception &e) {
		return {};
	}  
}

inline std::string signJWT(const size_t user_id, 
                           const std::string &username,
                           const std::string &secret,
                           const std::string issuer = "arth") {

  return jwt::create()
      .set_issuer(issuer)
      .set_type("JWT")
      .set_payload_claim("user_id",
                         jwt::claim(picojson::value(int64_t(user_id))))
      .set_payload_claim("username", jwt::claim(username))
      .sign(jwt::algorithm::hs256{secret});
}
