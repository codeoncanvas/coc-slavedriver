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

#include "cinder/gl/gl.h"
#include "cocSlaveDriverBase.h"

namespace coc {

class Slave : public SlaveDriverBase {
    
public:

	//! Connect to master
    void setup( std::string _multicastIp,  int _port  );

	//! Process received messages
	void update();

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos ) override;

	//! Check if we need to update/render
	bool getHasFrameChanged();

	//! Get last time delta received
	double getTimeDelta();

	//! Get last timeline::appTime received
	double getTimeApp();

	//! Sends then clears messages, must be called after update
	void send() override;

	void cleanup() override;

	uint32_t getLastFrameReceived() { return lastFrameReceived; }

	void 						joinGroup();
	void 						leaveGroup();

	float 						delayJoin = 0;

private:


	uint32_t 					lastFrameReceived = 0;
	double 						lastDeltaReceived = 0;
	double 						lastAppTimeReceived = 0;
	bool 						hasFrameChanged = false;

	asio::ip::udp::endpoint 	udpEndpoint;
	asio::ip::udp::socket 		*udpSocket;

	enum { udpMax = 64 };
	char udpData[udpMax];
	std::string multicastIp;
	bool isJoined = false;
	bool hasJoinedAtStart = false;
	double lastReceivedUdp;

	void						udpRead();
	void 						udpHandleReceive( const asio::error_code &error, size_t bytes_recvd );

    
};//class Slave

}//namespace coc