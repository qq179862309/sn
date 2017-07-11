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
		SOCKET			s;                   //�׽��־��  
		SSL*			pSSL;				//SSL���
		HANDLE			hEvent;              //���׽���s��������¼�������  
		sockaddr_in		addrRemote;     //�ͻ��˵�ַ��Ϣ  
		ST_SOCKET_OBJ	*pNext;       //ָ��һ��SOCKET_OBJ  
	}SOCKET_OBJ, *PSOCKET_OBJ;

	typedef struct ST_THREAD_OBJ
	{
		HANDLE			hEvents[WSA_MAXIMUM_WAIT_EVENTS];    //��¼��ǰ�߳�Ҫ�ȴ����¼�����ľ��  
		int				nSocketCount;                           //��¼��ǰ�̴߳�����׽�������  
		PSOCKET_OBJ		pSockHeader;                    //��ǰ�̴߳�����׽��ֶ����б��ͷָ��  
		PSOCKET_OBJ		pSockTail;                      //��ǰ�̴߳�����׽��ֶ����б��βָ��  
		CRITICAL_SECTION cs;                        //�ؼ�����α���,Ϊ����ͬ���Ա��ṹ�ķ���  
		ST_THREAD_OBJ	*pNext;                       //ָ����һ��THREAD_OBJ  
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
		PTHREAD_OBJ			m_pThreadList; //ָ���̶߳����б�ı�ͷ -- �̲߳����б����߳�һһ��Ӧ  
		CRITICAL_SECTION	m_cs;          //ͬ����ȫ�ֱ���g_pThreadList�ķ��� 
		LONG				m_nTotalConnections;   //�ܹ���������  
		LONG				m_nCurrentConnections; //��ǰ��������  
		SSL_CTX*			m_ctx;					//SSL
	};
}