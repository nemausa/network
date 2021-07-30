#include "easy_iocp_server.hpp" 
#include "cell_network.hpp"

void easy_iocp_server::start(int n) {
    easy_tcp_server::start<cell_iocp_server>(n);
}

void easy_iocp_server::on_run(cell_thread *pthread) {
    cell_iocp iocp;
    iocp.create();
    iocp.reg(sockfd());
    iocp.load_accept(sockfd());

    const int len = 2014;
    char buf[len] = {};

    io_data_base iodata = {};
    iodata.wsabuff.buf = buf;
    iodata.wsabuff.len = len;

    iocp.post_accept(&iodata);
    io_event ioevent = {};
    while (pthread->is_run()) {
        time4msg();
        int ret = iocp.wait(ioevent, 1);
        if (ret < 0) {
            cell_log::info("easy_iocp_server.on_run wait exit");
            pthread->exit();
            break;
        }
        if (ret == 0) {
            continue;
        }

        if (io_type_e::ACCEPT == ioevent.p_io_data->io_type) {
            iocp_accept(ioevent.p_io_data->sockfd);
            iocp.post_accept(&iodata);
        }
    }
}

SOCKET easy_iocp_server::iocp_accept(SOCKET sock) {
//    if (INVALID_SOCKET == sock) {
//        cell_log::info("accept INVALID_SOCKET");
//    } else {
//        if (client_count_ < max_client) {
//            add_client_to_server(new cell_client(sock, SEND_BUFF_SIZE, RECV_BUFF_SIZE));
//        } else {
//            cell_network::destory_socket(sock);
//            cell_log::info("accept to maxclient");           
//        }
//    }
}