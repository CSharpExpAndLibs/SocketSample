/*
 * Unix Domain socketサーバーサンプル
 *  このサーバーを立ち上げて
 *    logger -p <facil>.<prio> -u /tmp/domaind.socket '<Message>'
 *  を実行すれば、<facil>.<prio> に相当する接頭詞の数値が分かる。
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUELIMIT 5
#define SOCKET_FILE "/tmp/domaind.socket"

int main(int argc, char* argv[]) {

    int servSock;
    int clitSock;
    struct sockaddr_un servSockAddr;
    struct sockaddr_un clitSockAddr;
    unsigned int clitLen;
    char recvBuffer[1025];
    int byteRcvd;
    int totalBytesRcvd;

    // Socket fileを削除
    remove(SOCKET_FILE);
    
    if ((servSock = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0 ){
        perror("socket() failed.");
        return EXIT_FAILURE;
    }

    memset(&servSockAddr, 0, sizeof(servSockAddr));
    servSockAddr.sun_family      = AF_LOCAL;
    strcpy(servSockAddr.sun_path, SOCKET_FILE);
    
    if (bind(servSock, (struct sockaddr *) &servSockAddr,
             sizeof(servSockAddr) ) < 0 ) {
        perror("bind() failed.");
        return EXIT_FAILURE;
    }

    if (listen(servSock, QUEUELIMIT) < 0) {
        perror("listen() failed.");
        return EXIT_FAILURE;
    }

    while (1) {
        clitLen = sizeof(clitSockAddr);
        if ((clitSock = accept(servSock, (struct sockaddr *) &clitSockAddr,
                               &clitLen)) < 0) {
            perror("accept() failed.");
            return EXIT_FAILURE;
        }
        printf("connected from domain sock:%s.\n", SOCKET_FILE);

        // クライアントから明示的に接続切断されるまで受信を続ける
        totalBytesRcvd = 0;
        while (1) {
            byteRcvd = recv(clitSock, recvBuffer, sizeof(recvBuffer)-1, 0);
            if(byteRcvd == 0){
                printf("Conneciton Closed\n");
                break;
            } else if (byteRcvd < 0){
                perror("recv() failed.");
                break;
            }
            recvBuffer[byteRcvd] = '\0';
            printf("%s\n", recvBuffer);
            totalBytesRcvd += byteRcvd;
        }
        close(clitSock);
        printf("total Bytes received=%d\n", totalBytesRcvd);
    }

    return EXIT_SUCCESS;
}
