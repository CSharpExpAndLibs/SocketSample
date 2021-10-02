#include <Windows.h>

#include <stdio.h> //printf(), fprintf(), perror()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()

#define MSGSIZE 32
#define MAX_MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

char *tolowerstr(char *str)
{
    char* p = str;
    while (*p != '\0') {
        *p++ = tolower(*p);
    }
    return str;
}

int client(char *servIpAddr, int servPort) {

    int sock; //local socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    char sendBuffer[4096]; //receive temporary buffer
    int err = 0;

    memset(&servSockAddr, 0, sizeof(servSockAddr));

    servSockAddr.sin_family = AF_INET;
    servSockAddr.sin_addr.s_addr = inet_addr(servIpAddr);
    servSockAddr.sin_port = htons(servPort);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        err = WSAGetLastError();
        printf("socket() failed:%d\n", err);
        return EXIT_FAILURE;
    }

    printf("Start to connect to %s:%d\n", servIpAddr, servPort);
    if (connect(sock, (struct sockaddr*)&servSockAddr,
                sizeof(servSockAddr)) < 0) {
        err = WSAGetLastError();
        printf("connect() failed:%d\n", err);
        err = EXIT_FAILURE;
        goto EXIT;
    }

    printf("connect to %s:%d\n", servIpAddr, servPort);

#if 0
    /*
     * Serverの待ちバッファ長より長いStreamを送った時の実験用コード
     *  ⇛ ちゃんと細切れで受け取ってくれた。
     */
    for (int i = 0; i < sizeof(sendBuffer); i++) {
        sendBuffer[i] = 'A';
    }
    if (send(sock, sendBuffer, sizeof(sendBuffer), 0) < 0) {
        perror("send() failed.");
        return EXIT_FAILURE;
    }
#endif // 0

    while (1) {
        printf("Enter Exit to exit program, or enter string to send!\n");
        if (fgets(sendBuffer, sizeof(sendBuffer), stdin) == NULL) {
            printf("Read error\n");
            continue;
        }
        // '\n'を取り除く
        if (sendBuffer[strlen(sendBuffer)-1] == '\n')
            sendBuffer[strlen(sendBuffer)-1] = '\0';
        
        if (strcmp(tolowerstr(sendBuffer), "exit") == 0)
            break;
        
        if (send(sock, sendBuffer, strlen(sendBuffer), 0) < 0) {
            int err = WSAGetLastError();
            printf("send() failed:%d\n", err);
            err = EXIT_FAILURE;
            goto EXIT;
        }
    }

EXIT:
    closesocket(sock);

    return err;
}
