// GIRLUdpServer.h: interface for the GIRLUdpServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIRLUDPSERVER_H__36C79CC4_050B_49FE_9938_751A3FF0F708__INCLUDED_)
#define AFX_GIRLUDPSERVER_H__36C79CC4_050B_49FE_9938_751A3FF0F708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>
#include "hlinfos.h"
#include "IGClockable.h"

enum GIRLUdpMessageType {
#if _TRACES >= 5
	Msg_Debug,
#endif
	Msg_Log,
	Msg_Rcon,
	Msg_Info,
	Msg_Rules,
	Msg_Players,
	Msg_Error
};

enum GIRLUdpErrorType {
	E_Started,
	E_ThreadEnded,
	E_Bind,
	E_RecvFrom,
	E_RconChallange,
	E_HandlePacket
};

struct GIRLUdpMessage {
	DWORD m_dwParam;
	virtual ~GIRLUdpMessage() {};
	virtual GIRLUdpMessageType GetType() = 0;
};

struct GIRLUdpMessageLog : public GIRLUdpMessage
{
	GIRLUdpMessageLog(const std::string& sString) : m_sLine(sString) {};
	std::string m_sLine;
	virtual GIRLUdpMessageType GetType() {return Msg_Log;};
};

struct GIRLUdpMessageRcon : public GIRLUdpMessage
{
	GIRLUdpMessageRcon(const std::string& sString) : m_sString(sString) {};
	std::string m_sString;
	virtual GIRLUdpMessageType GetType() {return Msg_Rcon;};
};

#if _TRACES >= 5
struct GIRLUdpMessageDebug : public GIRLUdpMessage
{
	GIRLUdpMessageDebug(const char* pString) : m_sString(pString) {};
	std::string m_sString;
	virtual GIRLUdpMessageType GetType() {return Msg_Debug;};
};
#endif

struct GIRLUdpMessageInfo : public GIRLUdpMessage, public ServerInfoStruct
{
	virtual GIRLUdpMessageType GetType() {return Msg_Info;};
};

struct GIRLUdpMessageRules : public GIRLUdpMessage, public std::list<RuleInfoStruct>
{
	virtual GIRLUdpMessageType GetType() {return Msg_Rules;};
};

struct GIRLUdpMessagePlayers : public GIRLUdpMessage, public std::list<PlayerInfoStruct>
{
	virtual GIRLUdpMessageType GetType() {return Msg_Players;};
};

struct GIRLUdpMessageError : public GIRLUdpMessage
{
	GIRLUdpMessageError(GIRLUdpErrorType eType, int nError, int nWSAError) :
		m_eType(eType),
		m_nError(nError),
		m_nWSAError(nWSAError)
	{};

	virtual GIRLUdpMessageType GetType() {return Msg_Error;};
	GIRLUdpErrorType m_eType;
	int m_nError;
	int m_nWSAError;
};

struct GIRLUdpFragmentedMessage
{
	GIRLUdpFragmentedMessage() : 
		m_nTotalFragments(0), 
		m_pFragments(NULL), 
		m_pFragmentLengths(NULL),
		m_dwID(0)
		{};
	virtual ~GIRLUdpFragmentedMessage();

	DWORD m_dwID;
	int m_nTotalFragments;
	char** m_pFragments;
	int* m_pFragmentLengths;

	void Init(DWORD dwID, int nFragments);
	void SetFragment(int nFragment, char* pBuf, int nLen);
};

typedef std::map<DWORD, GIRLUdpFragmentedMessage*> ID2FRAG;

class GIRLUdpServer : public CIGCLockable
{
public:
	GIRLUdpServer(HWND hWnd, UINT uMsg, unsigned short uLocalPort, unsigned long uRemoteIP, unsigned short uRemotePort, DWORD dwParam);
	virtual ~GIRLUdpServer();

protected:
	HWND m_hWnd;
	UINT m_uMsg;
	bool m_bStop;
	bool m_bStarted;
	bool m_bContinue;
	unsigned short m_uPort;
	unsigned long m_uRemoteIP;
	unsigned short m_uRemotePort;
	unsigned short m_uLogmodPort;
	DWORD m_dwParam;

	HANDLE m_hThread;
	SOCKET m_socket;

	ID2FRAG m_mFragMsgs;
	std::string m_sRconChallenge;

	static DWORD WINAPI GIRLUdpServerThreadFunc(LPVOID lpThis);
	DWORD Run();
	void PostWindowMessage(GIRLUdpMessage* pMsg);
	void DeleteAllMsgs();
	void Stop();
	void Start();

	int HandlePacket(char* pPacket, int nPacketLen);
	int HandleLogModPacket(char* pPacket, int nPacketLen);

	int HandleRegularPacket(char* pPacket, int nPacketLen);
	int HandleFragmentOfPacket(char* pPacket, int nPacketLen);
	int CheckFragment(DWORD dwID, GIRLUdpFragmentedMessage* pMsg);
	void ClearFragMsgMap();

	bool ReadInfo(char*& pPacket, int& nPacketLen, GIRLUdpMessageInfo* pMsg);
	bool ReadPlayers(char*& pPacket, int& nPacketLen, GIRLUdpMessagePlayers* pMsg);
	bool ReadRules(char*& pPacket, int& nPacketLen, GIRLUdpMessageRules* pMsg);

public:
	bool IsLogMod();
	bool SendTo(const char* pPacket, int nPacketLen, const sockaddr_in& addrTo);
	void ChangeRemoteIP(unsigned long uRemoteIP, unsigned short uRemotePort);
	unsigned short GetLocalPort() {return m_uPort;};
	void ReBind(unsigned short uPort);
	void Continue();

	bool Send(LPCTSTR pCmd);
	bool SendLogMod(LPCTSTR pCmd);
	bool SendRcon(LPCTSTR pCmd, LPCTSTR pPassword);
};

#endif // !defined(AFX_GIRLUDPSERVER_H__36C79CC4_050B_49FE_9938_751A3FF0F708__INCLUDED_)
