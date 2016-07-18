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

#include "TcpServer.h"
#include "cinder/gl/gl.h"
#include "cocSlaveDriverBase.h"


namespace coc{

class Master : public SlaveDriverBase {
    
public:

	//! Start listening for client connections
    void setup(  asio::io_service& _ioService, std::string _multicastIp, int _port );

	//! Call to send message
	void update( double _delta, double _appTime );

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos ) override;

	//! Sends then clears messages, must be called after update
	void send() override;

	void cleanup() override;
    
private:

	void writeToAll( ci::BufferRef _buf );

    TcpServerRef				serverTcp;
	std::vector<TcpSessionRef>	sessions;

	int32_t						lastFrameSent = -1;

	asio::ip::udp::endpoint 	udpEndpoint;
	asio::ip::udp::socket 		*udpSocket;

	void 						udpHandleSend( const asio::error_code &error );
	void 						udpSend();

	void						accept();
    void						onAccept( TcpSessionRef _session );
    void						onCancel();
    void						onClose();
    void						onError( std::string err, size_t bytesTransferred );
    void						onRead( ci::BufferRef buffer );
    void						onReadComplete();
	void						onWrite( size_t bytesTransferred );
    
};//class Master

}// namespace coc
