#include "easy_tcp_server.hpp"
#include "cell_network.hpp"

easy_tcp_server::easy_tcp_server() {
    sockfd_ = INVALID_SOCKET;
    observer_ = new observer(*this);
}

easy_tcp_server::~easy_tcp_server() {
    close();
}

SOCKET easy_tcp_server::init_socket() {
    cell_network::init();
    if (INVALID_SOCKET != sockfd_) {
        cell_log::info("<socket=%d>close old socket...\n", (int)sockfd_);
        close();
    }
    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sockfd_) {
        cell_log::info("error, create socket failed\n");
    } else {
        cell_network::make_reuseadd(sockfd_);
        cell_log::info("create socket=<%d> succes\n", (int)sockfd_);
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
#endif

    int ret = ::bind(sockfd_, (sockaddr*)&sin, sizeof(sockaddr));
    if (SOCKET_ERROR == ret) {
        cell_log::info("error, bind port<%d> failed\n", port);
    } else {
        cell_log::info("bind port<%d> success\n", port);
    }
    return ret;
}

int easy_tcp_server::listen(int n) {
    int ret = ::listen(sockfd_, n);

    if (SOCKET_ERROR == ret) {
        cell_log::info("socket=<%d> error, listen port failed\n", sockfd_);
    } else {
        cell_log::info("socket=<%d> listen port success\n", sockfd_);
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
        cell_log::info("socket=<%d> error, accept invalid socket\n",(int)sockfd_);
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
        auto server = new cell_server(n, observer_);
        cell_servers_.push_back(server);
        server->start();
    }
    thread_.start(nullptr,
        [this](cell_thread* pthread) {
            on_run(pthread);
        });

}

void easy_tcp_server::close() {
    cell_log::info("easy_tcp_server close begin\n");
    thread_.close();
    if (sockfd_ != INVALID_SOCKET) {
        for (auto s : cell_servers_) {
            delete s;
        }
        cell_servers_.clear();
#ifdef _WIN32
        closesocket(sockfd_);
        WSACleanup();
#else
        ::close(sockfd_);
#endif        
    }
    cell_log::info("easy_tcp_server close end\n");
}

bool easy_tcp_server::is_run() {
    return sockfd_ != INVALID_SOCKET;
}

void easy_tcp_server::on_run(cell_thread *pthread) {
    while (pthread->is_run()) {
        time4msg();
        fd_set fd_read;
        FD_ZERO(&fd_read);
        FD_SET(sockfd_, &fd_read);
        timeval t = {0, 1};
        int ret = select(sockfd_ + 1, &fd_read, nullptr, nullptr, &t);
        if (ret < 0) {
            std::cout << "accept select end" << std::endl;
            pthread->exit();
            break;
        }

        if (FD_ISSET(sockfd_, &fd_read)) {
            FD_CLR(sockfd_, &fd_read);
            accept();
        }
    }
}


void easy_tcp_server::time4msg() {
    auto t1 = time_.second();
    if (t1 >= 1.0f) {
        cell_log::info("thread<%d>, time<%f>, socket<%d>, client_count<%d>, recv_count<%d>, message<%d>\n",
            cell_servers_.size(), t1, sockfd_, observer_->client_count(), observer_->recv_count(), observer_->msg_count());
        observer_->msg_count(0);
        observer_->recv_count(0);
        time_.update();

    }

}

