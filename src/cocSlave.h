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

class Slave {
    
public:
    
    void setup( asio::io_service& ioService, std::string serverIp, int serverPort, int _slaveId );
	void update();
	void drawDebug( ci::ivec2 pos );
	void write( std::string msg );


private:

	int							slaveId;
	int 						receivedMax = 5;
	std::deque<std::string> 	received;
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