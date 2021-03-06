/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#if !defined(QUERYRESULT_H)
#define QUERYRESULT_H

#include <ace/Refcounted_Auto_Ptr.h>
#include <ace/Null_Mutex.h>

#include "Field.h"

#ifdef WIN32
  #define FD_SETSIZE 1024
  #include <winsock2.h>
#endif
#include <mysql.h>

class QueryResult
{
    public:
        QueryResult(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount);
        ~QueryResult();

        bool NextRow();

        Field *Fetch() const { return mCurrentRow; }

        const Field & operator [] (int index) const { return mCurrentRow[index]; }

        uint32 GetFieldCount() const { return mFieldCount; }
        uint64 GetRowCount() const { return mRowCount; }

    protected:
        Field *mCurrentRow;
        uint32 mFieldCount;
        uint64 mRowCount;

    private:
        enum Field::DataTypes ConvertNativeType(enum_field_types mysqlType) const;
        void EndQuery();
        MYSQL_RES *mResult;

};

typedef ACE_Refcounted_Auto_Ptr<QueryResult, ACE_Null_Mutex> QueryResult_AutoPtr;

typedef std::vector<std::string> QueryFieldNames;

class QueryNamedResult
{
    public:
        explicit QueryNamedResult(QueryResult* query, QueryFieldNames const& names) : mQuery(query), mFieldNames(names) {}
        ~QueryNamedResult() { delete mQuery; }

        // compatible interface with QueryResult
        bool NextRow() { return mQuery->NextRow(); }
        Field *Fetch() const { return mQuery->Fetch(); }
        uint32 GetFieldCount() const { return mQuery->GetFieldCount(); }
        uint64 GetRowCount() const { return mQuery->GetRowCount(); }
        Field const& operator[] (int index) const { return (*mQuery)[index]; }

        // named access
        Field const& operator[] (const std::string &name) const { return mQuery->Fetch()[GetField_idx(name)]; }
        QueryFieldNames const& GetFieldNames() const { return mFieldNames; }

        uint32 GetField_idx(const std::string &name) const
        {
            for (size_t idx = 0; idx < mFieldNames.size(); ++idx)
            {
                if (mFieldNames[idx] == name)
                    return idx;
            }
            ASSERT(false && "unknown field name");
            return uint32(-1);
        }

    protected:
        QueryResult *mQuery;
        QueryFieldNames mFieldNames;
};

#endif

