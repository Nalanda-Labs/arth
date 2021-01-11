#ifndef _HASHPASSWORD_H_
#define _HASHPASSWORD_H_

#include <string>
#include <argon2.h>
#include <bsd/stdlib.h> // for arc4random_buf
#include <tuple>

#include "base64.h"

class PasswordUtils {
public:
	/**
	 * This function takes as password and performs a hash.
	 */
	static inline std::tuple<std::string, std::string> hashPassword(const std::string &password, const std::string &stored_salt) {
		const size_t hashlen = 64;
		const size_t saltlen = 16;
		const size_t encoded_len = 256;
		const uint32_t iterations = 4; //no.of iterations
		const uint32_t memory = 1<<18; // 256M
		const uint32_t threads = 1;
		char encoded_password[encoded_len];
		std::string encoded_salt = "";

		uint8_t hash1[hashlen];
    	uint8_t hash2[hashlen];
    	uint8_t salt[saltlen];

    	if (stored_salt == "") {    		
    		arc4random_buf((void*)salt, saltlen); // it takes a void* so we cast it
			std::ostringstream convert;
			for (int i = 0; i < saltlen; i++) {
				convert << (int)salt[i];
			}

			std::string ec = convert.str();
			encoded_salt = Base64::encode(ec);

    	} else {			
			std::string decoded_salt;
			Base64::decode(stored_salt, decoded_salt);
			if (decoded_salt == "") {
				return std::make_tuple("", "");
			} else {
				for(int i=0; i<saltlen; i++) {
					salt[i] = (u_int8_t)(decoded_salt[i]);
				}
			}
			encoded_salt = stored_salt;
		}

		// it is a cpu blocking operation so we launch it using std::async
		auto ret = std::async(argon2i_hash_encoded, iterations, memory, threads, (const void*)password.c_str(), 
							password.length(), salt, saltlen, hashlen, encoded_password, encoded_len);

		if(ret.get() == (int)ARGON2_OK)
			return std::make_tuple(std::move(std::string(encoded_password)), std::move(encoded_salt));
		else
			return std::make_tuple("", "");
	}


	static inline bool verifyPassword(const std::string &password, const std::string &hash, const std::string &stored_salt) {
		auto ret = hashPassword(password, stored_salt);
		if (hash == std::get<0>(ret))
			return true;
		
		return false;

	}
};

#endif