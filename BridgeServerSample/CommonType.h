#pragma once

/// <summary>
/// 通信スレッドメソッドの引数
/// </summary>
typedef struct {
	int socket;
	struct sockaddr_in *socketAddr;
} CommThreadInf_t;
