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

namespace coc{

class Master {
    
public:
    
    void setup(  asio::io_service& ioService, int serverPort );
	void update();
	void drawDebug( ci::ivec2 pos );
	void write( std::string msg );


    
private:

	int							receivedMax = 5;
	std::deque<std::string> 	received;
    TcpServerRef				server;
    std::vector<TcpSessionRef>	sessions;
    int32_t						port;

	void						listen();
    void						onAccept( TcpSessionRef _session );
    void						onCancel();
    void						onClose();
    void						onError( std::string err, size_t bytesTransferred );
    void						onRead( ci::BufferRef buffer );
    void						onReadComplete();
    void						onWrite( size_t bytesTransferred );
    
};//class Master

}// namespace coc
