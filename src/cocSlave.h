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

#include "TcpClient.h"
#include "cinder/gl/gl.h"

namespace coc {

class MessageForSlave {
public:
	MessageForSlave( char key, std::string value ) :
	key(key),
	value(value)
	{}
	char key;
	std::string value;
};

class Slave {
    
public:

	//! Connect to master
    void setup( asio::io_service& _ioService, std::string _ip, int _port, int _id );

	//! Process received messages
	void update();

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos );

	//! Optionally add pairs to frame message
	void addKeyValuePair( char _key, std::string _value);

	//! Check if we need to update/render
	bool getHasFrameChanged();

	//! Check if we need to process messages
	bool hasWaitingMessages() { return receivedMessages.size(); };

	//! Get next message form queue
	MessageForSlave	getNextMessage();

	//! Get last time delta received
	float getTimeDelta();


private:

	void connect();
	void processKeyValuePair(char _key, std::string _value);
	void write( std::string msg );
	void reply();

	int 						lastFrameReceived = -1;
	float 						lastDeltaReceived = -1;
	bool 						hasFrameChanged = false;
	float						lastConnectionAttempt;
	float 						connectionAttemptInterval = 5.0f;
	std::string					msg = "";
	int							slaveId = -1;
	int 						receivedStringMax = 5;
	std::deque<std::string> 	receivedStrings;
	std::deque<MessageForSlave> receivedMessages;
	TcpClientRef				client;
	TcpSessionRef				session;
	std::string					host;
	int32_t						port;

	void						onConnect( TcpSessionRef _session );
	void						onError( std::string err, size_t bytesTransferred );
	void						onRead( ci::BufferRef buffer );
	void						onWrite( size_t bytesTransferred );

    
};//class Slave

}//namespace coc