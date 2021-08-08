#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/conf.hpp"
#include "utils/string.hpp"

config::~config() {
    std::vector<pconf_item>::iterator pos;
    for (pos = item_list_.begin(); pos != item_list_.end(); ++pos) {
        delete (*pos);
    }
    item_list_.clear();
}

bool config::load(const char *conf_name) {
    FILE *fp;
    fp = fopen(conf_name, "r");
    if (fp == NULL) 
        return false;
    
    char linebuf[501];

    while (!feof(fp)) {
        if (fgets(linebuf, 500, fp) == NULL)
            continue;
        
        if (linebuf[0] == 0)
            continue;
        
        if (*linebuf == ';' || 
            *linebuf == ' ' || 
            *linebuf == '#' || 
            *linebuf == '[' || 
            *linebuf == '\t' || 
            *linebuf == '\n')
            continue;
        
    lblprocestring:
        if (strlen(linebuf) > 0) {
            if (linebuf[strlen(linebuf) - 1] == 10 ||
                linebuf[strlen(linebuf) - 1] == 13 ||
                linebuf[strlen(linebuf) - 1] == 32)  {
                    linebuf[strlen(linebuf) - 1] = 0;
                    goto lblprocestring;
            } 
        }
        if (linebuf[0] == 0)
            continue;
        
        char *ptmp = strchr(linebuf, '=');
        if (ptmp != NULL) {
            pconf_item p_item = new conf_item;
            memset(p_item, 0, sizeof(conf_item));
            strncpy(p_item->item_name, linebuf, (int)(ptmp - linebuf));
            strcpy(p_item->item_content, ptmp + 1);

            trim(p_item->item_name);
            ltrim(p_item->item_content);

            item_list_.push_back(p_item);
        } else {

        }
    }

    fclose(fp);
    return true;
}

const char *config::get_string(const char *p_itemname) {
    std::vector<pconf_item>::iterator pos;
    for (pos = item_list_.begin(); pos != item_list_.end(); ++pos) {
        if (strcasecmp((*pos)->item_name, p_itemname) == 0)
            return (*pos)->item_content;
    }
    return NULL;
}

int config::get_int_default(const char *p_itemname, const int def) {
    std::vector<pconf_item>::iterator pos;
    for (pos = item_list_.begin(); pos != item_list_.end(); ++pos) {
        if (strcasecmp((*pos)->item_name, p_itemname) == 0)
            return atoi((*pos)->item_content);
    }
    return def;
}

bool config::has_key(const char *p_itemname) {
    std::vector<pconf_item>::iterator pos;
    for (pos = item_list_.begin(); pos != item_list_.end(); ++pos) {
        if (strcasecmp((*pos)->item_name, p_itemname) == 0)
            return true; 
    }
    return false;
}