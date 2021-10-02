#include <stdio.h> //printf(), fprintf(), perror()
#include <sys/socket.h> //socket(), connect(), recv()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> //atoi(), return , EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset()
#include <unistd.h> //close()

#define MSGSIZE 32
#define MAX_MSGSIZE 1024
#define BUFSIZE (MSGSIZE + 1)

int main(int argc, char* argv[]) {

    int sock; //local socket descripter
    struct sockaddr_in servSockAddr; //server internet socket address
    unsigned short servPort; //server port number
    char sendBuffer[4096]; //receive temporary buffer

    if (argc != 3) {
        fprintf(stderr, "argument count mismatch error.\n");
        return EXIT_FAILURE;
    }

    memset(&servSockAddr, 0, sizeof(servSockAddr));

    servSockAddr.sin_family = AF_INET;

    if (inet_aton(argv[1], &servSockAddr.sin_addr) == 0) {
        fprintf(stderr, "Invalid IP Address.\n");
        return EXIT_FAILURE;
    }

    if ((servPort = (unsigned short)atoi(argv[2])) == 0) {
        fprintf(stderr, "invalid port number.\n");
        return EXIT_FAILURE;
    }
    servSockAddr.sin_port = htons(servPort);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ){
        perror("socket() failed.");
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr*)&servSockAddr,
                sizeof(servSockAddr)) < 0) {
        perror("connect() failed.");
        return EXIT_FAILURE;
    }

    printf("connect to %s\n", inet_ntoa(servSockAddr.sin_addr));

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
        
        if (strcmp(sendBuffer, "Exit") == 0)
            break;
        
        if (send(sock, sendBuffer, strlen(sendBuffer), 0) < 0) {
            perror("send() failed.");
            return EXIT_FAILURE;
        }
    }
    
    close(sock);

    return EXIT_SUCCESS;
}
