#include <openssl/md5.h>
#include <iostream>
#include <sstream>
#include <iomanip>

inline const std::string generate_md5(const std::string &input)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)input.c_str(), input.length(), result);

    std::ostringstream sout;
    sout << std::hex << std::setfill('0');
    for (long long c : result)
    {
        sout << std::setw(2) << (long long)c;
    }
    return sout.str();
}