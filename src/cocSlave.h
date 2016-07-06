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
#include "cocSlaveDriverBase.h"

namespace coc {

class Slave : public SlaveDriverBase {
    
public:

	//! Connect to master
    void setup( asio::io_service& _ioService, std::string _ip, int _port, int _id );

	//! Process received messages
	void update();

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos ) override;

	//! Check if we need to update/render
	bool getHasFrameChanged();

	//! Get last time delta received
	float getTimeDelta();


private:

	void connect();
	void processKeyValuePair(char _key, std::string _value) override;
	void write( std::string msg );
	void reply();

	int 						lastFrameReceived = -1;
	float 						lastDeltaReceived = -1;
	bool 						hasFrameChanged = false;
	float						lastConnectionAttempt;
	float 						connectionAttemptInterval = 5.0f;
	int							slaveId = -1;
	TcpClientRef				client;
	TcpSessionRef				session;
	std::string					host;


	void						onConnect( TcpSessionRef _session );
	void						onError( std::string err, size_t bytesTransferred );
	void						onRead( ci::BufferRef buffer );
	void						onWrite( size_t bytesTransferred );

    
};//class Slave

}//namespace coc