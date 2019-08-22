
//
// Created by may on 2018/5/5.
//

#include <iostream>
#include "database.h"
#include "../../QtUtil/Utils/Log.h"
//#include "../libTools/Timer.h"
#include <time.h>

#ifdef _WINDOWS
#include <windows.h>
#else

#include <sys/time.h>

#endif

#ifdef _WINDOWS

int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year   = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday   = wtm.wDay;
    tm.tm_hour   = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm. tm_isdst  = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

namespace qtalk {
    namespace sqlite {

        // Return SQLite version string using runtime call to the compiled library
        const char *getLibVersion() noexcept // nothrow
        {
            return sqlite3_libversion();
        }

// Return SQLite version number using runtime call to the compiled library
        int getLibVersionNumber() noexcept // nothrow
        {
            return sqlite3_libversion_number();
        }

        const int OPEN_FLAGS_DEFAULT = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

        database::database(const std::string &filename, const int flags, const int timeoutMs,
                           const std::string &zVfs) : _filename(filename), _sqlite(nullptr) {

            const int ret = sqlite3_open_v2(filename.c_str(), &_sqlite, flags,
                                            zVfs.empty() ? nullptr : zVfs.c_str());
            if (SQLITE_OK != ret) {
                const qtalk::sqlite::exception exception(_sqlite, ret);
                sqlite3_close(_sqlite);
                throw exception;
            }
            if (timeoutMs > 0) {
                setBusyTimeout(timeoutMs);
            }
        }

        database::~database() {
            const int ret = sqlite3_close(_sqlite);
            (void) ret;
            //assert(SQLITE_OK == ret);// "database is locked"
        }

        void database::setBusyTimeout(const int timeoutMs) {
            const int ret = sqlite3_busy_timeout(_sqlite, timeoutMs);
            check(ret, _sqlite);
        }

        bool database::tableExists(const char *apTableName) {
            statement query(*this, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?");
            query.bind(1, apTableName);
            (void) query.executeNext();
            return (1 == query.getColumn(0).getInt());
        }

// Get the rowid of the most recent successful INSERT into the database from the current connection.
        long long database::getLastInsertRowid() const noexcept // nothrow
        {
            return sqlite3_last_insert_rowid(_sqlite);
        }

// Get total number of rows modified by all INSERT, UPDATE or DELETE statement since connection.
        int database::getTotalChanges() const noexcept // nothrow
        {
            return sqlite3_total_changes(_sqlite);
        }

// Return the numeric result code for the most recent failed API call (if any).
        int database::getErrorCode() const noexcept // nothrow
        {
            return sqlite3_errcode(_sqlite);
        }

// Return the extended numeric result code for the most recent failed API call (if any).
        int database::getExtendedErrorCode() const noexcept // nothrow
        {
            return sqlite3_extended_errcode(_sqlite);
        }

// Return UTF-8 encoded English language explanation of the most recent failed API call (if any).
        const char *database::getErrorMsg() const noexcept // nothrow
        {
            return sqlite3_errmsg(_sqlite);
        }

        int database::exec(const char *query) {
            const int ret = sqlite3_exec(_sqlite, query, nullptr, nullptr, nullptr);
            check(ret, _sqlite);

            // Return the number of rows modified by those SQL statements (INSERT, UPDATE or DELETE only)
            return sqlite3_changes(_sqlite);
        }

        column statement::getColumn(const int index) {
            checkRow();
            checkIndex(index);

            // Share the Statement Object handle with the new Column created
            return column(stmt, index);
        }

        statement::statement(database &database, const char *query) :
                _query(query),
                stmt(database._sqlite, _query),
                mColumnCount(0),
                mbHasRow(false),
                mbDone(false) {
            mColumnCount = sqlite3_column_count(stmt);
        }

        statement::statement(database &database, const std::string &query) :
                _query(query),
                stmt(database._sqlite, _query),
                mColumnCount(0),
                mbHasRow(false),
                mbDone(false) {
            info_log(query);
            mColumnCount = sqlite3_column_count(stmt);
        }

        long long statement::executeAffectedLines() {
            bool result = executeStep();

            if (result)
                return sqlite3_changes(_sqlite);
            return 0;
        }

        bool statement::executeNext() {
            const int ret = tryExecuteNext();
            if ((SQLITE_ROW != ret) && (SQLITE_DONE != ret)) // on row or no (more) row ready, else it's a problem
            {
                auto msg = sqlite3_errmsg(_sqlite);

                throw sqlite::exception(msg);
//                throw sqlite::exception(stmt, ret);
            }

            return mbHasRow; // true only if one row is accessible by getColumn(N)
        }

        // Execute a one-step query with no expected result
        int statement::exec() {
            const int ret = tryExecuteNext();
            if (SQLITE_DONE != ret) // the statement has finished executing successfully
            {
                if (SQLITE_ROW == ret) {
                    throw sqlite::exception("exec() does not expect results. Use executeNext.");
                } else {
                    throw sqlite::exception(_sqlite, ret);
                }
            }

            // Return the number of rows modified by those SQL statements (INSERT, UPDATE or DELETE)
            return sqlite3_changes(stmt);
        }

        int statement::tryExecuteNext() noexcept {
            if (false == mbDone) {
                const int ret = sqlite3_step(stmt);
                if (SQLITE_ROW == ret) // one row is ready : call getColumn(N) to access it
                {
                    mbHasRow = true;
                } else if (SQLITE_DONE == ret) // no (more) row ready : the query has finished executing
                {
                    mbHasRow = false;
                    mbDone = true;
                } else {
                    mbHasRow = false;
                    mbDone = false;
                }

                return ret;
            } else {
                // Statement needs to be reseted !
                return SQLITE_MISUSE;
            }
        }

        void statement::resetBindings() {
            sqlite3_reset(stmt);
        }

        bool statement::executeStep() {
            return sqlite3_step(stmt) == SQLITE_DONE;
        }

        void statement::clearBindings() {
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
        }

        void statement::bind(const int aIndex, const int aValue) {
            const int ret = sqlite3_bind_int(stmt, aIndex, aValue);
            check(ret, stmt);
        }

// Bind a 32bits unsigned int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement
        void statement::bind(const int aIndex, const unsigned aValue) {
            const int ret = sqlite3_bind_int64(stmt, aIndex, aValue);
            check(ret, stmt);
        }

// Bind a 64bits int value to a parameter "?", "?NNN", ":VVV", "@VVV" or "$VVV" in the SQL prepared statement
        void statement::bind(const int aIndex, const long long aValue) {
            const int ret = sqlite3_bind_int64(stmt, aIndex, aValue);
            check(ret, stmt);
        }

        /**
          * @功能描述 添加string 类型重载实现

          * @date 2018.9.21
          */
        void statement::bind(const int index, const std::string &value) {
            bind(index, value.data());
        }

        statement::~statement() = default;

        void statement::bind(const int index, const char *value) {
            const int ret = sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
            check(ret, stmt);
        }

        /**
          * @功能描述 添加blob 类型重载实现

          * @date 2018.10.24
          */
        void statement::bind(const int index, const void *value, const int size) {
            const int ret = sqlite3_bind_blob(stmt, index, value, size, SQLITE_STATIC);
            check(ret, stmt);
        }

        exception::exception(const sqlite3 *sqlite, std::string &str) :
                std::runtime_error(str),
                _errcode(-1), // 0 would be SQLITE_OK, which doesn't make sense
                _extendedErrcode(-1) {
        }

        exception::exception(sqlite3 *sqlite, int ret) :
                std::runtime_error(sqlite3_errmsg(sqlite)),
                _errcode(ret),
                _extendedErrcode(sqlite3_extended_errcode(sqlite)) {
        }

        const char *exception::getErrorStr() const noexcept // nothrow
        {
            return sqlite3_errstr(_errcode);
        }

        exception::exception(const char *errMessage) :
                std::runtime_error(errMessage),
                _errcode(-1),
                _extendedErrcode(-1) {

        }

        column::column(statement::Ptr &stmt, int index) noexcept:
                mStmtPtr(stmt),
                mIndex(index) {

        }

        column::~column() {
        }

        const char *column::getName() const noexcept {
            return sqlite3_column_name(mStmtPtr, mIndex);;
        }

        int column::getInt() const noexcept {
            return sqlite3_column_int(mStmtPtr, mIndex);
        }

        unsigned column::getUInt() const noexcept {
            return static_cast<unsigned>(getInt64());
        }

        long long column::getInt64() const noexcept {
            return sqlite3_column_int64(mStmtPtr, mIndex);
        }

        double column::getDouble() const noexcept {
            return sqlite3_column_double(mStmtPtr, mIndex);
        }

        const char *column::getText(const char *apDefaultValue) const noexcept {
            const char *pText = reinterpret_cast<const char *>(sqlite3_column_text(mStmtPtr, mIndex));
            return (pText ? pText : apDefaultValue);
        }

        const void *column::getBlob() const noexcept {
            return sqlite3_column_blob(mStmtPtr, mIndex);
        }

        std::string column::getString() const {
            // Note: using sqlite3_column_blob and not sqlite3_column_text
            // - no need for sqlite3_column_text to add a \0 on the end, as we're getting the bytes length directly
            const char *data = static_cast<const char *>(sqlite3_column_blob(mStmtPtr, mIndex));

            // SQLite docs: "The safest policy is to invoke… sqlite3_column_blob() followed by sqlite3_column_bytes()"
            // Note: std::string is ok to pass nullptr as first arg, if length is 0
            return std::string(data, static_cast<unsigned long>(sqlite3_column_bytes(mStmtPtr, mIndex)));
        }

        int column::getType() const noexcept {
            return sqlite3_column_type(mStmtPtr, mIndex);
        }

        int column::getBytes() const noexcept {
            return sqlite3_column_bytes(mStmtPtr, mIndex);
        }

        std::ostream &operator<<(std::ostream &aStream, const column &aColumn) {
            aStream.write(aColumn.getText(), aColumn.getBytes());
            return aStream;
        }

        statement::Ptr::Ptr(sqlite3 *apSQLite, std::string &aQuery) :
                mpSQLite(apSQLite),
                mpStmt(nullptr),
                mpRefCount(nullptr) {
            const int ret = sqlite3_prepare_v2(apSQLite, aQuery.c_str(), static_cast<int>(aQuery.size()), &mpStmt,
                                               nullptr);
            if (SQLITE_OK != ret) {
                throw exception(apSQLite, ret);
            }
            // Initialize the reference counter of the sqlite3_stmt :
            // used to share the mStmtPtr between Statement and Column objects;
            // This is needed to enable Column objects to live longer than the Statement objet it refers to.
            mpRefCount = new unsigned int(1);  // NOLINT(readability/casting)
        }

        statement::Ptr::Ptr(const statement::Ptr &aPtr) :
                mpSQLite(aPtr.mpSQLite),
                mpStmt(aPtr.mpStmt),
                mpRefCount(aPtr.mpRefCount) {
            //assert(NULL != mpRefCount);
            //assert(0 != *mpRefCount);

            // Increment the reference counter of the sqlite3_stmt,
            // asking not to finalize the sqlite3_stmt during the lifetime of the new objet
            ++(*mpRefCount);
        }

        statement::Ptr::~Ptr() {
            //assert(NULL != mpRefCount);
            //assert(0 != *mpRefCount);

            // Decrement and check the reference counter of the sqlite3_stmt
            --(*mpRefCount);
            if (0 == *mpRefCount) {
                // If count reaches zero, finalize the sqlite3_stmt, as no Statement nor Column objet use it anymore.
                // No need to check the return code, as it is the same as the last statement evaluation.
                sqlite3_finalize(mpStmt);

                // and delete the reference counter
                delete mpRefCount;
                mpRefCount = nullptr;
                mpStmt = nullptr;
            }
            // else, the finalization will be done later, by the last object
        }
    }
}


int64_t getCurrentTime()      //直接调用这个函数就行了，返回值最好是int64_t，long long应该也可以
{
    struct timeval tv;
    gettimeofday(&tv, NULL);    //该函数在sys/time.h头文件中
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


#define NEED_TEST
#ifdef TEST
#ifdef NEED_TEST

int main(int argc, char *argv[]) {

    try {

        const char *verson = qtalk::sqlite::getLibVersion();

        // Open a database file
        qtalk::sqlite::database db("/Users/may/Desktop/1.db");


        long start = Timer::now();

        qtalk::sqlite::statement query1(db, "select count(1) from IM_Sessionlist;");
        int count = 0;

        if (query1.executeNext()) {
            count = query1.getColumn(0);
        }


        long start2 = Timer::now();


        long beginTime = getCurrentTime();

        for (int i = 0; i < 1000; ++i)
            db.exec("insert into test(id, name, size) values (1, 'nnd', 70)");

        long endTime = getCurrentTime();

        int diff = endTime - beginTime;

        // Compile a SQL query, containing one parameter (index 1)
        qtalk::sqlite::statement query(db, "select * From test where size > ?;");
        query.bind(1, 6);

        // Loop to execute the query step by step, to get rows of result
        while (query.executeNext()) {
            // Demonstrate how to get some typed column value
            int id = query.getColumn(0);
            const char *value = query.getColumn(1);
            int size = query.getColumn(2);
            std::cout << "row: " << id << ", " << value << ", " << size << std::endl;
        }
    }
    catch (std::exception &e) {
        std::cout << "exception: ") + e.what()) << std::endl;
    }
}

#endif
#endif
