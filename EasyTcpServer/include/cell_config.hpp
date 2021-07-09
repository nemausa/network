/**
* @file cell_config.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-07-09-15-46
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef CELL_ARGS
#define CELL_ARGS

#include <map>
#include "cell.hpp"

class cell_config {
private:
    cell_config();
    ~cell_config();
public:
    static cell_config &instance();
    void init(int argc, char* args[]);
    void made_cmd(char *cmd);
    const char *get_string(const char *arg_name, const char *def);
    int get_int(const char *arg_name, int def);
private:
    std::string exe_path_;
    std::map<std::string, std::string> kv_;
};

#endif // CELL_ARGS