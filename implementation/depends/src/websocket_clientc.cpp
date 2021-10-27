#include "websocket_clienc.hpp"

namespace nemausa {
namespace io {
websocket_clientc::websocket_clientc(SOCKET sockfd = INVALID_SOCKET,
        int send_size = SEND_BUFF_SIZE,
        int recv_size = RECV_BUFF_SIZE):
        http_clientc(sockfd, send_size, recv_size)
{

}

bool websocket_clientc::has_msg()
{
    if (client_state_e::join == state_)
        return http_clientc::has_msg();
    else if (client_state_e::run == state_)
        return has_msgws();

    return false;
}

void websocket_clientc::pop_msg()
{
    http_clientc::pop_msg();
    if (wsh_.header_size > 0)
    {
        recv_buffer_.pop(wsh_.header_size + wsh_.len);
        wsh_.header_size = 0;
        wsh_.len = 0;
    }
}

bool websocket_clientc::has_msgws()
{
    if (recv_buffer_.length() < 2)
        return false;    

    const uint8_t *data = (const uint8_t*)recv_buffer_.data();
    wsh_.header_size = 2;
    wsh_.fin = ((data[0] & 0x80) == 0x80);
    wsh_.opcode = static_cast<websocket_code_e>(data[0] & 0xF);
    wsh_.mask = ((data[1] & 0x80) == 0x80);
    if (wsh_.mask)
        wsh_.header_size += 4;
    
    if (opcode_close == wsh_.opcode) 
    {
        on_close();
        return true;
    }

    wsh_.len0 = (data[1] & 0x7F);

    if (wsh_.len0 == 126)
        wsh_.header_size +=2;
    else if (wsh_.len0 == 127)
        wsh_.header_size += 8;
    
    if (wsh_.len0 == 126)
    {
        wsh_.len |= data[2] << 8;
        wsh_.len |= data[3] << 0;
    }
    else if (wsh_.len0 == 127)
    {
        wsh_.len |= (uint64_t)data[2] << 56;
        wsh_.len |= (uint64_t)data[3] << 56;
        wsh_.len |= (uint64_t)data[4] << 56;
        wsh_.len |= (uint64_t)data[5] << 56;
        wsh_.len |= data[6] << 24;
        wsh_.len |= data[7] << 16;
        wsh_.len |= data[8] << 8;
        wsh_.len |= data[9] << 0;
    }
    else 
    {
        wsh_.len = wsh_.len0;
    }

    if (wsh_.header_size + wsh_.len > recv_buffer_.size())
    {
        SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "websocket::hasmsgws > recvbuff.size()");
        on_close();
        return false;
    }

    return true;
}

char * websocket_clientc::fetch_data()
{
    char *rbuf = recv_buffer_.data() + wsh_.header_size;
    // 将数据转换为无符号整数数据
    if (wsh_.mask)
    {
        const uint8_t *data = (const uint8_t *)recv_buffer_.data();
        wsh_.masking_key[0] = data[wsh_.header_size - 4];
        wsh_.masking_key[1] = data[wsh_.header_size - 3];
        wsh_.masking_key[2] = data[wsh_.header_size - 2];
        wsh_.masking_key[3] = data[wsh_.header_size - 1];
        for (uint64_t i = 0; i < wsh_.len; ++i)
            rbuf[i] ^= wsh_.masking_key[i & 0x3];
    }
    return rbuf;
}

void websocket_clientc::do_mask(int len)
{
    if (send_buffer_.length() < len)
        return;

    char *rbuf = send_buffer_.data() + send_buffer_.length() -len;
    const uint8_t *masking_key = (const uint8_t*) &mask_key_;
    for (int i = 0; i < len, i++)
        rbuf[i] ^= masking_key[i & 0x3];
}

int websocket_clientc::write_header(websocket_code_e opcode, 
        uint64_t len, 
        bool mask, 
        int32_t mask_key)
{
    uint8_t header[14] = {};
    uint8_t header_size = 0;

    if (len < 126)
    {
        header_size = 2;
    }
    else if (len < 65536)
    {
        header_size = 4;
    }
    else 
    {
        header_size = 10;
    }

    header[0] = 0x80 | opcode;
    if (len < 126)
    {
        header[1] = (len & 0x7F);
    }
    else if (len < 65536)
    {
        header[1] = 126;
        header[2] = (len >> 8) & 0xFF;
        header[3] = (len >> 0) & 0xFF;
    }
    else 
    {
        header[1] = 127;
        for (int i = 2; i < 10; i++)
            header[i] = (len >> (9-i)*8) & 0xFF;
    }

    if (mask)
    {
        mask_key_ = rand();
        header[1] = 0x80;
        header_size += 4;
        const uint8_t * masking_key = (const uint8_t *)&mask_key_;
        header[header_size - 4] = masking_key[0];
        header[header_size - 3] = masking_key[1];
        header[header_size - 2] = masking_key[2];
        header[header_size - 1] = masking_key[3];
    }

    int ret = send_data((const char*)header, header_size);
    if (SOCKET_ERROR == ret)
    {
        SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "senddata error");
    }
    return ret;
}

int websocket_clientc::write_text(const char * pdata, int len)
{
    int ret  = write_header(opcode_text, len, true, mask_key_);
    if (SOCKET_ERROR != ret)
    {
        ret = send_data(pdata, len);
        if (SOCKET_ERROR == ret)
        {
            SPDLOG_LOGGER_ERROR(spdlog::get(MULTI_SINKS), "senddata error");
            return ret;
        }
        do_mask(len);
    }
    return ret;
}

int websocket_clientc::ping()
{
    return write_header(opcode_ping, 0, false, 0);
}

int websocket_clientc::pong()
{
    return write_header(opcode_pong, 0, false, 0);
}

websocket_header & websocket_clientc::websocket_header()
{
    return wsh_;
}

} // namespace nemausa
} // namespace io