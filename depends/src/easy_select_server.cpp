#include "depends/easy_select_server.hpp"
#include "depends/cell_select_server.hpp"

void easy_select_server::start(int n) {
    easy_tcp_server::start<cell_select_server>(n);
}

void easy_select_server::on_run(cell_thread *pthread) {
    cell_fdset fd_read;
    while (pthread->is_run()) {
        time4msg();
        fd_read.zero();
        fd_read.add(sockfd());
        timeval t = {0, 1};
        int ret = select(sockfd() + 1, fd_read.fdset(), 0, 0, &t);
        if (ret < 0) {
            if (errno == EINTR) {
                LOG_INFO("easy_select_server select EINTR");
                continue;
            }
            LOG_PERROR("easy_select_server.on_run select");
            pthread->exit();
            break;
        }
        if (fd_read.has(sockfd())) {
            accept();
        }
    }
}