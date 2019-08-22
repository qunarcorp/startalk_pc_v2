//
// Created by may on 2018/5/4.
//

#ifndef WIDGET_JID_H
#define WIDGET_JID_H

#include <string>
#include "../qtutil_global.h"

using namespace std;

namespace QTalk {
    namespace Entity {

        class QTALK_UTIL_EXPORT JID {

        public:
            JID(const char *user, const std::string &clientVersion = "",  const std::string &platformStr = "");

            static JID *jidWithString(std::string *str);

            JID(const std::string *jid, const std::string &clientVersion = "",  const std::string &platformStr = "");


            JID(std::string *node, std::string *domain, std::string *resource, const std::string &clientVersion = "",  const std::string &platformStr = "");

        public:
            std::string fullname();

            std::string barename();

            string &username();

            string &resources();

            std::string &domainname();

        private:
            void init(std::string *node, std::string *domain, std::string *resource);

            void innerParseUser(const char *input, const string &clientVersion, const string &platformStr);

        private:
            std::string node;
            std::string domain;
            std::string resource;


        };
    }

}


#endif //WIDGET_JID_H
