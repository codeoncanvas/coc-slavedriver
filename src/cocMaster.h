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
    void setup(  asio::io_service& _ioService, std::string _ip, int _port );

	//! Call to send message
	void update( float _delta );

	//! Draw debug text to screen
	void drawDebug( ci::ivec2 pos ) override;

    
private:

	void writeToAll( ci::BufferRef _buf );
	bool allRepliesReceived();

    TcpServerRef				serverTcp;
	std::vector<TcpSessionRef>	sessions;

	UdpClientRef    			clientUdp;
	UdpSessionRef   			sessionUdp;

	int32_t						lastFrameSent = -1;
	int 						numReplies = 0;

	void 						onConnect( UdpSessionRef session );
//	void						onWriteUdp( size_t bytesTransferred ) { ci::app::console()<<"wrote udp bytes "<<bytesTransferred<<std::endl; };


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
