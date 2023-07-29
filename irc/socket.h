/*
	This file is adopted from Adi Degani's IRC library
	(c) Adi Degani
	For more details, see: http://www.codeguru.com/network/irc.html

	CSBot - An IRC bot for publishing CS and NS games
	Copyright (C) 2001, 2002, 2003, 2004 Ilan Tayary ([NoCt]Yonatanz)
	Email: yonatanz@unitedadmins.com
	Website: http://www.csbot.org

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// socket.h

#ifndef SOCKET_H
#define SOCKET_H

class WinsockInit
	{
	public :
		WSADATA m_wsd;
		int m_iStatus;

		WinsockInit(WORD wVersionRequested = 0x0101);
		~WinsockInit();
	};


class InetAddr : public sockaddr_in
	{
	public :
		InetAddr(WORD wPort = 0);
		InetAddr(LPCTSTR lpszAddress, WORD wPort = 0);
		InetAddr& operator = (LPCTSTR lpszAddress);

	protected :
		void Resolve(LPCTSTR lpszAddress, WORD wPort = 0);
	};


class Socket
	{
	public :
		Socket();
		Socket(SOCKET s);
		Socket(const Socket& s);
		virtual ~Socket();

		bool Create();
		void Close();
		bool Bind(const InetAddr& addr);
		bool Connect(const InetAddr& addr);
		bool Listen();
		Socket Accept(InetAddr& addr);
		int Send(const unsigned char* buf, int cbBuf);
		int Send(const char* fmt, ...);
		int Receive(unsigned char* buf, int cbBuf);
		bool SetOpt(int opt, const char* pBuf, int cbBuf);
		bool GetOpt(int opt, char* pBuf, int& cbBuf);
		operator SOCKET& () const { return (SOCKET&)m_sock; }
		bool IsValid() const { return m_sock != INVALID_SOCKET; }
		operator bool() const { ASSERT(FALSE); return false; }

	protected :
		SOCKET m_sock;

	private :
		bool m_bOwnSocket;
	};


#endif
