#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <iostream>
#include <string>
#include <curl/curl.h>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output);

#endif