// GIRLUdpServer.cpp: implementation of the GIRLUdpServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GIRLUdpServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE	8192

GIRLUdpServer::GIRLUdpServer(HWND hWnd, UINT uMsg, unsigned short uPort, unsigned long uRemoteIP, unsigned short uRemotePort, DWORD dwParam) : CIGCLockable("GIRLUdpServer"),
	m_hWnd(hWnd),
	m_uMsg(uMsg),
	m_bStop(true),
	m_bContinue(false),
	m_bStarted(false),
	m_uPort(uPort),
	m_uRemoteIP(htonl(uRemoteIP)),
	m_uRemotePort(htons(uRemotePort)),
	m_uLogmodPort(0),
	m_hThread(NULL),
	m_socket(INVALID_SOCKET),
	m_dwParam(dwParam)
{
	// start our thread using current values of members
	Start();
}

GIRLUdpServer::~GIRLUdpServer()
{
	// stop the thread and remove all leftover messages from queue before we get delete
	Stop();
}

void GIRLUdpServer::Stop()
{
	// make sure thread cannot block anymore
	::shutdown(m_socket, SD_BOTH);
	::closesocket(m_socket);
	// tell thread to stop
	m_socket = INVALID_SOCKET;
	m_bStop = true;
	DWORD dwCode = 0;
	int nTimeout = 20;
	// wait for thread to stop
	do {
		ASSERT(::GetExitCodeThread(m_hThread, &dwCode));
		if (dwCode == STILL_ACTIVE)
		{
			Sleep(500);
			nTimeout--;
		}
	} while ((dwCode == STILL_ACTIVE) && (nTimeout > 0));

	// timeout is a REALLY bad thing here, let's hope it never happens
	ASSERT(nTimeout > 0);
	// close the thread's handle
	::CloseHandle(m_hThread);

	// delete all leftover messages in the window's queue
	DeleteAllMsgs();
	ClearFragMsgMap();
}

void GIRLUdpServer::Start()
{
	// make a new socket to work with
	m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ASSERT(m_socket != INVALID_SOCKET);

	// reset thread-control flags
	m_bStop = false;
	m_bStarted = false;
	m_bContinue = false;

	// start the thread
	DWORD dwThreadID = ::GetCurrentThreadId();
	m_hThread = ::CreateThread(NULL, 0, &GIRLUdpServerThreadFunc, (LPVOID)this, 0, &dwThreadID);
	ASSERT(m_hThread != NULL);

	// wait until the thread completes its initialization
	// we don't want the program to send stuff on the socket before it is bound, etc.
	while (!m_bStarted)
		Sleep(200);
}

void GIRLUdpServer::DeleteAllMsgs()
{
	// we can't do this outside of the window's thread
	ASSERT(GetWindowThreadProcessId(m_hWnd, NULL) == GetCurrentThreadId());
	MSG msg;
	// remove all leftover messages in this thread's message queue
	while (::PeekMessage(&msg, m_hWnd, m_uMsg, m_uMsg, PM_REMOVE))
	{
		// got a message - delete it!
		GIRLUdpMessage* pMsg = (GIRLUdpMessage*)msg.lParam;
		delete pMsg;
	}
}

DWORD GIRLUdpServer::GIRLUdpServerThreadFunc(LPVOID lpThis)
{
	// thread callback func... call the Run() method
	GIRLUdpServer* pThis = (GIRLUdpServer*)lpThis;
	DWORD dwRet = pThis->Run();
	return dwRet;
}

DWORD GIRLUdpServer::Run()
{
	// if needed, bind the socket
	int nError = 0;
	if (m_uPort > 0)
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		// bind to all local interfaces
		addr.sin_addr.S_un.S_addr = 0;
		addr.sin_port = htons(m_uPort);

		nError = ::bind(m_socket, (const sockaddr*)&addr, sizeof(addr));
		if (nError != 0)
		{
			// error binding... a real bummer
			PostWindowMessage(new GIRLUdpMessageError(E_Bind, nError, ::WSAGetLastError()));
			// turn on the bStarted flag, so the main thread doesn't get stuck
			m_bStarted = true;
			return 0;
		}
	}

	char cBuffer[BUFFER_SIZE+1];
	sockaddr_in addrFrom;
	// turn on the bStarted flag, to signal that we're ready to start
	m_bStarted = true;

	while (!m_bContinue && !m_bStop)
		Sleep(200);

	while (!m_bStop)
	{
		int nLen = sizeof(addrFrom);
		// get a packet from the socket (blocking)
		nError = ::recvfrom(m_socket, cBuffer, BUFFER_SIZE, 0, (sockaddr*)&addrFrom, &nLen);
		// handle error...
		if ((nError == SOCKET_ERROR) || (nError < 0))
		{
			PostWindowMessage(new GIRLUdpMessageError(E_RecvFrom, nError, ::WSAGetLastError()));
			return 0;
		}
		// handle graceful close
		if (nError == 0)
			break;

		// figure out our random port if not explicitly set
		if (m_uPort == 0)
		{
			sockaddr_in addrLocal;
			int nSize = sizeof(addrLocal);
			::getsockname(m_socket, (sockaddr*)&addrLocal, &nSize);
			m_uPort = ntohs(addrLocal.sin_port);
		}

		// null-terminate it, for reading strings
		cBuffer[nError] = '\0';

		// verify the origin of the packet. we ignore packets not from our game server
		if ((addrFrom.sin_addr.S_un.S_addr == m_uRemoteIP) && (addrFrom.sin_port == m_uRemotePort))
		{
			// handle the packet in one of many ways...
			nError = HandlePacket(cBuffer, nError);
			// handle errors
			if (nError != 0)
			{
				PostWindowMessage(new GIRLUdpMessageError(E_HandlePacket, nError, 0));
				return 0;
			}
		} 
		else if ((addrFrom.sin_addr.S_un.S_addr == m_uRemoteIP) && (addrFrom.sin_port == m_uLogmodPort))
		{
			// also take care of logmod packets
			nError = HandleLogModPacket(cBuffer, nError);
			// handle errors
			if (nError != 0)
			{
				PostWindowMessage(new GIRLUdpMessageError(E_HandlePacket, nError, 0));
				return 0;
			}
#if _TRACES >= 5
		} else {
			CString sMsg;
			sMsg.Format("Filtered packet from %s:%u, %d bytes", inet_ntoa(addrFrom.sin_addr), ntohs(addrFrom.sin_port), nError);
			PostWindowMessage(new GIRLUdpMessageDebug(sMsg));
#endif
		}
	}

	// let the main thread know that we stop.
	PostWindowMessage(new GIRLUdpMessageError(E_ThreadEnded, 0, 0));
	return 0;
}

void GIRLUdpServer::PostWindowMessage(GIRLUdpMessage* pMsg)
{
	// send the message to the window. what joy!
	pMsg->m_dwParam = m_dwParam;
	::PostMessage(m_hWnd, m_uMsg, (WPARAM)pMsg->GetType(), (LPARAM)pMsg);
}

bool GIRLUdpServer::SendTo(const char* pPacket, int nPacketLen, const sockaddr_in& addrTo)
{
	// send the buffer on the socket.
	return (::sendto(m_socket, pPacket, nPacketLen, 0, (const sockaddr*)&addrTo, sizeof(addrTo)) != SOCKET_ERROR);
}

void GIRLUdpServer::ChangeRemoteIP(unsigned long uRemoteIP, unsigned short uRemotePort)
{
	// change the IP filter settings.
	m_uRemoteIP = htonl(uRemoteIP);
	m_uRemotePort = htons(uRemotePort);
}

void GIRLUdpServer::ReBind(unsigned short uPort)
{
	// rebinding is a rough action.
	// first stop the thread and get rid of leftovers
	Stop();
	// now remember the new port
	m_uPort = uPort;
	// and restart the thread
	Start();
}

char* cHex = "0123456789ABCDEF";

bool ReadPtr(char*& pPacket, int& nPacketLen, void* pBuf, int nLen)
{
	if (nPacketLen < nLen)
		return false;
	memcpy(pBuf, pPacket, nLen);
	pPacket += nLen;
	nPacketLen -= nLen;
	return true;
}

#define ReadVar(__var) (ReadPtr(pPacket, nPacketLen, &__var, sizeof(__var)))

bool ReadStr(char*& pPacket, int& nPacketLen, std::string& sStr)
{
	if ((int)strlen(pPacket) >= nPacketLen)
		return false;
	sStr.assign(pPacket);
	pPacket += sStr.size()+1;
	nPacketLen -= sStr.size()+1;
	return true;
}

int GIRLUdpServer::HandlePacket(char* pPacket, int nPacketLen)
{
#if _TRACES >= 5
	// send a debugging message about this packet
	char cStr[16384];
	int nStrPos = 0;
	sprintf(cStr, "Packet received, %d bytes:\n", nPacketLen);
	nStrPos = strlen(cStr);
	for (int nPos = 0; nPos < nPacketLen; nPos++)
	{
		char c = pPacket[nPos];
		if (((c >= 32) && (c <= 127)) || (c == '\r') || (c == '\n'))
		{
			cStr[nStrPos] = c;	nStrPos++;
		}
		else
		{
			cStr[nStrPos] = '\\'; nStrPos++;
			cStr[nStrPos] = cHex[((unsigned char)c) >> 4]; nStrPos++;
			cStr[nStrPos] = cHex[((unsigned char)c) & 15]; nStrPos++;
			cStr[nStrPos] = ' '; nStrPos++;
		}
	}
	cStr[nStrPos] = '\0';
	PostWindowMessage(new GIRLUdpMessageDebug(cStr));
#endif

	// sane check - we don't do too big messages
	if (nPacketLen > BUFFER_SIZE)
		return 1;

	int nRet;
	// fetch packet type, and handle accordingly
	DWORD dwPacketType;
	if (!ReadVar(dwPacketType))
		return 2;

	switch (dwPacketType)
	{
	case 0xffffffff:
		// full parsable packet
		nRet = HandleRegularPacket(pPacket, nPacketLen);
		break;
	case 0xfffffffe:
		// a fragment of a packet. remember it for later, 
		// when its friend packets arrive, they'll get processed
		nRet = HandleFragmentOfPacket(pPacket, nPacketLen);
		break;
	default:
		// unknown type = error
		return 3;
	}

	return nRet;
}


int GIRLUdpServer::HandleLogModPacket(char* pPacket, int nPacketLen)
{
#if _TRACES >= 5
	// send a debugging message about this packet
	char cStr[16384];
	int nStrPos = 0;
	sprintf(cStr, "LogMod Packet received, %d bytes:\n", nPacketLen);
	nStrPos = strlen(cStr);
	for (int nPos = 0; nPos < nPacketLen; nPos++)
	{
		char c = pPacket[nPos];
		if (((c >= 32) && (c <= 127)) || (c == '\r') || (c == '\n'))
		{
			cStr[nStrPos] = c;	nStrPos++;
		}
		else
		{
			cStr[nStrPos] = '\\'; nStrPos++;
			cStr[nStrPos] = cHex[((unsigned char)c) >> 4]; nStrPos++;
			cStr[nStrPos] = cHex[((unsigned char)c) & 15]; nStrPos++;
			cStr[nStrPos] = ' '; nStrPos++;
		}
	}
	cStr[nStrPos] = '\0';
	PostWindowMessage(new GIRLUdpMessageDebug(cStr));
#endif

	std::string sLogModHeader;
	if (!ReadStr(pPacket, nPacketLen, sLogModHeader))
		return 30;

	if (sLogModHeader.find("LOGFWD -:") != 0)
		return 31;

	unsigned short uSourcePort = (unsigned short)atoi(sLogModHeader.substr(9).c_str());

	return HandlePacket(pPacket, nPacketLen);
}

bool GIRLUdpServer::ReadInfo(char*& pPacket, int& nPacketLen, GIRLUdpMessageInfo* pMsg)
{
	// read server's info from packet
	if (!ReadStr(pPacket, nPacketLen, pMsg->sAddress)) return false;
	if (!ReadStr(pPacket, nPacketLen, pMsg->sHostname)) return false;
	if (!ReadStr(pPacket, nPacketLen, pMsg->sMapName)) return false;
	if (!ReadStr(pPacket, nPacketLen, pMsg->sGame)) return false;
	if (!ReadStr(pPacket, nPacketLen, pMsg->sGameDescription)) return false;
	if (!ReadVar(pMsg->uPlayerCount)) return false;
	if (!ReadVar(pMsg->uMaxPlayers)) return false;
	if (!ReadVar(pMsg->uVersion)) return false;
	return true;
}

bool GIRLUdpServer::ReadPlayers(char*& pPacket, int& nPacketLen, GIRLUdpMessagePlayers* pMsg)
{
	// read players from packet
	unsigned char uCount;
	if (!ReadVar(uCount)) return false;
	for (unsigned char uNum = 0; uNum < uCount; uNum++)
	{
		PlayerInfoStruct p;
		if (!ReadVar(p.uIndex)) return false;
		if (!ReadStr(pPacket, nPacketLen, p.sName)) return false;
		if (!ReadVar(p.lFrags)) return false;
		if (!ReadVar(p.fTime)) return false;
		pMsg->push_back(p);		
	}

	return true;
}

bool GIRLUdpServer::ReadRules(char*& pPacket, int& nPacketLen, GIRLUdpMessageRules* pMsg)
{
	// read rules from packet
	unsigned short uLogmodPort = 0;

	bool bLMStatus = false;
	unsigned short uCount;
	if (!ReadVar(uCount)) return false;
	for (unsigned short uNum = 0; uNum < uCount; uNum++)
	{
		RuleInfoStruct r;
		if (!ReadStr(pPacket, nPacketLen, r.sName)) return false;
		if (!ReadStr(pPacket, nPacketLen, r.sValue)) return false;
		pMsg->push_back(r);

		if (r.sName == "lm_port")
			uLogmodPort = htons(atoi(r.sValue.c_str()));
		else if (r.sName == "lm_status")
			bLMStatus = atoi(r.sValue.c_str()) != 0;
	}

	if (bLMStatus && (uLogmodPort != 0))
	{
		m_uLogmodPort = uLogmodPort;
	} else {
		m_uLogmodPort = 0;
	}

	return true;
}

int GIRLUdpServer::HandleRegularPacket(char* pPacket, int nPacketLen)
{
	// read the packet type
	char cType;
	if (!ReadVar(cType))
		return 6;

	int nRet = 0;
	switch (cType)
	{
	case 'C':
		{
			// server info packet
			GIRLUdpMessageInfo* pMsg = new GIRLUdpMessageInfo;
			if (!ReadInfo(pPacket, nPacketLen, pMsg))
			{
				delete pMsg;
				return 7;
			} else
				PostWindowMessage(pMsg);
		}
		break;
	case 'D':
		{
			// players info packet
			GIRLUdpMessagePlayers* pMsg = new GIRLUdpMessagePlayers;
			if (!ReadPlayers(pPacket, nPacketLen, pMsg))
			{
				delete pMsg;
				return 8;
			} else
				PostWindowMessage(pMsg);
		}
		break;
	case 'E':
		{
			// ruels info packet
			GIRLUdpMessageRules* pMsg = new GIRLUdpMessageRules;
			if (!ReadRules(pPacket, nPacketLen, pMsg))
			{
				delete pMsg;
				return 9;
			} else
				PostWindowMessage(pMsg);
		}
		break;
	case 'l':
		{
			// redirected log (logaddress) or rcon response
			std::string sResponse;
			if (!ReadStr(pPacket, nPacketLen, sResponse)) 
				return 13;
			if (sResponse.find("og ") == 0)
				PostWindowMessage(new GIRLUdpMessageLog(sResponse.substr(3)));
			else
				PostWindowMessage(new GIRLUdpMessageRcon(sResponse));
		}
		break;
	case 'c':
		{
			// rcon's challenge ID
			std::string sChallenge;
			if (!ReadStr(pPacket, nPacketLen, sChallenge)) 
				return 10;
			if (sChallenge.find("hallenge rcon ") != 0)
				return 11;
			sChallenge.erase(0, 14);
			int nPos = sChallenge.find("\n");
			if (nPos == std::string::npos)
				return 12;
			sChallenge.erase(nPos);
			IGCLock lock(this, "Challenge changed");
			m_sRconChallenge = sChallenge;
			PostWindowMessage(new GIRLUdpMessageError(E_RconChallange, 0, 0));
		}
		break;
	case '9':
		// notice about bad rcon challenge
//		TRACE("Bad rcon challenge reported!!\n");
		break;
	default:
		return 4;
	}
	return nRet;
}

int GIRLUdpServer::HandleFragmentOfPacket(char* pPacket, int nPacketLen)
{
	// sane check - we don't do too small messages
	DWORD dwID;
	if (!ReadVar(dwID)) return 14;
	unsigned char cID;
	if (!ReadVar(cID)) return 15;
	int nTotal = (int)cID & 15;
	int nCurrent = (int)cID >> 4;

	GIRLUdpFragmentedMessage* pMsg;
	ID2FRAG::iterator i = m_mFragMsgs.find(dwID);
	if (i == m_mFragMsgs.end())
	{
		pMsg = new GIRLUdpFragmentedMessage;
		m_mFragMsgs[dwID] = pMsg;
		pMsg->Init(dwID, nTotal);
	} else {
		pMsg = (*i).second;
	}

	pMsg->SetFragment(nCurrent, pPacket, nPacketLen);

	return CheckFragment(dwID, pMsg);
}

int GIRLUdpServer::CheckFragment(DWORD dwID, GIRLUdpFragmentedMessage* pMsg)
{
	// check if we have all the fragments
	// and accumulate length while we're at it
	bool bDone = true;
	int nPacketLen = 0;
	for (int nFrag = 0; nFrag < pMsg->m_nTotalFragments; nFrag++)
	{
		// does this fragment exist?
		if (pMsg->m_pFragments[nFrag] == NULL)
		{
			// no - we wanna quit
			bDone = false;
			break;
		}
		// yes - add its length
		nPacketLen += pMsg->m_pFragmentLengths[nFrag];
	}
	// quit if not finished
	if (!bDone)
		return 0;
	
	// make a new buffer for the combined packet
	char* pPacket = new char[nPacketLen];
	int nPos = 0;
	// and combine all fragments to one big packet
	for (nFrag = 0; nFrag < pMsg->m_nTotalFragments; nFrag++)
	{
		// copy this part
		memcpy(pPacket + nPos, pMsg->m_pFragments[nFrag], pMsg->m_pFragmentLengths[nFrag]);
		// and move on
		nPos += pMsg->m_pFragmentLengths[nFrag];
	}

	// now handle it as if it was never fragmented
	int nRet = HandlePacket(pPacket, nPacketLen);
	// and clean up
	delete [] pPacket;

	return nRet;
}

bool GIRLUdpServer::Send(LPCTSTR pCmd)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = m_uRemoteIP;
	addr.sin_port = m_uRemotePort;
	return SendTo(pCmd, strlen(pCmd), addr);
}

bool GIRLUdpServer::SendLogMod(LPCTSTR pCmd)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = m_uRemoteIP;
	addr.sin_port = m_uLogmodPort;
	return SendTo(pCmd, strlen(pCmd), addr);
}

bool GIRLUdpServer::SendRcon(LPCTSTR pCmd, LPCTSTR pPassword)
{
	std::string sCmd = "\xff\xff\xff\xff" "rcon ";

	{
		IGCLock lock(this, "SendRcon");
		sCmd += m_sRconChallenge;
	}

	sCmd += " \"";
	sCmd += pPassword;
	sCmd += "\" ";
	sCmd += pCmd;
	return Send(sCmd.c_str());
}

void GIRLUdpFragmentedMessage::Init(DWORD dwID, int nFragments)
{
	ASSERT(m_dwID == 0);
	ASSERT(m_nTotalFragments == 0);
	ASSERT(m_pFragments == NULL);
	ASSERT(m_pFragmentLengths == NULL);
	ASSERT(nFragments < 16);

	m_dwID = dwID;
	m_nTotalFragments = nFragments;
	m_pFragments = new char*[nFragments];
	m_pFragmentLengths = new int[nFragments];
	for (int nFrag = 0; nFrag < nFragments; nFrag++)
	{
		m_pFragments[nFrag] = NULL;
		m_pFragmentLengths[nFrag] = 0;
	}
}

void GIRLUdpFragmentedMessage::SetFragment(int nFragment, char* pBuf, int nLen)
{
	ASSERT(m_dwID != 0);
	ASSERT(m_nTotalFragments != 0);
	ASSERT(m_pFragments != NULL);
	ASSERT(m_pFragmentLengths != NULL);
	if (nFragment >= m_nTotalFragments)
		return;

	m_pFragments[nFragment] = new char[nLen];
	m_pFragmentLengths[nFragment] = nLen;
	memcpy(m_pFragments[nFragment], pBuf, nLen);
}

GIRLUdpFragmentedMessage::~GIRLUdpFragmentedMessage()
{
	if (m_pFragments != NULL)
	{
		for (int nFrag = 0; nFrag < m_nTotalFragments; nFrag++)
		{
			if (m_pFragments[nFrag] != NULL)
				delete [] m_pFragments[nFrag];
		}
		delete [] m_pFragments;
	}
	if (m_pFragmentLengths != NULL)
		delete [] m_pFragmentLengths;
}

void GIRLUdpServer::ClearFragMsgMap()
{
	for (ID2FRAG::iterator i = m_mFragMsgs.begin(); i != m_mFragMsgs.end(); i++)
		delete (*i).second;

	m_mFragMsgs.clear();
}

void GIRLUdpServer::Continue()
{
	m_bContinue = true;
}

bool GIRLUdpServer::IsLogMod()
{
	return (m_uLogmodPort != 0);
}
