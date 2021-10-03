/*
* 2つのクライアントの通信の中継を行う
*  ClientA --> Master
*  ClientB --> Slave
*  - MasterはSlaveへのデータ送付をServerに依頼
*  - Serverはデータから既定の方法でIDを取得する
*  - ServerはSlaveへデータを送付し、上記IDを送信キューに格納する
*   - 条件によってはMasterからのデータをコピーまたは加工して複数のデータを作成する
*   - Serverは作成した複数データにそれぞれユニークな第二IDを付加する。
*   - Serverは送信キューへのID保存時にそれと第二IDを関連付けて保持する
*  -------------------------------------------------------- 
*  - SlaveはMasterへのデータ送付をServerに依頼
*  - Serverはデータから既定の方法でIDを取得する
*  - Serverは取得したIDを送信キューと照合してMasterからの元データのIDを特定する
*  - 照合結果が一致したIDを付加した（或いは既に付加されている場合はそのまま）Slaveからのデータを
*    Masterへ送付し送信キューのIDを削除する
* 
*/
#include <Windows.h>

#include <stdio.h> //printf(), fprintf(), perror()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()

#include "CommonType.h"
#include "CommThread.h"

#define QUEUELIMIT 5

int clientSockets[2] = { 0 };

int GetDstSocket(int ownSocket)
{
    if (ownSocket == clientSockets[0])
        return clientSockets[1];
    else
        return clientSockets[0];
}

int StartServer(int servPort) {

    int servSock; //server socket descriptor
    int clitSock; //client socket descriptor
    struct sockaddr_in servSockAddr; //server internet socket address
    //struct sockaddr_in clitSockAddr; //client internet socket address
    int clitLen; // client internet socket address length
    char recvBuffer[1025];
    int byteRcvd;
    int totalBytesRcvd;
    HANDLE clientThreads[2] = { 0 };
    int clientNum = 0;

    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        int err = WSAGetLastError();
        printf("socket() failed:%d\n", err);
        return EXIT_FAILURE;
    }
    printf("Server socket (%d) opened\n", servSock);


    memset(&servSockAddr, 0, sizeof(servSockAddr));
    servSockAddr.sin_family = AF_INET;
    servSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSockAddr.sin_port = htons(servPort);

    if (bind(servSock, (struct sockaddr*)&servSockAddr,
        sizeof(servSockAddr)) < 0) {
        perror("bind() failed.");
        return EXIT_FAILURE;
    }

    if (listen(servSock, QUEUELIMIT) < 0) {
        perror("listen() failed.");
        return EXIT_FAILURE;
    }
    printf("Socket (%d) start to listen on port (%d)\n", servSock, servPort);

    while (1) {
        // ソケットアドレス長
        int sockAddrLen = sizeof(struct sockaddr_in);
        // ソケットアドレス構造体：ここにクライアントソケットの情報が入る
        struct sockaddr_in* clientSockAddr = (struct sockaddr_in*)malloc(sockAddrLen);
        if (clientSockAddr == NULL) {
            perror("malloc() failed.");
            return EXIT_FAILURE;
        }
        
        printf("Start waiting Connection...\n");
        int clientSocket = 0;
        if ((clientSocket = accept((SOCKET)servSock, (struct sockaddr*)clientSockAddr,
            &sockAddrLen)) < 0) {
            perror("accept() failed.");
            return EXIT_FAILURE;
        }
        printf("connected from %s.\n", inet_ntoa(clientSockAddr->sin_addr));

        // このクライアント用の通信スレッドを立ち上げる
        CommThreadInf_t* inf = (CommThreadInf_t*)malloc(sizeof(CommThreadInf_t));
        if (inf == NULL) {
            perror("malloc() failed.");
            return EXIT_FAILURE;
        }
        inf->socket = clientSocket;
        inf->socketAddr = clientSockAddr;
        HANDLE h = CreateThread(
            NULL,
            0,
            StartCommunication,
            inf,
            0,
            NULL
        );
        if (h == NULL) {
            perror("CreateThread() failed.");
            return EXIT_FAILURE;
        }
        clientThreads[clientNum++] = h;

        // クライアントから明示的に接続切断されるまで受信を続ける
        totalBytesRcvd = 0;
        while (1) {
            byteRcvd = recv(clitSock, recvBuffer, sizeof(recvBuffer) - 1, 0);
            if (byteRcvd == 0) {
                printf("Conneciton Closed\n");
                break;
            }
            else if (byteRcvd < 0) {
                perror("recv() failed.");
                break;
            }
            recvBuffer[byteRcvd] = '\0';
            printf("%s\n", recvBuffer);
            totalBytesRcvd += byteRcvd;
        }
        closesocket(clitSock);
        printf("total Bytes received=%d\n", totalBytesRcvd);
    }

    return EXIT_SUCCESS;
}
