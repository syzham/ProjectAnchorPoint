#ifndef PROJECTANCHORPOINT_LOGGER_H
#define PROJECTANCHORPOINT_LOGGER_H

#include <iostream>
#include <string>

const char header = '=';
const int headerLength = 60;
const char tab = ' ';
const int tabLength = 4;

extern inline void LogHeader(const char* title);
extern inline void Log(const char* body, int level = 0);
extern inline void LogError(const char* error, int code = 0);

#endif //PROJECTANCHORPOINT_LOGGER_H
