//
// Created by may on 2018/5/5.
//

#ifndef WIDGET_DATABASE_H
#define WIDGET_DATABASE_H

#include <string>
#include <stdexcept>
#include "sqlite3.h"
#include <map>

#pragma once

namespace qtalk {

    namespace sqlite {

        extern const int OPEN_FLAGS_DEFAULT;

        const char *getLibVersion() noexcept;

        int getLibVersionNumber() noexcept;

        class exception : public std::runtime_error {
        public:
            exception(sqlite3 *sqlite, int ret);

            exception(const char *errMessage);

            exception(const sqlite3 *sqlite, std::string &str);

            const char *getErrorStr() const noexcept;

        private:
            int _errcode;         ///< Error code value
            int _extendedErrcode; ///< Detailed error code if any
        };

        inline void check(const int aRet, sqlite3 *sqlite) {
            if (SQLITE_OK != aRet) {
                std::string errmessage = sqlite3_errmsg(sqlite);
                throw exception(errmessage.c_str());
            }
        }

        class column;

        class statement;

        class database {
            friend statement;

        public:
            database(const std::string &filename,
                     const int flags = OPEN_FLAGS_DEFAULT,
                     const int timeoutMs = 0,
                     const std::string &zVfs = "");

            ~database();

            bool tableExists(const char *tableName);

            inline bool tableExists(const std::string &aTableName) {
                return tableExists(aTableName.c_str());
            }

            int exec(const char *query);

            void setBusyTimeout(const int timeoutMs);

            long long getLastInsertRowid() const noexcept;

            int getTotalChanges() const noexcept;

            int getErrorCode() const noexcept;

            int getExtendedErrorCode() const noexcept;

            const char *getErrorMsg() const noexcept;

            const std::string &getFilename() const noexcept {
                return _filename;
            }

        private:
            sqlite3 *_sqlite;
            std::string _filename;
        };

        class statement {

            friend column;
        public:
            statement(database &database, const char *apQuery);

            statement(database &aDatabase, const std::string &aQuery);

            ~statement();

            void clearBindings();

            void resetBindings();

            bool executeStep();

            void bind(const int index, const int value);

            void bind(const int index, const unsigned value);

#if (LONG_MAX == INT_MAX)

            void bind(const int index, const long value) {
                bind(index, static_cast<int>(value));
            }

#else

            void bind(const int index, const long value) {
                bind(index, static_cast<long long>(value));
            }

#endif

            void bind(const int index, const long long value);

            void bind(const int index, const double value);

            void bind(const int index, const std::string &value);

            void bind(const int index, const char *value);

            void bind(const int index, const void *value, const int size);

            void bindNoCopy(const int index, const std::string &value);

            void bindNoCopy(const int index, const char *value);

            void bindNoCopy(const int index, const void *value, const int size);

            void bind(const int index);

            void bind(const char *apName, const int value);

            void bind(const char *apName, const unsigned value);

            void bind(const char *apName, const long long aValue);

            void bind(const char *apName, const double aValue);

            void bind(const char *apName, const std::string &aValue);

            void bind(const char *apName, const char *apValue);

            void bind(const char *apName, const void *apValue, const int aSize);

            void bindNoCopy(const char *apName, const std::string &aValue);

            void bindNoCopy(const char *apName, const char *apValue);

            void bindNoCopy(const char *apName, const void *apValue, const int aSize);

            void bind(const char *apName); // bind NULL value


            inline void bind(const std::string &aName, const int aValue) {
                bind(aName.c_str(), aValue);
            }

            inline void bind(const std::string &aName, const unsigned aValue) {
                bind(aName.c_str(), aValue);
            }

            inline void bind(const std::string &aName, const long long aValue) {
                bind(aName.c_str(), aValue);
            }

            inline void bind(const std::string &aName, const double aValue) {
                bind(aName.c_str(), aValue);
            }

            inline void bind(const std::string &aName, const std::string &aValue) {
                bind(aName.c_str(), aValue);
            }

            inline void bind(const std::string &aName, const char *apValue) {
                bind(aName.c_str(), apValue);
            }

            inline void bind(const std::string &aName, const void *apValue, const int aSize) {
                bind(aName.c_str(), apValue, aSize);
            }

            inline void bindNoCopy(const std::string &aName, const std::string &aValue) {
                bindNoCopy(aName.c_str(), aValue);
            }

            inline void bindNoCopy(const std::string &aName, const char *apValue) {
                bindNoCopy(aName.c_str(), apValue);
            }

            inline void bindNoCopy(const std::string &aName, const void *apValue, const int aSize) {
                bindNoCopy(aName.c_str(), apValue, aSize);
            }

            inline void bind(const std::string &aName) // bind NULL value
            {
                bind(aName.c_str());
            }

            bool executeNext();

            long long executeAffectedLines();


            column getColumn(const int index);

            inline void checkIndex(const int aIndex) const {
                if ((aIndex < 0) || (aIndex >= mColumnCount)) {
                    throw qtalk::sqlite::exception("Column index out of range.");
                }
            }

            inline void checkRow() const {
                if (false == mbHasRow) {
                    throw qtalk::sqlite::exception(
                            "No row to get a column from. executeNext() was not called, or returned false.");
                }
            }


        private:

            class Ptr {
            public:
                // Prepare the statement and initialize its reference counter
                Ptr(sqlite3 *apSQLite, std::string &aQuery);

                // Copy constructor increments the ref counter
                Ptr(const Ptr &aPtr);

                // Decrement the ref counter and finalize the sqlite3_stmt when it reaches 0
                ~Ptr();

                /// Inline cast operator returning the pointer to SQLite Database Connection Handle
                inline operator sqlite3 *() const {
                    return mpSQLite;
                }

                /// Inline cast operator returning the pointer to SQLite Statement Object
                inline operator sqlite3_stmt *() const {
                    return mpStmt;
                }

            private:
                Ptr &operator=(const Ptr &aPtr);

            private:
                sqlite3 *mpSQLite;    //!< Pointer to SQLite Database Connection Handle
                sqlite3_stmt *mpStmt;      //!< Pointer to SQLite Statement Object
                unsigned int *mpRefCount;  //!< Pointer to the heap allocated reference counter of the sqlite3_stmt
                //!< (to share it with Column objects)
            };

            std::string _query;         //!< UTF-8 SQL Query
            sqlite3 *_sqlite;           // sqlite instance
            Ptr stmt;       //!< Shared Pointer to the prepared SQLite Statement Object
            int mColumnCount;   //!< Number of columns in the result of the prepared statement
            mutable std::map<std::string, int> mColumnNames;   //!< Map of columns index by name (mutable so getColumnIndex can be const)
            bool mbHasRow;           //!< true when a row has been fetched with executeStep()
            bool mbDone; //!< true when the last executeStep() had no more row to fetch
            int exec();

            int tryExecuteNext() noexcept;
        };

        class column {
        public:
            column(statement::Ptr &stmt, int index) noexcept;

            ~column();

            const char *getName() const noexcept; // nothrow

#ifdef SQLITE_ENABLE_COLUMN_METADATA
            /**
     * @brief Return a pointer to the table column name that is the origin of this result column
     *
     *  Require definition of the SQLITE_ENABLE_COLUMN_METADATA preprocessor macro :
     * - when building the SQLite library itself (which is the case for the Debian libsqlite3 binary for instance),
     * - and also when compiling this wrapper.
     */
    const char* getOriginName() const noexcept; // nothrow
#endif

            /// Return the integer value of the column.
            int getInt() const noexcept; // nothrow
            /// Return the 32bits unsigned integer value of the column (note that SQLite3 does not support unsigned 64bits).
            unsigned getUInt() const noexcept; // nothrow
            /// Return the 64bits integer value of the column (note that SQLite3 does not support unsigned 64bits).
            long long getInt64() const noexcept; // nothrow
            /// Return the double (64bits float) value of the column
            double getDouble() const noexcept; // nothrow
            /**
             * @brief Return a pointer to the text value (NULL terminated string) of the column.
             *
             * @warning The value pointed at is only valid while the statement is valid (ie. not finalized),
             *          thus you must copy it before using it beyond its scope (to a std::string for instance).
             */
            const char *getText(const char *apDefaultValue = "") const noexcept; // nothrow
            /**
             * @brief Return a pointer to the binary blob value of the column.
             *
             * @warning The value pointed at is only valid while the statement is valid (ie. not finalized),
             *          thus you must copy it before using it beyond its scope (to a std::string for instance).
             */
            const void *getBlob() const noexcept; // nothrow
            /**
             * @brief Return a std::string for a TEXT or BLOB column.
             *
             * Note this correctly handles strings that contain null bytes.
             */
            std::string getString() const;

            /**
             * @brief Return the type of the value of the column
             *
             * Return either SQLite::INTEGER, SQLite::FLOAT, SQLite::TEXT, SQLite::BLOB, or SQLite::Null.
             *
             * @warning After a type conversion (by a call to a getXxx on a Column of a Yyy type),
             *          the value returned by sqlite3_column_type() is undefined.
             */
            int getType() const noexcept; // nothrow

            /// Test if the column is an integer type value (meaningful only before any conversion)
            inline bool isInteger() const noexcept // nothrow
            {
                return (SQLITE_INTEGER == getType());
            }

            /// Test if the column is a floating point type value (meaningful only before any conversion)
            inline bool isFloat() const noexcept // nothrow
            {
                return (SQLITE_FLOAT == getType());
            }

            /// Test if the column is a text type value (meaningful only before any conversion)
            inline bool isText() const noexcept // nothrow
            {
                return (SQLITE_TEXT == getType());
            }

            /// Test if the column is a binary blob type value (meaningful only before any conversion)
            inline bool isBlob() const noexcept // nothrow
            {
                return (SQLITE_BLOB == getType());
            }

            /// Test if the column is NULL (meaningful only before any conversion)
            inline bool isNull() const noexcept // nothrow
            {
                return (SQLITE_NULL == getType());
            }

            /**
             * @brief Return the number of bytes used by the text (or blob) value of the column
             *
             * Return either :
             * - size in bytes (not in characters) of the string returned by getText() without the '\0' terminator
             * - size in bytes of the string representation of the numerical value (integer or double)
             * - size in bytes of the binary blob returned by getBlob()
             * - 0 for a NULL value
             */
            int getBytes() const noexcept;

            /// Alias returning the number of bytes used by the text (or blob) value of the column
            inline int size() const noexcept {
                return getBytes();
            }

            /// Inline cast operator to int
            inline operator int() const {
                return getInt();
            }

            /// Inline cast operator to 32bits unsigned integer
            inline operator unsigned int() const {
                return getUInt();
            }

#if (LONG_MAX == INT_MAX) // sizeof(long)==4 means the data model of the system is ILP32 (32bits OS or Windows 64bits)

            /// Inline cast operator to 32bits long
            inline operator long() const {
                return getInt();
            }

            /// Inline cast operator to 32bits unsigned long
            inline operator unsigned long() const {
                return getUInt();
            }

#else // sizeof(long)==8 means the data model of the system is LLP64 (64bits Linux)

            /// Inline cast operator to 64bits long when the data model of the system is ILP64 (Linux 64 bits...)
            inline operator long() const {
                return getInt64();
            }

#endif

            /// Inline cast operator to 64bits integer
            inline operator long long() const {
                return getInt64();
            }

            /// Inline cast operator to double
            inline operator double() const {
                return getDouble();
            }

            /**
             * @brief Inline cast operator to char*
             *
             * @see getText
             */
            inline operator const char *() const {
                return getText();
            }

            /**
             * @brief Inline cast operator to void*
             *
             * @see getBlob
             */
            inline operator const void *() const {
                return getBlob();
            }

#if !(defined(_MSC_VER) && _MSC_VER < 1900)
            // NOTE : the following is required by GCC and Clang to cast a Column result in a std::string
            // (error: conversion from ‘SQLite::Column’ to non-scalar type ‘std::string {aka std::basic_string<char>}’)
            // but is not working under Microsoft Visual Studio 2010, 2012 and 2013
            // (error C2440: 'initializing' : cannot convert from 'SQLite::Column' to 'std::basic_string<_Elem,_Traits,_Ax>'
            //  [...] constructor overload resolution was ambiguous)
            /**
             * @brief Inline cast operator to std::string
             *
             * Handles BLOB or TEXT, which may contain null bytes within
             *
             * @see getString
             */
            inline operator std::string() const {
                return getString();
            }

#endif

        private:
            statement::Ptr mStmtPtr;   ///< Shared Pointer to the prepared SQLite Statement Object
            int mIndex; ///< Index of the column in the row of result, starting at 0
        };

        /**
 * @brief Standard std::ostream text inserter
 *
 * Insert the text value of the Column object, using getText(), into the provided stream.
 *
 * @param[in] aStream   Stream to use
 * @param[in] aColumn   Column object to insert into the provided stream
 *
 * @return  Reference to the stream used
 */
        std::ostream &operator<<(std::ostream &aStream, const column &aColumn);

//#if __cplusplus >= 201402L || (defined(_MSC_VER) && _MSC_VER >= 1900)
//
//        // Create an instance of T from the first N columns, see declaration in Statement.h for full details
//template<typename T, int N>
////T Statement::getColumns()
////{
////    checkRow();
////    checkIndex(N - 1);
////    return getColumns<T>(std::make_integer_sequence<int, N>{});
////}
////
////// Helper function called by getColums<typename T, int N>
////template<typename T, const int... Is>
////T Statement::getColumns(const std::integer_sequence<int, Is...>)
////{
////    return T{Column(mStmtPtr, Is)...};
////}
//
//#endif
    }
}


#endif //WIDGET_DATABASE_H
