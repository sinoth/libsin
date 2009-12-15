#ifndef _SINSQLITE_H
#define _SINSQLITE_H

#include <exception>
#include <string>
#include <sqlite3.h>


///////////////////////////////////////////////////////////////////////////////
// sql exceptions yay
//
class sinsql_exception: public std::exception {
public:
    sinsql_exception(const std::string& in_msg, const std::string& in_db_msg, int in_rc) throw()
        { msg = in_msg; db_msg = in_db_msg; rc = in_rc; }
    ~sinsql_exception() throw() {}
    //const char* what() { return msg.c_str(); }
    const char* getMsg() { return msg.c_str(); }
    const char* getDBMsg() { return db_msg.c_str(); }
    int getRC() { return rc; }
private:
    int rc;
    std::string msg;
    std::string db_msg;
};




///////////////////////////////////////////////////////////////////////////////
// statement class for RAII magic
//
class sinsql_statement {
private:
    int rc;
    sqlite3_stmt *my_statement;
    sqlite3 *my_db;

public:

    sqlite3_stmt* me() { return my_statement; }
    void reset() { sqlite3_reset(my_statement); }

    //////////////////////////////////////////////////
    sinsql_statement(sqlite3 *db, const char *query) {
            my_db = db;
            rc = sqlite3_prepare_v2(db, query, -1, &my_statement, NULL);
            switch ( rc ) {
                case SQLITE_OK: break;
                default:
                    throw (sinsql_exception("sinsql_statement ctor",sqlite3_errmsg(db),rc));
                    break;
            }
        }


    /////////////////////////////
    ~sinsql_statement() throw() {
            switch( sqlite3_finalize(my_statement) ) {
                case SQLITE_OK: break;
                default:
                    fprintf(stderr, "SQL error (sinsql_statement dtor): [%d]%s\n", rc, sqlite3_errmsg(my_db) );
                    //throw (rc); //don't throw! bad things will happen!
                    break;
            }
        }


    ////////////
    int step() {
            rc = sqlite3_step(my_statement);
            switch ( rc ) {
                case SQLITE_DONE: case SQLITE_ROW:
                    //both of these cases are fine.  pass the result
                    return rc; break;
                default:
                    //everything else is an error, so flip out
                    throw (sinsql_exception("sinsql_statement step",sqlite3_errmsg(my_db),rc));
                    break;
            }
        }

};




///////////////////////////////////////////////////////////////////////////////
// sqlite helper functions
//
class sinsql {
public:

    ///////////////////////////////////////////////////////
    static bool existence(sqlite3 *db, const char *query) {
            sinsql_statement temp_statement(db, query);

            switch ( temp_statement.step() ) {
                case SQLITE_DONE:
                    //no result from query, so send back a negatory
                    return false; break;
                case SQLITE_ROW:
                    //found a row! send back an affirmative
                    return true; break;
                default:
                    break;
            }
            return false;
        }


    //////////////////////////////////////////////////
    static bool exec(sqlite3 *db, const char *query) {
            sinsql_statement temp_statement(db, query);

            switch ( temp_statement.step() ) {
                case SQLITE_DONE:
                    //successful
                    return false; break;
                case SQLITE_ROW:
                    //also okay
                    return false; break;
                default:
                    break;
            }
            return true;
        }


    //////////////////////////////////////////////////////////////////////
    static bool get_text(sqlite3 *db, const char *query, char* &result ) {
            int string_size;
            sinsql_statement temp_statement(db, query);

            switch ( temp_statement.step() ) {
                case SQLITE_DONE:
                    //we don't want this! we want a result :( ah well, return error
                    result = NULL;
                    return true; break;

                case SQLITE_ROW:
                    //make sure its a blob
                    if ( sqlite3_column_type(temp_statement.me(),0) != SQLITE_TEXT ) {
                        //not a blob, return error
                        result = NULL;
                        return true; break;
                    }
                    //grab the result
                    string_size = sqlite3_column_bytes(temp_statement.me(),0);
                    result = (char*)malloc(string_size);
                    memcpy(result, sqlite3_column_text(temp_statement.me(), 0), string_size);
                    return false; break;

                default:
                   break;
            }
            return true;
        }


    //////////////////////////////////////////////////////////////////
    static bool get_int(sqlite3 *db, const char *query, int &result) {
            sinsql_statement temp_statement(db, query);

            switch ( temp_statement.step() ) {
                case SQLITE_DONE:
                    //we don't want this! we want a result :( ah well, return error
                    return true; break;

                case SQLITE_ROW:
                    //yeaaa a result, lets get that sucker
                    result = sqlite3_column_int(temp_statement.me(), 0);
                    return false; break;

                default:
                    break;
            }
            return true;
        }


    ///////////////////////////////////////////////////////////////////////////////////////
    static bool get_blob(sqlite3 *db, const char *query, void* &result, int &result_size) {
            sinsql_statement temp_statement(db, query);

            switch ( temp_statement.step() ) {
                case SQLITE_DONE:
                    //we don't want this! we want a result :( ah well, return error
                    result = NULL;
                    return true; break;

                case SQLITE_ROW:
                    //make sure its a blob
                    if ( sqlite3_column_type(temp_statement.me(),0) != SQLITE_BLOB ) {
                        //not a blob, return error
                        result = NULL; result_size = -1;
                        return true; break;
                    }
                    //grab the result
                    result_size = sqlite3_column_bytes(temp_statement.me(),0);
                    result = (void*)malloc(result_size);
                    memcpy(result, sqlite3_column_blob(temp_statement.me(), 0), result_size);
                    return false; break;

                default:
                    break;
            }
            return true;
        }


    /////////////////////////////////////////////////////////
    static bool open_db(const char *db_name, sqlite3* &db ) {
            int rc;

            rc = sqlite3_open(db_name, &db);
            switch ( rc ) {
                case SQLITE_OK:
                    //everything is fine
                    return false; break;
                default:
                    //freak out!
                    throw (sinsql_exception("sinsql_open_db",sqlite3_errmsg(db),rc));
                    break;
            }
        }

};

#endif
