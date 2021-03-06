// CMServer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "client_signaldata.h"
#include "init.h"
#include "global_data.h"
#include "client_thread.h"
#include "client_requestpost.h"
#include "client_mem.h"
#include "nc_requestpost.h"
#include "cm_mysql.h"
#include "statistics _thread.h"
#include "tinyxml2.h"

extern bool Mysql_InitConnectionPool(int nMin, int nMax);
extern void DueTostatistical();
extern void SimTotal();

int main()
{
	if (!UniqueInstance())
	{
		_tprintf(_T("已经有在使用中的服务\n"));
		return -1;
	}

	

	if (!InitWinsock2() || !GetExtensionFunctionPointer() || !Mysql_InitConnectionPool(10, 50))
	{
		_tprintf(_T("winsock2初始化失败\n"));
		return -1;
	}

	//if (!CreateDB())
	//{
	//	_tprintf(_T("数据库创建失败\n"));
	//	return -1;
	//}

	if (!CreateUserTbl() || !CreateKhTbl() || !CreateSimTbl() || !CreateDxzhTbl() || !CreateLlcTbl() || !CreateSsdqTbl() || !CreateKhjlTbl() || !CreateLltcTbl() || !CreateStatisticsTbl() || !CreateLogTbl())
	{
		_tprintf(_T("表创建失败\n"));
		return -1;
	}

	InitClientSockMem();

	_beginthreadex(NULL, 0, statistics_func, NULL, 0, NULL);
	_beginthreadex(NULL, 0, mysql_thread, NULL, 0, &mysqlThreadId);

	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	DWORD dwCpunum = sys.dwNumberOfProcessors * 2;
	g_dwPagesize = sys.dwPageSize * 8;

	hComport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);
	if (NULL == hComport)
	{
		_tprintf(_T("创建完成端口失败, errCode = %d\n"), WSAGetLastError());
		return -1;
	}

	for (DWORD i = 0; i < dwCpunum; i++)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, client_thread, NULL, 0, NULL);
		if (NULL != hThread)
		{
			CloseHandle(hThread);
		}
	}

	LISTEN_SOCK* client_listen_sock = new LISTEN_SOCK;
	if (NULL == client_listen_sock)
		return -1;
	if (!InitClientListenSock(client_listen_sock, 7099))
	{
		delete client_listen_sock;
		return -1;
	}

	client_listen_sock->f_acceptex = Client_PostAcceptEx;
	for (int i = 0; i < ACCEPT_NUM; i++)
	{
		Client_PostAcceptEx(client_listen_sock);
	}

	LISTEN_SOCK* api_listen_sock = new LISTEN_SOCK;
	if (NULL == api_listen_sock)
		return -1;
	if (!InitClientListenSock(api_listen_sock, 80))
	{
		delete api_listen_sock;
		return -1;
	}

	api_listen_sock->f_acceptex = Nc_PostAcceptEx;
	for (int i = 0; i < ACCEPT_NUM; i++)
	{
		Nc_PostAcceptEx(api_listen_sock);
	}

	while (true)
	{
		int rt = WSAWaitForMultipleEvents(g_dwListenCount, hEvent_Array, FALSE, INFINITE, FALSE);
		if (WSA_WAIT_FAILED == rt)
		{
			_tprintf(_T("带来异常，推出\n"));
			return -1;
		}

		for (DWORD nIndex = 0; nIndex < g_dwListenCount; nIndex++)
		{
			rt = WaitForSingleObject(hEvent_Array[nIndex], 0);
			if (WSA_WAIT_TIMEOUT == rt)
				continue;
			else if (WSA_WAIT_FAILED == rt)
			{
				_tprintf(_T("带来异常，推出\n"));
				return -1;
			}

			WSAResetEvent(hEvent_Array[nIndex]);
			for (size_t i = 0; i < ACCEPT_NUM; i++)
			{
				LSock_Array[nIndex]->f_acceptex(LSock_Array[nIndex]);
			}

			if (LSock_Array[nIndex]->acceptPendingMap.size() > MAX_ACCEPT_NUM)
			{
				int nError = 0;
				int optlen,
					optval;
				optlen = sizeof(optval);

				for (tbb::concurrent_hash_map<int, void*>::iterator i = LSock_Array[nIndex]->acceptPendingMap.begin(); i != LSock_Array[nIndex]->acceptPendingMap.end(); ++i)
				{
					nError = getsockopt(((KEY_VALUE*)(i->second))->sock, SOL_SOCKET, SO_CONNECT_TIME, (char*)&optval, &optlen);
					if (SOCKET_ERROR == nError)
					{
						_tprintf(_T("getsockopt failed error: %d\n"), WSAGetLastError());
						continue;
					}

					if (optval != 0xffffffff && optval > 60)
					{
						closesocket(((KEY_VALUE*)(i->second))->sock);
						((KEY_VALUE*)(i->second))->sock = INVALID_SOCKET;
					}
				}
			}
		}
	}

    return 0;
}

