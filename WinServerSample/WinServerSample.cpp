// WinServerSample.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>

#include <Windows.h>

#include <stdio.h> //printf(), fprintf(), perror()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
//#include <ws2def.h>
//#include <winsock2.h>

#define QUEUELIMIT 5

int server(int servPort) {

    int servSock; //server socket descriptor
    int clitSock; //client socket descriptor
    struct sockaddr_in servSockAddr; //server internet socket address
    struct sockaddr_in clitSockAddr; //client internet socket address
    int clitLen; // client internet socket address length
    char recvBuffer[1025];
    int byteRcvd;
    int totalBytesRcvd;

    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed.");
        return EXIT_FAILURE;
    }

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

    while (1) {
        clitLen = sizeof(clitSockAddr);
        if ((clitSock = accept((SOCKET)servSock, (struct sockaddr*)&clitSockAddr,
            &clitLen)) < 0) {
            perror("accept() failed.");
            return EXIT_FAILURE;
        }
        printf("connected from %s.\n", inet_ntoa(clitSockAddr.sin_addr));

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

int main(int ac, char *av[])
{
	server(atoi(av[1]));
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
