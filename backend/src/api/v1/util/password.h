#ifndef _HASHPASSWORD_H_
#define _HASHPASSWORD_H_

inline std::string hashPassword(const std::string &password) {
	return password;
}


inline bool verifyPassword(const std::string &password, const std::string &hash) {
	return hashPassword(password) == hash;
}

#endif