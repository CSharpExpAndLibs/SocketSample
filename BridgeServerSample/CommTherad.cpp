#include <Windows.h>
#include <stdio.h>

#include "CommonType.h"

/// <summary>
/// 受信データからデータ長を求める
/// ここでは先頭の4byteがデータ長を表している
/// ものとする。実際の既定に従って適宜修正
/// </summary>
/// <param name="buff"></param>
/// <returns></returns>
int RetrieveDataLen(char* buff)
{
	int len = 0;
	for (int i = 0; i < 4; i++) {
		len |= buff[i] << (3 - i);
	}
	return len;
}

DWORD StartCommunication(LPVOID arg)
{
	CommThreadInf_t* inf = (CommThreadInf_t*)arg;
	char clientIp[64] = { 0 };

	// クライアントのIPを文字列として記憶しておく
	strcpy(clientIp, inet_ntoa(inf->socketAddr->sin_addr));
	printf("Receive thread for %s is created\n", clientIp);

	// 宛先のソケットを取得する
	int dstSocket = 0;
	dstSocket = GetDstSocket(inf->socket);
	while (dstSocket == 0) {
		Sleep(100);
		dstSocket = GetDstSocket(inf->socket);
	}

	char recvBuffer[1024];
	while (1) {
		// Payloadの長さ（ID等含む）
		int dataLength = 0;
		// 送信元で付加されたID
		int dataId = 0;
		// 最初のコンタクト
		int bytesRcv = recv(inf->socket, recvBuffer, sizeof(recvBuffer), 0);
		if (bytesRcv == 0) {
			printf("Connection Closed for %s\n", clientIp);
			break;
		}
		// データ長を求める
		dataLength = RetrieveDataLen(recvBuffer);
		// 送付データをバッファの準備
		char* sendData = (char*)malloc(dataLength);
		// 実際はもっと真面目なエラー処理が必要
		if (sendData == NULL) {
			perror("malloc() failed\n");
			continue;
		}
		// 送付データにコピー
		//  データ長領域をスキップ
		int currentDataLen = 0;
		char* srcBuff = &recvBuffer[4];
		bytesRcv -= 4;
		memcpy(sendData+currentDataLen, recvBuffer, bytesRcv);
		currentDataLen += bytesRcv;
		// データが一度で来ていなかったら繰り返し受信して
		// 送付データを完成させる
		while (currentDataLen < dataLength) {
			bytesRcv = recv(inf->socket, recvBuffer, sizeof(recvBuffer), 0);
			if (bytesRcv == 0) {
				printf("Connection Closed for %s\n", clientIp);
				break;
			}
			memcpy(sendData + currentDataLen, recvBuffer, bytesRcv);
			currentDataLen += bytesRcv;
		}
		// TODO ここまで。



	}

	return 0;
}