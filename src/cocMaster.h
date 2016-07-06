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


namespace coc{

class Master {
    
public:

	//! Start listening for client connections
    void setup(  asio::io_service& _ioService, int _port );

	//! Optionally add pairs to frame message
	void addKeyValuePair( char _key, std::string _value);

	//! Call to send message
	void update( float _delta );

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos );

    
private:

	void writeToAll( std::string _msg );

	std::string					msg = "";
	int							receivedMax = 5;
	std::deque<std::string> 	received;
    TcpServerRef				server;
    std::vector<TcpSessionRef>	sessions;
    int32_t						port;
	bool 						disableNagle = true;

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
