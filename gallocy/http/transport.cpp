#include "gallocy/http/transport.h"


gallocy::http::UDPTransport::UDPTransport(gallocy::common::Peer dst_peer, uint16_t listen_port) {
    // TODO(rverdon): Create the socket here? Or in the UDPClient?
    // CREATE the socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("udptransport socket");
        exit(1);
    }

    // SETUP timeout on the socket
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = UDP_TIMEOUT_100_MS;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, std::addressof(tv), sizeof(tv)) < 0) {
        perror("udptransport setsockopt");
        exit(1);
    }

    peer = dst_peer;

    // SETUP the listen sockaddr
    memset(std::addressof(listen_addr), 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // BIND the socket to the port passed in (or if its 0, it will get assigned a port)
    if (bind(sock, reinterpret_cast<struct sockaddr*>(std::addressof(listen_addr)), sizeof(listen_addr)) < 0) {
        perror("udptransport bind");
        exit(1);
    }
}


gallocy::string gallocy::http::UDPTransport::read() {
    int recvlen = 0;
    gallocy::string http;

    // WHILE there is still data on the socket
    while ((recvlen = recvfrom(sock, buf, UDP_BUFSIZE, MSG_WAITALL,
                    reinterpret_cast<struct sockaddr*>(std::addressof(recv_addr)), std::addressof(recv_addr_len))) > 0) {
        // APPEND it to the http string
        http.append(reinterpret_cast<char*>(buf), recvlen);
    }

    return http;
}


void gallocy::http::UDPTransport::write(gallocy::string http) {
    uint32_t total_sent = 0;
    uint32_t sent = 0;
    struct sockaddr_in dst_addr = peer.get_socket();

    while (total_sent != http.length()) {
        if ((sent = sendto(sock, http.substr(total_sent).c_str(), http.length() - total_sent, 0,
                        reinterpret_cast<struct sockaddr*>(std::addressof(dst_addr)), sizeof(dst_addr))) < 0) {
            perror("udptransport sendto failed");
            exit(1);
        }
        total_sent += sent;
    }
}


gallocy::http::UDPTransport::~UDPTransport() {
    // CLEANUP
    close(sock);
}
