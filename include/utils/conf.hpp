/**
* @file conf.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-08-07-22-36
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CONF
#define CONF

#include <vector>
#include <utils/utils.hpp>

typedef struct {
    char item_name[50];
    char item_content[500];
}conf_item, *pconf_item;

class config : public singleton<config> {
public:
    ~config();
public:
    bool load(const char *conf_name);
    const char *get_string(const char *p_itemname);
    int get_int_default(const char *p_itemname, const int def);
    bool has_key(const char *p_itemname);
public:
    std::vector<pconf_item> item_list_;
}; 

#endif // CONF