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

void Slave::setup( asio::io_service& _ioService, std::string _serverIp, std::string _multicastIp, int _port, int _id ) {

	SlaveDriverBase::setup();

	host		= _serverIp;
	port		= _port;
	slaveId		= _id;
	multicastIp = _multicastIp;

	// TCP

	if (useTcp) {
		client = TcpClient::create( _ioService );

		client->connectConnectEventHandler( &Slave::onConnect, this );
		client->connectErrorEventHandler( &Slave::onError, this );
		client->connectResolveEventHandler( [ & ]()
		{
			CI_LOG_I( "Endpoint resolved" );
		} );

		lastConnectionAttempt = -connectionAttemptInterval;
	}


	// UDP

	udpEndpoint = asio::ip::udp::endpoint( asio::ip::address::from_string("0.0.0.0"), (short) port);
	udpSocket = new asio::ip::udp::socket( _ioService );
	udpSocket->open(udpEndpoint.protocol());
	udpSocket->set_option(asio::ip::udp::socket::reuse_address(true));
	udpSocket->bind(udpEndpoint);

	udpRead();

//	lastReceivedUdp = getElapsedSeconds();
}

void Slave::joinGroup()
{
	if (isJoined) return;

	udpSocket->set_option(
			asio::ip::multicast::join_group(asio::ip::address::from_string(multicastIp)));
	isJoined = true;
	CI_LOG_I("Joined multicast group");
}

void Slave::leaveGroup()
{
	if (!isJoined) return;

	udpSocket->set_option(
			asio::ip::multicast::leave_group(asio::ip::address::from_string(multicastIp)));
	isJoined = false;
	CI_LOG_I("Left multicast group");
}


void Slave::udpHandleReceive( const asio::error_code &error, size_t bytes_recvd )
{
	if (error) {
		CI_LOG_E("UDP error");
	}
	else {
		lastReceivedUdp = getElapsedSeconds();

		//todo: optimise with bytes instead buffer
		ci::BufferRef buf = ci::Buffer::create(udpData, bytes_recvd);

		if (bytesInUdp.getPairs().size()) {
			bytesInUdp.clear();
//			CI_LOG_E("Too slow to process UDP");
		}
		bytesInUdp.processBuffer(buf);

		for ( KeyValByteBase * kv : bytesInUdp.getPairs() ) {


			switch (kv->getKey()) {
				case 'F':
				{
					coc::KeyValByte<int32_t>* tmp = (coc::KeyValByte<int32_t>*) kv;
					uint32_t newFrame = tmp->getValue();

					if (newFrame <= lastFrameReceived) { //duplicate/old packet
						bytesInUdp.clear();
					}
					else {
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
				case 'A':
				{
					coc::KeyValByte<double>* tmp = (coc::KeyValByte<double>*) kv;
					lastAppTimeReceived = tmp->getValue();
				}
					break;
			}

		}


	}

	udpRead();//recursive read to ensure no backlog of messages
}

void Slave::udpRead() {
	udpSocket->async_receive_from(
			asio::buffer(udpData, udpMax), udpEndpoint,
			bind(&Slave::udpHandleReceive, this,
					std::placeholders::_1,//error
					std::placeholders::_2));//bytes_transferred
}

void Slave::connect()
{
	CI_LOG_I( "Connecting to: " + host + ":" + toString( port ) );
	client->connect( host, (uint16_t)port );
}


void Slave::update() {

	if (!isJoined && !hasJoinedAtStart && getElapsedSeconds() > 5) { //delay joining group on launch to avoid delays
		CI_LOG_I("About to join multicast group for first time...");
		joinGroup();
		hasJoinedAtStart = true;
//		lastReceivedUdp = getElapsedSeconds();
	}
	else if (isJoined && getElapsedSeconds() - lastReceivedUdp > 5) { //reset frame if master down
		lastFrameReceived = 0;
	}
//	else if (isJoined && getElapsedSeconds() - lastReceivedUdp > 5) { //try reconnecting to group if no messages
//		CI_LOG_E("Rejoining multicast group");
//		leaveGroup();
//		joinGroup();
//		lastReceivedUdp = getElapsedSeconds();
//	}

	if (useTcp) {
		if (!session && getElapsedSeconds() - lastConnectionAttempt > connectionAttemptInterval) {
			connect();
			lastConnectionAttempt = getElapsedSeconds();
		}
		else if (session && session->getSocket()->is_open()) {

			session->read();
		}
	}


}

void Slave::send()
{
	//TCP

	if (useTcp) {
		bytesInTcp.clear();

		//todo: implement sending

		bytesOutTcp.clear();
	}

	//UDP
	bytesInUdp.clear();

	udpRead();
}



double Slave::getTimeDelta()
{
	return lastDeltaReceived;
}

double Slave::getTimeApp()
{
	return lastAppTimeReceived;
}


void Slave::drawDebug( ci::ivec2 pos )
{

	string text = "SLAVE " + toString(slaveId) + ":\n";
	bool isConnected = false;
	if (session) isConnected = true;
	text += "connected? " + toString( isConnected ) + "\n";
	text += "lastFrame= " + toString( lastFrameReceived ) + "\n";
	text += "lastDelta= " + toString( lastDeltaReceived ) + "\n";
	text += "lastAppTime= " + toString( lastAppTimeReceived ) + "\n";

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
//			case '':
//				break;
		}

	}

}

void Slave::onWrite( size_t bytesTransferred )
{
//	CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}


void Slave::cleanup()
{
	if (session && session->getSocket()->is_open()) {
		session->close();
	}
}

}//namespace coc