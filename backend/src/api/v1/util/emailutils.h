#ifndef _EMAILUTILS_H_
#define _EMAILUTILS_H_

#include <string>
#include <algorithm>

class EmailUtils {
public:
	/**
	 * This function removes characters after + in username part and makes it lowercase.
	 * NOTE: This is not validation stuff. We rely on actual email landing in mailbox
	 * because email validation is quite painful with unicode letter in it and domain.
	 * If email is invalid it becomes an empty string.
	 */
	static void inline cleanEmail(std::string& email) {
		if (email.empty()) {
			return;
		}

		std::string delimiter = "@";
		size_t pos = 0;
		std::string username;
		
		int i = 0;

		while ((pos = email.find(delimiter)) != std::string::npos) {
			// it is supposed to have only one @
			if(i > 0) {
				email = "";
				return;
			}
    		username = email.substr(0, pos);
    		email.erase(0, pos + delimiter.length());
    		i++;
		}

		/// if @ is not found, the email is not valid anyway
		if (username.empty()) {
			return;
		}

		// email contains domain part now
		std::string domain = std::move(email);
		// now username and domain are separate we remove everything after +
		delimiter = '+';

		if((pos = username.find(delimiter)) != std::string::npos)
    		username = username.substr(0, pos);

    	// ensure all is lowercase
    	email = username + '@' + domain;
    	transform(email.begin(), email.end(), email.begin(), ::tolower);
	}
};

#endif