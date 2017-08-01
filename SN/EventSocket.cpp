#include "stdafx.h"
#include "socket/EventSocket.h"

#ifdef _WIN64
#	pragma comment(lib, "\\lib\\x64\\libeay32.lib")
#	pragma comment(lib, "\\lib\\x64\\ssleay32.lib")
#else
#	pragma comment(lib, "\\lib\\x86\\libeay32.lib")
#	pragma comment(lib, "\\lib\\x86\\ssleay32.lib")
#endif 

const int MAX_SOCKET_COUNT_PER_THREAD = WSA_MAXIMUM_WAIT_EVENTS - 1; // 定义每个线程最大管理的
const int CUSTOM_SOCKET_COUNT_PER_THREAD = 1;

//CEventSocket*	CEventSocket::GetEventSocket()
//{
//	if (G_pEventSocket == NULL)
//	{
//		G_pEventSocket = new CEventSocket();
//	}
//	return G_pEventSocket;
//}

CEventSocket::CEventSocket()
{
	m_pThreadList = NULL;
	m_nPort = 0;
	m_nTotalConnections = 0;
	m_nCurrentConnections = 0;
	m_ctx = NULL;
}


CEventSocket::~CEventSocket()
{
}

DWORD	CEventSocket::AcceptThread(LPVOID lpNull)
{
	CEventSocket*	lpEventSocket = (CEventSocket*)lpNull;
	while (1)
	{
		int nRet = ::WSAWaitForMultipleEvents(1, &lpEventSocket->m_wsaEvent, FALSE, -1, FALSE);
		if (nRet == WAIT_FAILED) // 失败  
		{
			break;
		}
		else if (nRet == WSA_WAIT_TIMEOUT) // 超时  
		{
			continue;
		}
		else // 成功 -- FD_ACCEPT事件发生  
		{
			::WSANETWORKEVENTS wsaNetEvent;
			::WSAEnumNetworkEvents(lpEventSocket->m_sListen, lpEventSocket->m_wsaEvent, &wsaNetEvent);
			if (wsaNetEvent.lNetworkEvents&FD_ACCEPT)
			{
				if (wsaNetEvent.iErrorCode[FD_ACCEPT_BIT] == 0)
				{
					while (1)
					{
						sockaddr_in si;
						int nLen = sizeof(si);
						SOCKET client = accept(lpEventSocket->m_sListen, (sockaddr*)&si, &nLen);
						if (client == SOCKET_ERROR)
							break;
						PSOCKET_OBJ pSocket = NULL;
						if (lpEventSocket->m_ctx)
						{
							SSL* sslSocket = SSL_new(lpEventSocket->m_ctx);
							if (sslSocket == NULL)
							{
								closesocket(client);
								break;
							}
							SSL_set_fd(sslSocket, client);

							SSL_set_accept_state(sslSocket);

							pSocket = lpEventSocket->GetSocketObj(client, sslSocket);
						}
						else
						{
							pSocket = lpEventSocket->GetSocketObj(client, NULL);
						}
						memcpy(&pSocket->addrRemote, &si, sizeof(sockaddr_in));
						::WSAEventSelect(pSocket->s, pSocket->hEvent, FD_READ | FD_CLOSE);
						PTHREAD_OBJ lpThread = lpEventSocket->AssignToFreeThread(pSocket);

						lpEventSocket->AcceptIO(lpThread, pSocket);
					}
				}
			}
		}
	}
	::DeleteCriticalSection(&lpEventSocket->m_cs);
	return 0;
}

DWORD	CEventSocket::WorkThread(LPVOID lpNull)
{
	PWORKINFO lpWorkInfo = (PWORKINFO)lpNull;
	PTHREAD_OBJ lpThread = lpWorkInfo->lpThread;
	CEventSocket* lpEventSocket = lpWorkInfo->lpEventSocket;
	while (1)
	{
		//等待网络事件  
		int nIndex = WSAWaitForMultipleEvents(lpThread->nSocketCount + 1, lpThread->hEvents, false, WSA_INFINITE, false);
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		//查看受信事件  
		for (int i = nIndex; i<lpThread->nSocketCount + 1; i++)
		{
			nIndex = WSAWaitForMultipleEvents(1, &lpThread->hEvents[i], true, 0, false);
			if (WSA_WAIT_FAILED == nIndex || WSA_WAIT_TIMEOUT == nIndex)
			{
				continue;
			}
			else
			{
				if (0 == i)
				{
					lpEventSocket->RebuildArray(lpThread); // 重建句柄数组  
					if (lpThread->nSocketCount == 0)
					{
						lpEventSocket->FreeThreadObj(lpThread);
						return 0;
					}
					::WSAResetEvent(lpThread->hEvents[0]);
				}
				else
				{
					PSOCKET_OBJ pSocket = lpEventSocket->FindSocketObj(lpThread, i);
					if (pSocket != NULL)
					{
						if (pSocket->pSSL != NULL)
						{
							int nShake = SSL_do_handshake(pSocket->pSSL);
							if (nShake == 1) 
							{ // 若返回值为1，则SSL握手已完成
								if (!lpEventSocket->HandleIO(lpThread, pSocket))
								{
									lpEventSocket->RebuildArray(lpThread); // 重建句柄数组  
								}
							}
						}
						else
						{
							if (!lpEventSocket->HandleIO(lpThread, pSocket))
							{
								lpEventSocket->RebuildArray(lpThread); // 重建句柄数组  
							}
						}
					}
				}
			}
		}
	} // end while()  
	delete lpWorkInfo;
	return 0;
}

//申请一个套接字对象 - 为一个连接的socket申请一个ST_SOCKET_OBJ对象，初始化  
PSOCKET_OBJ CEventSocket::GetSocketObj(SOCKET s, SSL* pSSL)
{
	PSOCKET_OBJ pSocket = (PSOCKET_OBJ)GlobalAlloc(GPTR, sizeof(SOCKET_OBJ)); // new SOCKET_OBJ()  
	if (pSocket != NULL)
	{
		pSocket->hEvent = ::WSACreateEvent();  // 创建事件  
		pSocket->s = s;    // 初始化套接字socket  
		pSocket->pSSL = pSSL;
		pSocket->pNext = NULL;
	}
	return pSocket;
}

//释放一个套接字对象  
VOID CEventSocket::FreeSocketObj(PSOCKET_OBJ lpSocket)
{
	::CloseHandle(lpSocket->hEvent);
	if (lpSocket->pSSL != NULL)
	{
		SSL_shutdown(lpSocket->pSSL);/* 关闭 SSL 连接 */
		SSL_free(lpSocket->pSSL); /* 释放 SSL */
	}
	if (lpSocket->s != INVALID_SOCKET)
	{
		::closesocket(lpSocket->s);
	}
	GlobalFree(lpSocket);
	lpSocket = NULL;
}

//申请一个线程对象,初始化它的成员,并将它添加到线程对象列表中  
PTHREAD_OBJ CEventSocket::GetThreadObj()
{
	PTHREAD_OBJ pThread = (PTHREAD_OBJ)GlobalAlloc(GPTR, sizeof(THREAD_OBJ));
	if (pThread != NULL)
	{
		pThread->pNext = NULL;
		pThread->nSocketCount = 0;
		pThread->pSockHeader = pThread->pSockTail = NULL;
		::InitializeCriticalSection(&pThread->cs);

		//创建一个事件对象,用于指示该线程的句柄数组hEvents[]需要重组  
		pThread->hEvents[0] = WSACreateEvent();

		//将线程对象加入线程对象列表中  
		::EnterCriticalSection(&m_cs);
		pThread->pNext = m_pThreadList;
		m_pThreadList = pThread;
		::LeaveCriticalSection(&m_cs);
	}
	return pThread;
}

VOID	CEventSocket::FreeThreadObj(PTHREAD_OBJ lpThread)
{
	::EnterCriticalSection(&m_cs);
	PTHREAD_OBJ p = m_pThreadList;
	if (p == lpThread)
	{
		m_pThreadList = lpThread->pNext;
	}
	else
	{
		while (p != NULL&&p->pNext != lpThread)   //找到pThread的前一个节点  
		{
			p = p->pNext;
		}
		if (p != NULL)
		{
			p->pNext = lpThread->pNext;
		}
	}
	::LeaveCriticalSection(&m_cs);
	//释放资源  
	::CloseHandle(lpThread->hEvents[0]); // pThread->hEvents[0]为用来指示hEvents[]需要重组的事件  
	::DeleteCriticalSection(&lpThread->cs);
	GlobalFree(lpThread);
}

//重新建立线程对象的events数组  即将pSocket中的event与pThread中的events关联起来  
VOID	CEventSocket::RebuildArray(PTHREAD_OBJ lpThread)
{
	::EnterCriticalSection(&lpThread->cs);
	PSOCKET_OBJ pSocket = lpThread->pSockHeader;
	int n = 1; //从下标为1开始 跳过pThread->hEvents[0]  
	while (pSocket != NULL)
	{
		lpThread->hEvents[n++] = pSocket->hEvent;
		pSocket = pSocket->pNext;
	}
	::LeaveCriticalSection(&lpThread->cs);
}

//向线程的套接字列表中插入一个套接字对象  
BOOL	CEventSocket::InsertSocketObj(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket)
{
	BOOL bRet = FALSE;
	::EnterCriticalSection(&lpThread->cs);
	//if (lpThread->nSocketCount < MAX_SOCKET_COUNT_PER_THREAD) // 每个线程最多管理MAX_SOCKET_COUNT_PER_THREAD个socket连接  
	if (lpThread->nSocketCount < CUSTOM_SOCKET_COUNT_PER_THREAD)
	{
		if (lpThread->pSockHeader == NULL)
		{   // 对象列表为空  
			lpThread->pSockHeader = lpThread->pSockTail = lpSocket;
		}
		else
		{   // 追加一个到对象列表末尾  
			lpThread->pSockTail->pNext = lpSocket;
			lpThread->pSockTail = lpSocket;
		}
		lpThread->nSocketCount++;
		bRet = TRUE;
	}
	::LeaveCriticalSection(&lpThread->cs);
	if (bRet)
	{
		InterlockedIncrement(&m_nTotalConnections);
		InterlockedIncrement(&m_nCurrentConnections);
	}
	return bRet;
}

//从给定线程的套接字对象列表中移除一个套接字对象  
VOID	CEventSocket::RemoveSocketObj(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket)
{
	::EnterCriticalSection(&lpThread->cs);
	//在套接字对象列表中查找指定的套接字对象,找到后将它移除  
	PSOCKET_OBJ pTemp = lpThread->pSockHeader;
	if (pTemp == lpSocket)
	{
		if (lpThread->pSockHeader == lpThread->pSockTail)
		{
			lpThread->pSockHeader = lpThread->pSockTail = pTemp->pNext;
		}
		else
		{
			lpThread->pSockHeader = pTemp->pNext;
		}
	}
	else
	{
		while (pTemp != NULL && pTemp->pNext != lpSocket)
		{
			pTemp = pTemp->pNext;
		}
		if (pTemp != NULL)
		{
			if (lpThread->pSockTail == lpSocket)
			{
				lpThread->pSockTail = pTemp;
			}
			pTemp->pNext = lpSocket->pNext;
		}
	}
	lpThread->nSocketCount--;
	::LeaveCriticalSection(&lpThread->cs);
	::WSASetEvent(lpThread->hEvents[0]); //指示线程重建句柄数组  
	::InterlockedDecrement(&m_nCurrentConnections);
}

PSOCKET_OBJ		CEventSocket::FindSocketObj(PTHREAD_OBJ lpThread, int nIndex)
{
	PSOCKET_OBJ pSocket = lpThread->pSockHeader;
	while (--nIndex)
	{
		if (pSocket == NULL)
		{
			return NULL;
		}
		pSocket = pSocket->pNext;
	}
	return pSocket;
}

VOID	CEventSocket::AcceptIO(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket)
{
	OnAccept(lpThread, lpSocket, 0);
}

BOOL	CEventSocket::HandleIO(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket)
{
	::WSANETWORKEVENTS wsaEvent;
	::WSAEnumNetworkEvents(lpSocket->s, lpSocket->hEvent/*待复位句柄*/, &wsaEvent);
	// FD_READ事件  
	if (wsaEvent.lNetworkEvents&FD_READ)
	{
		if (wsaEvent.iErrorCode[FD_READ_BIT] == 0)
		{
			////////////////////////////////////////////////////////////////////接收数据
			OnRecv(lpThread, lpSocket, 0);
		}
	}
	// FD_CLOSE事件  
	else if (wsaEvent.lNetworkEvents&FD_CLOSE)
	{
		OnClose(lpThread, lpSocket, 0);

		RemoveSocketObj(lpThread, lpSocket); // 需要重建句柄数组  
		//::WSASetEvent(pThread->hEvents[0]); //指示线程重建句柄数组  
		FreeSocketObj(lpSocket);
		return FALSE;
	}
	return TRUE;
}

//将一个套接字对象安排给空闲的线程处理  
//如果所有线程接受的socket已满，则新建线程提供服务  
PTHREAD_OBJ	CEventSocket::AssignToFreeThread(PSOCKET_OBJ lpSocket)
{
	lpSocket->pNext = NULL;
	::EnterCriticalSection(&m_cs);
	PTHREAD_OBJ pThread = m_pThreadList;
	//试图插入到现存的线程  
	while (pThread != NULL)
	{
		if (InsertSocketObj(pThread, lpSocket))
		{
			break;
		}
		pThread = pThread->pNext;
	}
	//如果没有空闲线程,为这个套接字对象创建新的线程  
	if (pThread == NULL)
	{
		pThread = GetThreadObj();
		InsertSocketObj(pThread, lpSocket);
		PWORKINFO pWorkInfo = new WORKINFO;
		pWorkInfo->lpEventSocket = this;
		pWorkInfo->lpThread = pThread;
		HANDLE hThread = ::CreateThread(NULL, 0, WorkThread, pWorkInfo, 0, NULL);
		CloseHandle(hThread);
	}
	::LeaveCriticalSection(&m_cs);
	//指示线程重建句柄数组  
	::WSASetEvent(pThread->hEvents[0]);

	return pThread;
}

int		CEventSocket::EventCreate(USHORT uPort)
{
	// 创建监听套节字

	m_sListen = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in sin;
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(uPort);
	if (SOCKET_ERROR == bind(m_sListen, (sockaddr*)&sin, sizeof(sockaddr)))
	{
		return 1;
	}
	uPort = uPort;
	listen(m_sListen, SOMAXCONN);

	m_wsaEvent = ::WSACreateEvent();
	::WSAEventSelect(m_sListen, m_wsaEvent, FD_ACCEPT);
	::InitializeCriticalSection(&m_cs);

	HANDLE hThread = CreateThread(NULL, 0, AcceptThread, this, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

int		CEventSocket::SendMsg(PSOCKET_OBJ lpSocket, LPVOID lpData, int nLength)
{
	int nRet = send(lpSocket->s, (char*)lpData, nLength, 0);
	return nRet;
}

int		CEventSocket::SendMsgSSL(PSOCKET_OBJ lpSocket, LPVOID lpData, int nLength)
{
	int nRet = SSL_write(lpSocket->pSSL, lpData, nLength);
	return nRet;
}

int		CEventSocket::Close(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket)
{
	RemoveSocketObj(lpThread, lpSocket); // 需要重建句柄数组  
	FreeSocketObj(lpSocket);
	RebuildArray(lpThread); // 重建句柄数组  
	return 0;
}

//VOID	CEventSocket::OnRecv(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError)
//{
//
//}
//
//VOID	CEventSocket::OnClose(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError)
//{
//
//}
//
//VOID	CEventSocket::OnAccept(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError)
//{
//
//}