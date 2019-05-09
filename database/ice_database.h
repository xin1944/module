#ifndef ICE_DATABASE_H
#define ICE_DATABASE_H

#include <string.h>
#include "json/json.h"


using namespace Json;

bool execQuery(const char* chsql, Json::Value& jvalue);
bool execNoQuery(const char* chsql);
bool isDbConnect();

//bool execQuery2(const char* chsql, Json::Value& jvalue);
//bool execNoQuery2(const char* chsql);

#endif // ICE_DATABASE_H
