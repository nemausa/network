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

void rtrim(char *str) {
    if (str == NULL)
        return;
    
    size_t len = strlen(str);
    while (len >0 && str[len-1] == ' ')
        str[--len] = 0;
}

void ltrim(char *str) {
    char *start, *temp;
    temp = str;
    while ((*temp) == ' ') {
        ++temp;
    }
    if ((*temp) == '\0') {
        *str = '\0';
        return;
    }
    start = temp;
    temp = str; 
    while ((*start) != '\0') {
        (*temp) = (*start);
        start++;
        temp++;
    }
    (*temp) = '\0';
}

void trim(char *str) {
    ltrim(str);
    rtrim(str);
}

#endif // STRING