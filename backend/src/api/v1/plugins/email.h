/**
 *
 *  SMTPMail.h(name changed)
 *
 * This plugin is for SMTP mail delievery for the Drogon web-framework. Implementation
 * reference from the project "SMTPClient" with Qt5 by kelvins. Please check out
 * https://github.com/kelvins/SMTPClient. 
 
Copyright 2020 ihmc3jn09hk
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <drogon/plugins/Plugin.h>

class SMTPMail : public drogon::Plugin<SMTPMail>
{
  public:
    SMTPMail() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    virtual void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    virtual void shutdown() override;

    /** Send an email
    * return : An ID of the email.
    */
    std::string sendEmail( const std::string &mailServer,
                    const uint16_t &port,
                    const std::string &from,
                    const std::string &to,
                    const std::string &subject,
                    const std::string &content,
                    const std::string &user,
                    const std::string &passwd,
                    const std::function<void(const std::string&)> &cb = {}
                    );

    /**
     * @brief Send Email using the parameters from config instead 
     * of manually specifying all parameters
     * 
     * @param to Email address to send the mail to
     * @param subject Subject of the email
     * @param content Content of email
     * @param config Config to use. Should contain smtp_server, 
     * admin_email, admin_username, password and (optionally) 
     * smpt_port. If smtp_port is not specified, uses port 25
     * @param cb 
     * @return std::string 
     */
    std::string sendEMail(const std::string &to, 
                          const std::string &subject, 
                          const std::string &content,
                          const Json::Value config,
                          const std::function<void(const std::string&)> &cb = {}) {
        return sendEmail(
            config.get("smtp_server", "").asString(),
            config.get("smtp_port", 587).asInt(),
            config.get("admin_email", "").asString(),
            to,
            subject,
            content,
            config.get("admin_username", "").asString(),
            config.get("password", "").asString(), 
            cb
          );
    }
};