#ifndef _SINSQLITE_H
#define _SINSQLITE_H

#include <exception>
#include <string>
#include <stdio.h>
#include <sqlite3.h>


///////////////////////////////////////////////////////////////////////////////
// sql exceptions yay
//
class sinsql_exception: public std::exception {
public:
    sinsql_exception(const std::string& in_msg, const std::string& in_db_msg, int in_rc) throw();
    ~sinsql_exception() throw();
    const char* getMsg();
    const char* getDBMsg();
    int getRC();

private:
    int rc;
    std::string msg;
    std::string db_msg;
};


///////////////////////////////////////////////////////////////////////////////
// statement class for RAII magic
//
class sinsql_statement {
public:
    sinsql_statement(sqlite3 *db, const char *query);
    ~sinsql_statement() throw();

    sqlite3_stmt* me();
    void reset();
    int step();

private:
    int rc;
    sqlite3_stmt *my_statement;
    sqlite3 *my_db;
};


///////////////////////////////////////////////////////////////////////////////
// sqlite helper functions
//
class sinsql {
public:
    static bool existence(sqlite3 *db, const char *query);
    static bool exec(sqlite3 *db, const char *query);
    static bool get_text(sqlite3 *db, const char *query, char* &result );
    static bool get_int(sqlite3 *db, const char *query, int &result);
    static bool get_blob(sqlite3 *db, const char *query, void* &result, int &result_size);
    static bool open_db(const char *db_name, sqlite3* &db );
};

#endif
