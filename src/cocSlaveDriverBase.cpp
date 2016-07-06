/**
 *
 *      ┌─┐╔═╗┌┬┐┌─┐
 *      │  ║ ║ ││├┤
 *      └─┘╚═╝─┴┘└─┘
 *   ┌─┐┌─┐╔╗╔┬  ┬┌─┐┌─┐
 *   │  ├─┤║║║└┐┌┘├─┤└─┐
 *   └─┘┴ ┴╝╚╝ └┘ ┴ ┴└─┘
 *
 * Copyright (c) 2016 Code on Canvas Pty Ltd, http://CodeOnCanvas.cc
 *
 * This software is distributed under the MIT license
 * https://tldrlegal.com/license/mit-license
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code
 *
 **/

#include <cinder/Utilities.h>
#include "cocSlaveDriverBase.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace coc {


void SlaveDriverBase::processBuffer( std::string _incoming )
{
	if (_incoming.length()) receivedStrings.push_back( _incoming );
	while (receivedStrings.size() > receivedStringMax) receivedStrings.pop_front();

	vector<string>	pairs = split( _incoming, ',', true );


	for ( string &s : pairs ) {
		vector<string> pair = split(s,'=');
		if (pair.size()==2) {
			processKeyValuePair( pair[0][0], pair[1] );
		}
		else {
//			CI_LOG_E("Pair incomplete!");
		}

	}
}


SlaveDriverMessage SlaveDriverBase::getNextMessage()
{
	SlaveDriverMessage m = receivedMessages.front();
	receivedMessages.pop_front();
	return m;
}

void SlaveDriverBase::addKeyValuePair( char _key, std::string _value )
{
	msg += _key;
	msg += '=';
	msg += _value;
	msg += ',';
}



}//namespace coc