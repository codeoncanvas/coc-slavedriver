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

#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cocSlave.h"

using namespace ci;
using namespace std;
using namespace ci::app;

namespace coc{

void Slave::setup( asio::io_service& ioService, std::string serverIp, int serverPort, int _slaveId ) {

	host		= serverIp;
	port		= serverPort;
	slaveId		= _slaveId;

	client = TcpClient::create( ioService );

	client->connectConnectEventHandler( &Slave::onConnect, this );
	client->connectErrorEventHandler( &Slave::onError, this );
	client->connectResolveEventHandler( [ & ]()
	{
		CI_LOG_I( "Endpoint resolved" );
	} );

	CI_LOG_I( "Connecting to: " + host + ":" + toString( port ) );
	client->connect( host, (uint16_t)port );

}

void Slave::update() {
	//
}

void Slave::drawDebug( ci::ivec2 pos )
{

	string text = "SLAVE " + toString(slaveId) + ":\n";
	bool isConnected = false;
	if (session) isConnected = true;
	text += "connected? " + toString( isConnected ) + "\n\n";
	for ( string &s : received) {
		text += s;
		text += '\n';
	}

	TextBox textbox;
	textbox.setText(text);
	gl::TextureRef tex = gl::Texture::create( textbox.render() );
	gl::ScopedColor col( Color(1,1,1) );
	gl::draw(tex, pos);
}

void Slave::write( std::string msg ) {
	if (session && session->getSocket()->is_open()) {
		session->write( TcpSession::stringToBuffer( msg ) );
		CI_LOG_V("Wrote: << msg");
	}
}

void Slave::onConnect( TcpSessionRef _session )
{
	CI_LOG_I( "Connected" );

	session = _session;
	session->connectCloseEventHandler( [ & ]()
	{
		CI_LOG_I( "Disconnected" );
	} );
	session->connectErrorEventHandler( &Slave::onError, this );
	session->connectReadCompleteEventHandler( [ & ]()
	{
		CI_LOG_I( "Read complete" );
	} );
	session->connectReadEventHandler( &Slave::onRead, this );
	session->connectWriteEventHandler( &Slave::onWrite, this );

	session->read();

}

void Slave::onError( string err, size_t bytesTransferred )
{
	string text = "Error";
	if ( !err.empty() ) {
		text += ": " + err;
	}
	CI_LOG_E( text );
}

void Slave::onRead( ci::BufferRef buffer )
{
//	CI_LOG_V( toString( buffer->getSize() ) + " bytes read" );

	string response	= TcpSession::bufferToString( buffer );
	received.push_back( response );
	while (received.size() > receivedMax) received.pop_front();

	session->read();

}

void Slave::onWrite( size_t bytesTransferred )
{
	CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}

}//namespace coc