/*
	CSBot - An IRC bot for publishing CS and NS games
	Copyright (C) 2004 Harry J Walsh
	Email: puzl@jarhedz.com
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

// RegexHelper.h: interface for the RegexHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGEXHELPER_H__34DD8296_A697_48C5_A447_848F5D39596D__INCLUDED_)
#define AFX_REGEXHELPER_H__34DD8296_A697_48C5_A447_848F5D39596D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
extern "C" {
	#include "regex.h"
}
class RegexHelper  
{
public:
	RegexHelper();
	bool setRegex(const char *regex);
	bool matchLine(const char *line);
	const char *clean(const bool *selections, const unsigned int num);

	virtual ~RegexHelper();
private:
	const unsigned int _numRegs;
	const unsigned int _bufferSize;
	regex_t _buffer;
	struct re_registers _regs;
	char _cleaned[1024];
	const char *_line;
	const char *_regex;


};

#endif // !defined(AFX_REGEXHELPER_H__34DD8296_A697_48C5_A447_848F5D39596D__INCLUDED_)
