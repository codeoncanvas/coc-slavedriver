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


namespace coc{

class Master : public SlaveDriverBase {
    
public:

	//! Start listening for client connections
    void setup( std::string _multicastIp, int _port );

	//! Call to send message
	void update( double _delta, double _appTime );

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos ) override;

	//! Sends then clears messages, must be called after update
	void send() override;

	void cleanup() override;
    
private:

	int32_t						lastFrameSent = -1;

	asio::ip::udp::endpoint 	udpEndpoint;
	asio::ip::udp::socket 		*udpSocket;

	void 						udpHandleSend( const asio::error_code &error );
	void 						udpSend();

    
};//class Master

}// namespace coc
