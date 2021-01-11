#ifndef _HASHPASSWORD_H_
#define _HASHPASSWORD_H_

#include <string>
#include <argon2.h>
#include <bsd/stdlib.h> // for arc4random_buf
#include <tuple>

class PasswordUtils {
public:
	/**
	 * This function takes as password and performs a hash.
	 */
	static inline std::tuple<std::string, std::string> hashPassword(const std::string &password, const std::string &stored_salt) {
		const size_t hashlen = 64;
		const size_t saltlen = 16;
		uint8_t hash1[hashlen];
    	uint8_t hash2[hashlen];
    	uint8_t salt[saltlen];

    	if (stored_salt == "") {    		
    		arc4random_buf((void*)salt, saltlen); // it takes a void* so we cast it
    	}
	}


	static inline bool verifyPassword(const std::string &password, const std::string &hash, const std::string &stored_salt) {
		return true;
	}
};

#endif