//
// Created by may on 2018/5/4.
//

#include <sstream>
#include <iostream>
#include <string.h>
#include <future>
#include <deque>
#include "JID.h"
#include "Utils/utils.h"

using namespace std;

namespace QTalk {
    namespace Entity {

        JID *JID::jidWithString(std::string *str) {
            JID *jid = new JID(str);
            return jid;
        }

        void JID::innerParseUser(const char *input, const std::string &clientVersion, const std::string &platformStr) {

            std::string jid(input);

            size_t pos = jid.find("@");
            if (pos > 0 && pos <= jid.size()) 
			{
                std::string node = jid.substr(0, pos);
                std::string remain = jid.substr(pos + 1, jid.size() - pos);

                pos = remain.find('/');
                std::string domain;
                std::string resource;

                if (!remain.empty()) 
				{
                    if (pos > 0 && pos <= remain.size()) 
					{
                        domain = remain.substr(0, pos);
                        resource = remain.substr(pos + 1, remain.size() - pos);
                    } 
					else
					{
						domain = remain;
						// resource todo
						char uuid[36] = {};
						memset(uuid, 0, strlen(uuid));
						QTalk::utils::generateUUID(uuid);
						
						std::ostringstream stream;
                        stream  << "V[" << clientVersion
                                << "]_P[" << platformStr
								<< "]_ID[" << uuid
								<< "]_PB";
						resource = stream.str();
					}
                    init(&node, &domain, &resource);
                } 
				else 
				{
                    throw "jid's parameter is illegal";
                }
            }
        }

        JID::JID(const std::string *jid, const string &clientVersion, const string &platformStr) {
            innerParseUser(jid->c_str(), clientVersion, platformStr);
        }


        void JID::init(string *node, string *domain, string *resource) {
            if (node == nullptr || domain == nullptr) {
                throw "jid's parameter is illegal";
            }

            this->domain = *domain;
            this->node = *node;
            this->resource = *resource;

        }

        JID::JID(string *node, string *domain, string *resource, const string &clientVersion, const string &platformStr) {
            init(node, domain, resource);
        }

        std::string JID::barename() {
            if(node.empty() || domain.empty())
                return std::string();
            std::stringstream ss;
            ss << node
               << "@"
               << domain;
            return ss.str();
        }

        std::string JID::fullname() {
            if (resource.empty())
                return barename();

            std::stringstream ss;
            ss << node
               << "@"
               << domain
               << "/"
               << resource;
            return ss.str();
        }

        std::string &JID::username() {
            return node;
        }

        std::string &JID::domainname() {
            return domain;
        }

        JID::JID(const char *user, const string &clientVersion, const string &platformStr) {
            innerParseUser(user, clientVersion, platformStr);
        }

        string &JID::resources() {
            return this->resource;
        }
    }
}
