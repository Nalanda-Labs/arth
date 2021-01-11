#ifndef _HASHPASSWORD_H_
#define _HASHPASSWORD_H_

#include <string>
#include <argon2.h>
#include <bsd/stdlib.h> // for arc4random_buf
#include <tuple>
#include <sstream>
#include <iostream>
#include "base64.h"

class PasswordUtils
{
public:
    /**
	 * This function takes as password and performs a hash.
	 */
    static inline std::tuple<std::string, std::string> hashPassword(const std::string &password, const std::string &stored_salt)
    {
        const size_t hashlen = 64;
        const size_t saltlen = 16;
        const size_t encoded_len = 256;
        const uint32_t iterations = 4;   //no.of iterations
        const uint32_t memory = 1 << 18; // 256M
        const uint32_t threads = 1;
        char encoded_password[encoded_len];
        std::string encoded_salt = "";

        uint8_t hash1[hashlen];
        uint8_t hash2[hashlen];
        uint8_t salt[saltlen];

        std::string ec;
        if (stored_salt == "")
        {
            arc4random_buf((void *)salt, saltlen); // it takes a void* so we cast it
            std::ostringstream convert;
            for (int i = 0; i < saltlen; i++)
            {
                convert << (int)salt[i];
            }

            ec = convert.str();
        }
        else
        {
            ec = stored_salt;
        }

        int ret = argon2i_hash_encoded(iterations, memory, threads, (const void *)password.c_str(),
                                       password.length(), ec.c_str(), ec.length(), hashlen, encoded_password, encoded_len);

        if (ret == (int)ARGON2_OK)
            return std::make_tuple(std::move(std::string(encoded_password)), std::move(ec));
        else
            return std::make_tuple("", "");
    }

    static inline bool verifyPassword(const std::string &password, const std::string &hash, const std::string &stored_salt)
    {
        auto [password_hash, _] = hashPassword(password, stored_salt);

        if (hash == password_hash)
            return true;

        return false;
    }
};

#endif