#include "cell_config.hpp"
#include "cell_log.hpp"

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
        LOG_INFO("mode_cmd %s=%s", cmd, val);
    } else {
        kv_[cmd] = "";
        LOG_INFO("made_cmd %s", cmd);
    }
}

const char *cell_config::get_string(const char *arg_name, const char *def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
        LOG_INFO("cell_config not found <%s>", arg_name);
    } else {
        def = itr->second.c_str();
    }
    LOG_INFO("%s=%s", arg_name, def);
    return def;
}

int cell_config::get_int(const char *arg_name, int def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
        LOG_INFO("cell_config not found <%s>", arg_name);
    } else {
        def = atoi(itr->second.c_str());
    }
    LOG_INFO("%s=%s", arg_name, def);
    return def;
}

bool cell_config::has_key(const char *key) {
    auto itr = kv_.find(key);
    return itr != kv_.end();
}