//
// Created by may on 2019-01-24.
//

#ifndef QTALK_V2_CJSON_INC_H
#define QTALK_V2_CJSON_INC_H

#include "cJSON.h"
#include <stdexcept>
#include <sstream>
#include <vector>
#include "../../Utils/Log.h"
#include "../../Utils/utils.h"
#include "../../qtutil_global.h"


//
// 是否允许key 重复
#ifndef CJSON_DUPLICATE_KEY_NOT_ALLOWED
#define CJSON_DUPLICATE_KEY_NOT_ALLOWED   1
#endif

//
//  key的大小写敏感是否打开
#ifndef CJSON_KEY_CASE_SENSITIVE
#define CJSON_KEY_CASE_SENSITIVE   1
#endif


namespace QTalk {
    namespace JSON {

        class cJSONObject {
        public:
            explicit cJSONObject() {
                object = cJSON_CreateObject();
            }

            ~cJSONObject() {
                cJSON_free(object);
            }

            cJSON *getcJSONObject() { return object; }


        public:
            void addObject(std::string &key, cJSONObject *object) {
                cJSON_AddItemToObject(this->object, key.c_str(), object->object);
                allObject.push_back(object->object);
            }

            void addString(std::string &key, const char *str) {
                cJSON *s = cJSON_CreateString(str);
                if (s != nullptr) {
                    cJSON_AddItemToObject(object, key.c_str(), s);
                    allObject.push_back(s);
                }
            }

            void addDouble(std::string &key, double value) {
                cJSON *s = cJSON_CreateNumber(value);
                if (s != nullptr) {
                    cJSON_AddItemToObject(object, key.c_str(), s);
                    allObject.push_back(s);
                }
            }

        private:
            cJSON *object;

            std::vector<cJSON *> allObject;
        };

        class cJSONArray {

        public:
            explicit cJSONArray() {
                objects = cJSON_CreateArray();
            }

            ~cJSONArray() {
                cJSON_free(objects);
            }

        public:
            void addcJSONObject(cJSONObject *object) {
                cJSON_AddItemToArray(objects, object->getcJSONObject());
            }

        private:
            cJSON *objects;
        };


        inline CJSON_PUBLIC(std::string) cJSON_to_string(const cJSON *object) {
            if (object == nullptr)
                return "";
            char *p = cJSON_PrintUnformatted(object);
            if (p) {
                std::string s(p);
                free(p);
                return s;
            }
            return "";
        }

        class QTALK_UTIL_EXPORT exception : public std::runtime_error {
        public:
            explicit exception(const char *errstring);
        };

        inline CJSON_PUBLIC(void) cJSON_addStringKey(cJSON *object, const char *key, const char *value) {
            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addStringKey.\n%s",
                                             object->string).c_str());
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            if (value == nullptr)
                cJSON_AddNullToObject(object, key);

            cJSON_AddStringToObject(object, key, value);
        }

        inline CJSON_PUBLIC(void) cJSON_addIntKey(cJSON *object, const char *key, int value) {

            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addIntKey.\n%s",
                                             object->string).c_str());
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddNumberToObject(object, key, value);

//            cJSON *insertObject = cJSON_CreateNumber(value);
//            cJSON_AddItemToObject(object, key, insertObject);
        }

        inline CJSON_PUBLIC(void) cJSON_addLonglongKey(cJSON *object, const char *key, long long value) {

            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addLonglongKey.\n%s",
                                             object->string).c_str());
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddNumberToObject(object, key, value);
//            cJSON *insertObject = cJSON_CreateNumber(value);
//            cJSON_AddItemToObject(object, key, insertObject);
        }

        inline CJSON_PUBLIC(void) cJSON_addDoubleKey(cJSON *object, const char *key, double value) {
            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addDoubleKey.\n%s",
                                             object->string).c_str());

#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddNumberToObject(object, key, value);
//            cJSON *insertObject = cJSON_CreateNumber(value);
//            cJSON_AddItemToObject(object, key, insertObject);
        }

        inline CJSON_PUBLIC(void) cJSON_addBooleanKey(cJSON *object, const char *key, bool value) {

            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addBooleanKey.\n%s",
                                             object->string).c_str());
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddBoolToObject(object, key, value);
//
//            cJSON *insertObject = cJSON_CreateBool(value);
//            cJSON_AddItemToObject(object, key, insertObject);
        }

        inline CJSON_PUBLIC(void) cJSON_addJsonObject(cJSON *object, const char *key, cJSON *value) {

            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addJsonObject.\n%s",
                                             object->string).c_str());

            if (value == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: insert a null value to key(%s) on \n%s", key,
                                             object->string).c_str());

            cJSON *destItem = value;

            if (object == value) {
                destItem = cJSON_Duplicate(object, true);
            }
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddItemToObject(object, key, destItem);
        }

        inline CJSON_PUBLIC(void) cJSON_addJsonArray(cJSON *object, const char *key, cJSON *value) {

            if (key == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: key is null when addJsonObject.\n%s",
                                             object->string).c_str());

            if (value == nullptr)
                throw QTalk::JSON::exception(
                        QTalk::utils::format("json error: insert a null value to key(%s) on \n%s", key,
                                             object->string).c_str());

            if (!cJSON_IsArray(value))
                throw QTalk::JSON::exception(
                        QTalk::utils::format(
                                "json error: your value is not a array when you add json array with key:%s.\n%s", key,
                                object->string).c_str());
#if CJSON_DUPLICATE_KEY_NOT_ALLOWED
            if (cJSON_HasObjectItem(object, key)) {
#if CJSON_KEY_CASE_SENSITIVE
                cJSON_DeleteItemFromObjectCaseSensitive(object, key);
#else
                cJSON_DeleteItemFromObject(object, key);
#endif

            }
#endif
            cJSON_AddItemToObject(object, key, value);
        }


        inline CJSON_PUBLIC(cJSON *)cJSON_GetObjectWithNumberItem(const cJSON *object, const char *key) {
            if (object == nullptr) {
                throw QTalk::JSON::exception("json object is null.");
            }

            std::string err = QTalk::JSON::cJSON_to_string(object);

            if (!cJSON_HasObjectItem(object, key)) {

                throw QTalk::JSON::exception(
                        QTalk::utils::format("JSON: %s\nkey %s is missing.", err.c_str(), key).c_str());
            }
            cJSON *result = cJSON_GetObjectItem(object, key);

            if (cJSON_IsNumber(result))
                return result;

            throw QTalk::JSON::exception(
                    QTalk::utils::format("JSON: %s\nkey %s is not a number.", err.c_str(), key).c_str());
        }

        inline CJSON_PUBLIC(cJSON *)cJSON_GetObjectWithStringItem(const cJSON *object, const char *key) {
            if (object == nullptr) {
                throw QTalk::JSON::exception("json object is null.");
            }

            std::string err = QTalk::JSON::cJSON_to_string(object);

            if (!cJSON_HasObjectItem(object, key)) {

                throw QTalk::JSON::exception(
                        QTalk::utils::format("JSON: %s\nkey %s is missing.", err.c_str(), key).c_str());
            }
            cJSON *result = cJSON_GetObjectItem(object, key);

            if (cJSON_IsString(result))
                return result;

            throw QTalk::JSON::exception(
                    QTalk::utils::format("JSON: %s\nkey %s is not a string.", err.c_str(), key).c_str());
        }

        inline CJSON_PUBLIC(cJSON *)cJSON_GetObjectWithBooleanItem(const cJSON *object, const char *key) {
            if (object == nullptr) {
                throw QTalk::JSON::exception("json object is null.");
            }

            std::string err = QTalk::JSON::cJSON_to_string(object);

            if (!cJSON_HasObjectItem(object, key)) {
                throw QTalk::JSON::exception(
                        QTalk::utils::format("JSON: %s\nkey %s is missing.", err.c_str(), key).c_str());
            }
            cJSON *result = cJSON_GetObjectItem(object, key);

            if (cJSON_IsBool(result))
                return result;
            throw QTalk::JSON::exception(
                    QTalk::utils::format("JSON: %s\nkey %s is not a boolean.", err.c_str(), key).c_str());
        }


        inline CJSON_PUBLIC(cJSON *)cJSON_GetObjectWithArrayItem(const cJSON *object, const char *key) {
            if (object == nullptr) {
                throw QTalk::JSON::exception("json object is null.");
            }

            std::string err = QTalk::JSON::cJSON_to_string(object);

            if (!cJSON_HasObjectItem(object, key)) {
                throw QTalk::JSON::exception(
                        QTalk::utils::format("JSON: %s\nkey %s is missing.", err.c_str(), key).c_str());
            }
            cJSON *result = cJSON_GetObjectItem(object, key);

            if (cJSON_IsArray(result))
                return result;
            throw QTalk::JSON::exception(
                    QTalk::utils::format("JSON: %s\nkey %s is not a array.", err.c_str(), key).c_str());
        }


        inline CJSON_PUBLIC(cJSON_bool) cJSON_SafeGetBoolValue(const cJSON *object, const char *key,
                                                               bool defaultValue = false) {

            try {
                cJSON *json = cJSON_GetObjectWithBooleanItem(object, key);
                return json->valueint;
            } catch (std::exception &exception) {

                error_log(exception.what());
            }
            return defaultValue;
        }

        inline CJSON_PUBLIC(int) cJSON_SafeGetIntValue(const cJSON *object, const char *key,
                                                       int defaultValue = 0) {

            try {
                cJSON *json = cJSON_GetObjectWithNumberItem(object, key);
                return json->valueint;
            } catch (std::exception &exception) {

                error_log(exception.what());
            }
            return defaultValue;
        }

        inline CJSON_PUBLIC(long long) cJSON_SafeGetLonglongValue(const cJSON *object, const char *key,
                                                                  long long defaultValue = 0) {
            try {
                cJSON *json = cJSON_GetObjectWithNumberItem(object, key);
                return json->valuellong;
            } catch (std::exception &exception) {

                error_log(exception.what());
            }
            return defaultValue;
        }

        inline CJSON_PUBLIC(double) cJSON_SafeGetDoubleValue(const cJSON *object, const char *key,
                                                             double defaultValue = 0) {
            try {
                cJSON *json = cJSON_GetObjectWithNumberItem(object, key);
                return json->valuedouble;
            } catch (std::exception &exception) {

                error_log(exception.what());
            }
            return defaultValue;
        }

        inline CJSON_PUBLIC(const char*)cJSON_SafeGetStringValue(const cJSON *object, const char *key,
                                                                 const char *defaultValue = "") {
            try {
                cJSON *json = cJSON_GetObjectWithStringItem(object, key);
                return json->valuestring;
            } catch (std::exception &exception) {

                error_log("error is {0} \n key is {1}", exception.what(), key);
            }
            return defaultValue;
        }
    };
};


#endif //QTALK_V2_CJSON_INC_H
