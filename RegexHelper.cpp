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

// RegexHelper.cpp: implementation of the RegexHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegexHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RegexHelper::RegexHelper()
:	_numRegs(30),
	_bufferSize(64)
{
	memset((void *)&_regs, 0, sizeof _regs);
	memset((void *)&_buffer, 0, sizeof _buffer);
	_buffer.buffer=new unsigned char[_bufferSize];
	_buffer.allocated=_bufferSize;
}

bool RegexHelper::setRegex(const char *regex) {
	bool ret=false;
	if ( regex != 0 ) {
		_regex=regex;
		if ( re_compile_pattern(_regex, strlen(_regex), &_buffer) == NULL ) {
			ret=true;
		}
	}
	return ret;
}

bool RegexHelper::matchLine(const char *line) {
	bool ret=false;
	if ( line != 0 ) {
		_line=line;
		ret=( re_search(&_buffer, _line, strlen(_line), 0, strlen(_line), &_regs) >= 0);
	}
	return ret;
}

const char *RegexHelper::clean(const bool *selections, const unsigned int num) {
	memset((void *)_cleaned, 0, 1024);
	if ( num == 0 ) {
		for ( int i=_regs.start[0]; i < _regs.end[0] && i < 1024; i++) {
				_cleaned[i]=_line[i];
		}
	}
	else {
		int l=0;
		for (	unsigned int i=1; 
				i < _numRegs && _regs.start[i] != -1 && _regs.end[i] != -1 && l < 1024; 
				i++ ) {
			if ( (i-1) < num && selections[i-1] ) {
				for ( int j=_regs.start[i]; j < _regs.end[i]; j++ ) {
					_cleaned[l++]=_line[j];
				}	
			}	
		}
	}
	return _cleaned;
}

RegexHelper::~RegexHelper()
{
	_buffer.allocated=0;
	delete _buffer.buffer;
}

