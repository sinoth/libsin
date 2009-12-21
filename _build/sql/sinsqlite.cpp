
#include <stdlib.h>
#include <string.h>

#include "sinsqlite.h"


///////////////////////////////////////////////////////////////////////////////
//

sinsql_exception::sinsql_exception(const std::string& in_msg, const std::string& in_db_msg, int in_rc) throw()
    { msg = in_msg; db_msg = in_db_msg; rc = in_rc; }
sinsql_exception::~sinsql_exception() throw() {}
const char* sinsql_exception::getMsg() { return msg.c_str(); }
const char* sinsql_exception::getDBMsg() { return db_msg.c_str(); }
int sinsql_exception::getRC() { return rc; }



///////////////////////////////////////////////////////////////////////////////
//
sqlite3_stmt* sinsql_statement::me() { return my_statement; }
void sinsql_statement::reset() { sqlite3_reset(my_statement); }


sinsql_statement::sinsql_statement(sqlite3 *db, const char *query) {
    my_db = db;
    rc = sqlite3_prepare_v2(db, query, -1, &my_statement, NULL);
    switch ( rc ) {
        case SQLITE_OK: break;
        default:
            printf("Offending query: [%s]\n", query);
            throw (sinsql_exception("sinsql_statement ctor",sqlite3_errmsg(db),rc));
            break;
    }
}


sinsql_statement::~sinsql_statement() throw() {
    switch( sqlite3_finalize(my_statement) ) {
        case SQLITE_OK: break;
        default:
            fprintf(stderr, "SQL error (sinsql_statement dtor): [%d]%s\n", rc, sqlite3_errmsg(my_db) );
            //throw (rc); //don't throw! bad things will happen!
            break;
    }
}


int sinsql_statement::step() {
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


///////////////////////////////////////////////////////////////////////////////
//
bool sinsql::existence(sqlite3 *db, const char *query) {
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
bool sinsql::exec(sqlite3 *db, const char *query) {
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
bool sinsql::get_text(sqlite3 *db, const char *query, char* &result ) {
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
            result = (char*)malloc(string_size+1);
            strcpy(result, (const char*)sqlite3_column_text(temp_statement.me(), 0));
            return false; break;

        default:
           break;
    }
    return true;
}


//////////////////////////////////////////////////////////////////
bool sinsql::get_int(sqlite3 *db, const char *query, int &result) {
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
bool sinsql::get_blob(sqlite3 *db, const char *query, void* &result, int &result_size) {
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
bool sinsql::open_db(const char *db_name, sqlite3* &db ) {
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
