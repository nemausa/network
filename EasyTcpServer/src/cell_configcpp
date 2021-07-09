#include "cell_config.hpp"

cell_config::cell_config() {

}

cell_config::~cell_config(){

}

cell_config &cell_config::instance() {
    static cell_config obj;
    return obj;
}

void cell_config::init(int argc, char* args[]) {
    exe_path_ = args[0];
    for (int n = 1; n < argc; n++) {
        made_cmd(args[n]);
    }
}

void cell_config::made_cmd(char *cmd) {
    char *val = strchr(cmd ,'=');
    if (val) {
        *val = '\0';
        val++;
        kv_[cmd] = val;
        cell_log::info("mode_cmd %s=%s", cmd, val);
    } else {
        kv_[cmd] = "";
        cell_log::info("made_cmd %s", cmd);
    }
}

const char *cell_config::get_string(const char *arg_name, const char *def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
        cell_log::info("cell_config not found <%s>", arg_name);
    } else {
        def = itr->second.c_str();
    }
    cell_log::info("%s=%s", arg_name, def);
    return def;
}

int cell_config::get_int(const char *arg_name, int def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
        cell_log::info("cell_config not found <%s>", arg_name);
    } else {
        def = atoi(itr->second.c_str());
    }
    cell_log::info("%s=%s", arg_name, def);
    return def;
}
