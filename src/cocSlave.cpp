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
#include "cinder/app/App.h"
#include "cocSlave.h"

using namespace ci;
using namespace std;
using namespace ci::app;

namespace coc{

void Slave::setup( asio::io_service& _ioService, std::string _ip, int _port, int _id ) {

	SlaveDriverBase::setup();

	host		= _ip;
	port		= _port;
	slaveId		= _id;

	client = TcpClient::create( _ioService );

	client->connectConnectEventHandler( &Slave::onConnect, this );
	client->connectErrorEventHandler( &Slave::onError, this );
	client->connectResolveEventHandler( [ & ]()
	{
		CI_LOG_I( "Endpoint resolved" );
	} );

	lastConnectionAttempt = -connectionAttemptInterval;
}


void Slave::connect()
{
	CI_LOG_I( "Connecting to: " + host + ":" + toString( port ) );
	client->connect( host, (uint16_t)port );
}


void Slave::update() {

	if (!session && getElapsedSeconds() - lastConnectionAttempt > connectionAttemptInterval) {
		connect();
		lastConnectionAttempt = getElapsedSeconds();
	}
	else if (session) {

		session->read();
	}

}


float Slave::getTimeDelta()
{
	return lastDeltaReceived;
}


void Slave::drawDebug( ci::ivec2 pos )
{

	string text = "SLAVE " + toString(slaveId) + ":\n";
	bool isConnected = false;
	if (session) isConnected = true;
	text += "connected? " + toString( isConnected ) + "\n";
	text += "lastFrame= " + toString( lastFrameReceived ) + "\n";
	text += "lastDelta= " + toString( lastDeltaReceived ) + "\n";

	TextBox textbox;
	textbox.setText(text);
	gl::TextureRef tex = gl::Texture::create( textbox.render() );
	gl::ScopedColor col( Color(1,1,1) );
	gl::draw(tex, pos);
}

bool Slave::getHasFrameChanged()
{
	bool b = hasFrameChanged;
	hasFrameChanged = false;
	return b;
}


void Slave::reply()
{

	bytesOutTcp.addPair('S',(uint32_t)slaveId);
	bytesOutTcp.addPair('F',(double)lastFrameReceived);
	write(bytesOutTcp.getBuffer());
	bytesOutTcp.clear();

}


void Slave::write( BufferRef _buf ) {
	if (session && session->getSocket()->is_open()) {
		session->write( _buf );
//		CI_LOG_V("Wrote buffer");
	}
}

void Slave::onConnect( TcpSessionRef _session )
{
	CI_LOG_I( "Connected" );

	session = _session;

	if (disableNagle) {
		asio::ip::tcp::no_delay option(true);
		session->getSocket()->set_option(option);
	}

	session->connectCloseEventHandler( [ & ]()
	{
		CI_LOG_E( "Disconnected" );
	} );
	session->connectErrorEventHandler( &Slave::onError, this );
	session->connectReadEventHandler( &Slave::onRead, this );
	session->read();

//	session->connectWriteEventHandler( &Slave::onWrite, this );
	//	session->connectReadCompleteEventHandler( [ & ]()
//	{
//		CI_LOG_I( "Read complete" );
//	} );
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

	bytesInTcp.processBuffer( buffer );

	for ( KeyValByteBase * kv : bytesInTcp.getPairs() ) {

		switch (kv->getKey()) {
			case 'F':
			{
				coc::KeyValByte<int32_t>* tmp = (coc::KeyValByte<int32_t>*) kv;
				uint32_t newFrame = tmp->getValue();
				if (newFrame != lastFrameReceived) {
					hasFrameChanged = true;
					lastFrameReceived = newFrame;
				}
			}
				break;
			case 'T':
			{
				coc::KeyValByte<double>* tmp = (coc::KeyValByte<double>*) kv;
				lastDeltaReceived = tmp->getValue();
			}
				break;
		}

	}

	bytesInTcp.clear();

	reply();
}

void Slave::onWrite( size_t bytesTransferred )
{
//	CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}

}//namespace coc