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

#pragma once

#include "cinder/app/App.h"

namespace coc {

class SlaveDriverMessage {
public:
	SlaveDriverMessage( char key, std::string value ) :
			key(key),
			value(value)
	{}
	char key;
	std::string value;
};//class SlaveDriverMessage


class SlaveDriverBase {

public:

	//! Optionally add pairs to frame message
	void addKeyValuePair( char _key, std::string _value);

	//! Check if we need to process messages
	bool hasWaitingMessages() { return receivedMessages.size(); };

	//! Get next message form queue
	SlaveDriverMessage	getNextMessage();

	//! Turn buffer into array of messages
	void processBuffer( std::string _incoming );

	//! Process message
	virtual void processKeyValuePair( char _key, std::string _value ) {};

	virtual void drawDebug( ci::ivec2 pos ) {};

protected:

	std::string						msg = "";
	int 							receivedStringMax = 5;
	std::deque<std::string> 		receivedStrings;
	std::deque<SlaveDriverMessage> 	receivedMessages;
	bool 							disableNagle = true;
	int32_t							port;


};//class cocSlaveDriverBase
}//namespace coc



