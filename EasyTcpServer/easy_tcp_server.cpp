#include "easy_tcp_server.hpp"

easy_tcp_server::easy_tcp_server() {
    sockfd_ = INVALID_SOCKET;
    observer_ = new observer(*this);
}

easy_tcp_server::~easy_tcp_server() {
    close();
}

SOCKET easy_tcp_server::init_socket() {
#ifdef _WIN32   
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif

#ifndef _WIN32
    //忽略异常信号，默认情况会导致进程终止
	signal(SIGPIPE, SIG_IGN);
#endif

    if (INVALID_SOCKET != sockfd_) {
        printf("<socket=%d>close old socket...\n", (int)sockfd_);
        close();
    }
    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        printf("error, create socket failed\n");
    } else {
        printf("create socket=<%d> succes\n", (int)sockfd_);
    }
    return sockfd_;
}

int easy_tcp_server::bind(const char *ip, unsigned short port) {
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

#ifdef _WIN32
    if (ip) {
        sin.sin_addr.S_un.S_addr = inet_addr(ip);
    } else {
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if (ip) {
        sin.sin_addr.s_addr = inet_addr(ip);
    } else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
    int option = 1;
    if ( setsockopt ( sockfd_, SOL_SOCKET, SO_REUSEADDR, &option,
    sizeof( option ) ) < 0 ){
        printf( "setsockopt\n" );
    }
#endif


    int ret = ::bind(sockfd_, (sockaddr*)&sin, sizeof(sockaddr));
    if (SOCKET_ERROR == ret) {
        printf("error, bind port<%d> failed\n", port);
    } else {
        printf("bind port<%d> success\n", port);
    }
    return ret;
}

int easy_tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        printf("socket=<%d> error, listen port failed\n", sockfd_);
    } else {
        printf("socket=<%d> listen port success\n", sockfd_);
    }
    return ret;
}

SOCKET easy_tcp_server::accept() {
    sockaddr_in client_addr = {};
    int length = sizeof(sockaddr_in);
    SOCKET c_sock = INVALID_SOCKET;
#ifdef _WIN32
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, &length);
#else
    c_sock = ::accept(sockfd_, (sockaddr*)&client_addr, (socklen_t*)&length);
#endif
    if (INVALID_SOCKET == c_sock) {
        printf("socket=<%d> error, accept invalid socket\n",(int)sockfd_);
    } else {
        add_client_to_server(new cell_client(c_sock));
    }
    return c_sock;
}

void easy_tcp_server::add_client_to_server(cell_client *client) {
    auto min_server =  cell_servers_[0];
    for (auto server : cell_servers_) {
        if (min_server->count() > server->count()) {
            min_server = server;
        }
    }

    min_server->add_client(client);
    create_message("join");
}

void easy_tcp_server::start(int cell_server_count) {
    for (int n = 0; n < cell_server_count; n++) {
        auto server = new cell_server(sockfd_, observer_);
        cell_servers_.push_back(server);
        server->start();
    }

}

void easy_tcp_server::close() {
    if (sockfd_ != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(sockfd_);
        WSACleanup();
#else
        ::close(sockfd_);
#endif        
    }

}

bool easy_tcp_server::is_run() {
    return sockfd_ != INVALID_SOCKET;
}

bool easy_tcp_server::on_run() {
    if (is_run()) {
        time4msg();
        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(sockfd_, &fd_read);
        timeval t = {0, 10};
        int ret = select(sockfd_ + 1, &fd_read, nullptr, nullptr, &t);
        if (ret < 0) {
            std::cout << "accept select end" << std::endl;
            close();
            return false;
        }

        if (FD_ISSET(sockfd_, &fd_read)) {
            FD_CLR(sockfd_, &fd_read);
            accept();
            return true;
        }

        return true;
    }

}


void easy_tcp_server::time4msg() {
    auto t1 = time_.second();
    if (t1 >= 1.0f) {
        printf("thread<%d>, time<%f>, socket<%d>, client_count<%d>, recv_count<%d>, message<%d>\n",
            cell_servers_.size(), t1, sockfd_, observer_->client_count(), observer_->recv_count(), observer_->msg_count());
        observer_->msg_count(0);
        observer_->recv_count(0);
        time_.update();

    }

}

