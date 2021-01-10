#include <string>

inline std::string hash_password(const std::string &password) {
  return password;
}


inline bool verify_password(const std::string &password, const std::string &hash) {
  return hash_password(password) == hash;
}
