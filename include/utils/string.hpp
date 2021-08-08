/**
* @file string.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-08-12-47
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef STRING
#define STRING

#include <cstdio>
#include <cstring>
#include <cctype>

char* rtrim(char *str) {
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;
}

char* ltrim(char *str) {
    char *end;
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void trim(char *str) {
    ltrim(str);
    rtrim(str);
}

#endif // STRING