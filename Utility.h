#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include <unistd.h>
#include <curl/curl.h>
#include <json/value.h>
#include <json/json.h>
#include <json/writer.h>
#include <ctime>
#include <fstream>
#include<algorithm>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

using namespace std;

char * GetCurrentTime();
tm * GetLocalTime();
bool IsFileExists(const char* fname);
bool CreateFile(const std::string& fname);
void WriteToFile(const std::string& fname,const std::string& str);
void ReadFile(const std::string& fname,std::string& str);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
void SendCurlData(const std::string &url,std::string &response);
bool ParseFile(Json::Value &root,const std::string &file);
bool ParseString(Json::Value &root,const std::string &str);
std::string UrlEncode(const std::string &s);
bool is_number(const std::string& s);

#endif
