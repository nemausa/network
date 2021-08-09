
#include "depends/cell_config.hpp"

#include "depends/cell_log.hpp"

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
         SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "mode_cmd {}={}", cmd, val);
    } else {
        kv_[cmd] = "";
         SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "made_cmd {}", cmd);
    }
}

const char *cell_config::get_string(const char *arg_name, const char *def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
         SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "cell_config not found <{}>", arg_name);
    } else {
        def = itr->second.c_str();
    }
     SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "{}={}", arg_name, def);
    return def;
}

int cell_config::get_int(const char *arg_name, int def) {
    auto itr = kv_.find(arg_name);
    if (itr == kv_.end()) {
         SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "cell_config not found <{}>", arg_name);
    } else {
        def = atoi(itr->second.c_str());
    }
     SPDLOG_LOGGER_INFO(spdlog::get(LOG_NAME), "{}={}", arg_name, def);
    return def;
}

bool cell_config::has_key(const char *key) {
    auto itr = kv_.find(key);
    return itr != kv_.end();
}