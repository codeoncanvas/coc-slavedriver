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

void Slave::setup( std::string _multicastIp, int _port ) {

	SlaveDriverBase::setup();

	port		= _port;
	multicastIp = _multicastIp;

	// UDP

	udpEndpoint = asio::ip::udp::endpoint( asio::ip::address::from_string("0.0.0.0"), (short) port);
	udpSocket = new asio::ip::udp::socket( app::App::get()->io_service() );
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
		CI_LOG_E("UDP error: " << error.message() );
	}
	else {
		lastReceivedUdp = getElapsedSeconds();

		//todo: optimise with bytes instead buffer?
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

		udpRead();//recursive read to ensure no backlog of messages

	}


}

void Slave::udpRead() {
	udpSocket->async_receive_from(
			asio::buffer(udpData, udpMax), udpEndpoint,
			bind(&Slave::udpHandleReceive, this,
					std::placeholders::_1,//error
					std::placeholders::_2));//bytes_transferred
}


void Slave::update() {

	if (!isJoined && !hasJoinedAtStart && getElapsedSeconds() > delayJoin) { //delay joining group on launch to avoid delays
		CI_LOG_I("About to join multicast group for first time...");
		joinGroup();
		hasJoinedAtStart = true;
//		lastReceivedUdp = getElapsedSeconds();
	}
	else if (isJoined && getElapsedSeconds() - lastReceivedUdp > 5) { //reset frame if master down
		lastFrameReceived = 0;
	}

}

void Slave::send()
{

	//UDP
	bytesInUdp.clear();

//	udpRead();
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

	string text = "SLAVE:\n";
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

void Slave::cleanup()
{
	//
}

}//namespace coc