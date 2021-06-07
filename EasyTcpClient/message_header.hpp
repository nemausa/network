/**
* @file message_header.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-06-22-09
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

enum cmd_type_e{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_JOIN,
    CMD_ERROR
};

struct data_header {
    short length;
    short cmd;
};

struct login : public data_header {
    login() {
        length = sizeof(login);
        cmd = CMD_LOGIN;
    }
    char user_name[32];
    char password[32];
    char data[932];
};

struct login_result : public data_header {
    login_result() {
        length = sizeof(login_result);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
    char data[992];
};

struct logout : public data_header {
    logout() {
        length = sizeof(logout);
        cmd = CMD_LOGOUT;
    }
    char user_name[32];
};

struct logout_result : public data_header {
    logout_result() {
        length = sizeof(logout_result);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;
    }
    int result;
};


struct new_join : public data_header {
    new_join() {
        length = sizeof(new_join);
        cmd = CMD_NEW_JOIN;
        sock = 0;
    }
    int sock;
};


#endif // MESSAGE_HEADER