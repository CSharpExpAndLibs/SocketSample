#include <Windows.h>
#include <stdio.h>

#include "CommonType.h"

/// <summary>
/// ��M�f�[�^����f�[�^�������߂�
/// �����ł͐擪��4byte���f�[�^����\���Ă���
/// ���̂Ƃ���B���ۂ̊���ɏ]���ēK�X�C��
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

	// �N���C�A���g��IP�𕶎���Ƃ��ċL�����Ă���
	strcpy(clientIp, inet_ntoa(inf->socketAddr->sin_addr));
	printf("Receive thread for %s is created\n", clientIp);

	// ����̃\�P�b�g���擾����
	int dstSocket = 0;
	dstSocket = GetDstSocket(inf->socket);
	while (dstSocket == 0) {
		Sleep(100);
		dstSocket = GetDstSocket(inf->socket);
	}

	char recvBuffer[1024];
	while (1) {
		// Payload�̒����iID���܂ށj
		int dataLength = 0;
		// ���M���ŕt�����ꂽID
		int dataId = 0;
		// �ŏ��̃R���^�N�g
		int bytesRcv = recv(inf->socket, recvBuffer, sizeof(recvBuffer), 0);
		if (bytesRcv == 0) {
			printf("Connection Closed for %s\n", clientIp);
			break;
		}
		// �f�[�^�������߂�
		dataLength = RetrieveDataLen(recvBuffer);
		// ���t�f�[�^���o�b�t�@�̏���
		char* sendData = (char*)malloc(dataLength);
		// ���ۂ͂����Ɛ^�ʖڂȃG���[�������K�v
		if (sendData == NULL) {
			perror("malloc() failed\n");
			continue;
		}
		// ���t�f�[�^�ɃR�s�[
		//  �f�[�^���̈���X�L�b�v
		int currentDataLen = 0;
		char* srcBuff = &recvBuffer[4];
		bytesRcv -= 4;
		memcpy(sendData+currentDataLen, recvBuffer, bytesRcv);
		currentDataLen += bytesRcv;
		// �f�[�^����x�ŗ��Ă��Ȃ�������J��Ԃ���M����
		// ���t�f�[�^������������
		while (currentDataLen < dataLength) {
			bytesRcv = recv(inf->socket, recvBuffer, sizeof(recvBuffer), 0);
			if (bytesRcv == 0) {
				printf("Connection Closed for %s\n", clientIp);
				break;
			}
			memcpy(sendData + currentDataLen, recvBuffer, bytesRcv);
			currentDataLen += bytesRcv;
		}
		// TODO �����܂ŁB



	}

	return 0;
}