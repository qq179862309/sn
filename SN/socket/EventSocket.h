#pragma once
#include <openssl/rsa.h>       /* SSLeay stuff */  
#include <openssl/crypto.h>  
#include <openssl/x509.h>  
#include <openssl/pem.h>  
#include <openssl/ssl.h>  
#include <openssl/err.h>


namespace SN
{
	typedef struct ST_SOCKET_OBJ
	{
		SOCKET			s;                   //套接字句柄  
		SSL*			pSSL;				//SSL句柄
		HANDLE			hEvent;              //与套接字s相关联的事件对象句柄  
		sockaddr_in		addrRemote;     //客户端地址信息  
		ST_SOCKET_OBJ	*pNext;       //指向一个SOCKET_OBJ  
	}SOCKET_OBJ, *PSOCKET_OBJ;

	typedef struct ST_THREAD_OBJ
	{
		HANDLE			hEvents[WSA_MAXIMUM_WAIT_EVENTS];    //记录当前线程要等待的事件对象的句柄  
		int				nSocketCount;                           //记录当前线程处理的套接字数量  
		PSOCKET_OBJ		pSockHeader;                    //当前线程处理的套接字对象列表的头指针  
		PSOCKET_OBJ		pSockTail;                      //当前线程处理的套接字对象列表的尾指针  
		CRITICAL_SECTION cs;                        //关键代码段变量,为的是同步对本结构的访问  
		ST_THREAD_OBJ	*pNext;                       //指向下一个THREAD_OBJ  
	}THREAD_OBJ, *PTHREAD_OBJ;

	class CEventSocket;

	typedef struct _tagWORKINFO
	{
		CEventSocket*	lpEventSocket;
		PTHREAD_OBJ		lpThread;
	}WORKINFO, *PWORKINFO;

	class CEventSocket
	{
	public:
		CEventSocket();
		~CEventSocket();
		//static	CEventSocket*	GetEventSocket();
		static	DWORD	WINAPI	WorkThread(LPVOID lpNull);
		static	DWORD	WINAPI	AcceptThread(LPVOID lpNull);

		virtual VOID	OnRecv(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError) = 0;
		virtual VOID	OnClose(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError) = 0;
		virtual VOID	OnAccept(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket, int nError) = 0;

		int				EventCreate(USHORT uPort);
		PSOCKET_OBJ		GetSocketObj(SOCKET s, SSL* pSSL);
		VOID			FreeSocketObj(PSOCKET_OBJ lpSocket);
		PTHREAD_OBJ		GetThreadObj();
		VOID			FreeThreadObj(PTHREAD_OBJ lpThread);
		VOID			RebuildArray(PTHREAD_OBJ lpThread);
		BOOL			InsertSocketObj(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket);
		VOID			RemoveSocketObj(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket);
		PSOCKET_OBJ		FindSocketObj(PTHREAD_OBJ lpThread, int nIndex);
		PTHREAD_OBJ		AssignToFreeThread(PSOCKET_OBJ lpSocket);
		BOOL			HandleIO(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket);
		VOID			AcceptIO(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket);

		int				SendMsg(PSOCKET_OBJ lpSocket, LPVOID lpData, int nLength);
		int				SendMsgSSL(PSOCKET_OBJ lpSocket, LPVOID lpData, int nLength);

		int				Close(PTHREAD_OBJ lpThread, PSOCKET_OBJ lpSocket);

	public:
		SOCKET				m_sListen;
		WSAEVENT			m_wsaEvent;
		USHORT				m_nPort;
		PTHREAD_OBJ			m_pThreadList; //指向线程对象列表的表头 -- 线程参数列表，和线程一一对应  
		CRITICAL_SECTION	m_cs;          //同步对全局变量g_pThreadList的访问 
		LONG				m_nTotalConnections;   //总共连接数量  
		LONG				m_nCurrentConnections; //当前连接数量  
		SSL_CTX*			m_ctx;					//SSL
	};
}