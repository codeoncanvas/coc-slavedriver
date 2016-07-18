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
#include "cinder/Text.h"
#include "cinder/app/App.h"
#include "cocMaster.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace coc{
    
void Master::setup( asio::io_service& _ioService, std::string _multicastIp, int _port )
{
    SlaveDriverBase::setup();

    port = _port;

    // TCP

    serverTcp = TcpServer::create( _ioService );

    serverTcp->connectAcceptEventHandler( &Master::onAccept, this );
    serverTcp->connectCancelEventHandler( &Master::onCancel, this );
    serverTcp->connectErrorEventHandler( &Master::onError, this );

    accept();

    // UDP

    udpEndpoint = asio::ip::udp::endpoint( asio::ip::address::from_string(_multicastIp), (short) port);
    udpSocket = new asio::ip::udp::socket( _ioService, udpEndpoint.protocol() );

}


void Master::update( double _delta, double _appTime )
{

    // UDP

    lastFrameSent = getElapsedFrames();

    bytesOutUdp.addPair('F', (uint32_t)lastFrameSent );
    bytesOutUdp.addPair('T', _delta );
    bytesOutUdp.addPair('A', _appTime );

}


void Master::send()
{
    // TCP

    bytesInTcp.clear();

    if (bytesOutTcp.getPairs().size()) { //if pairs have been added before update called
        writeToAll( bytesOutTcp.getBuffer() );
        bytesOutTcp.clear();
    }

    for ( auto session : sessions) session->read();

    // UDP

    bytesInUdp.clear();

    udpSend();

    bytesOutUdp.clear();
}


void Master::udpHandleSend( const asio::error_code &error )
{
    if (error) CI_LOG_E("UDP error");
}

void Master::udpSend()
{

    udpSocket->async_send_to(
            asio::buffer( bytesOutUdp.getBuffer()->getData(), bytesOutUdp.getBuffer()->getSize() ),//todo: optimise
            udpEndpoint,
            bind( &Master::udpHandleSend, this,
                    std::placeholders::_1 ));//error

}


void Master::drawDebug( ci::ivec2 pos )
{

    string text = "MASTER:\n";
    text += "numSessions = " + toString( sessions.size() ) + "\n";
    int numSessionsConnected = 0;
    for ( auto session : sessions) if (session) numSessionsConnected++;
    text += "numSessionsConnected = " + toString( numSessionsConnected ) + "\n\n";


    TextBox textbox;
    textbox.setText(text);
    gl::TextureRef tex = gl::Texture::create( textbox.render() );
    gl::ScopedColor col( Color(1,1,1) );
    gl::draw(tex, pos);
}


void Master::writeToAll( BufferRef _buf ) {

    for ( auto session : sessions) {
        if (session && session->getSocket()->is_open()) {
            session->write( _buf );
//            CI_LOG_V("Wrote buffer");
        }
    }


}


void Master::accept()
{
    if ( serverTcp ) {

        serverTcp->accept( (uint16_t)port );
        CI_LOG_I( "Listening on port " << toString( port ) <<"with max connections " << (int) serverTcp->getAcceptor()->max_connections );
    }
}


void Master::onAccept( TcpSessionRef _session )
{
    CI_LOG_I( "Connected" );

    sessions.push_back(_session);
    auto session = sessions.back();

    if (disableNagle) {
        asio::ip::tcp::no_delay option(true);
        session->getSocket()->set_option(option);
    }

    session->connectCloseEventHandler( [ & ]() {
        CI_LOG_I(  "Session closed" );
    } );
    session->connectErrorEventHandler( &Master::onError, this );
//    session->connectReadCompleteEventHandler( &Master::onReadComplete, this );
    session->connectReadEventHandler( &Master::onRead, this );
//    session->connectWriteEventHandler( &Master::onWrite, this );

//    session->read();

}

void Master::onCancel()
{
    CI_LOG_I( "Canceled" );

    accept();
}

void Master::onClose()
{
    CI_LOG_I( "Disconnected" );

    accept();
}

void Master::onError( string err, size_t bytesTransferred )
{
    string text = "Error";
    if ( !err.empty() ) {
        text += ": " + err;
    }
    CI_LOG_E( text );
}

void Master::onRead( BufferRef buffer )
{
//    CI_LOG_V( toString( buffer->getSize() ) + " bytes read" );

    bytesInTcp.processBuffer(buffer);

//    for ( KeyValByteBase * kv : bytesInTcp.getPairs() ) {
//
//        switch (kv->getKey()) {
//            case '':
//                break;
//        }
//
//    }

}

void Master::onReadComplete()
{
//    CI_LOG_I( "Read complete" );
}

void Master::onWrite( size_t bytesTransferred )
{
        CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}


void Master::cleanup()
{
    for ( auto session : sessions) {
        if (session && session->getSocket()->is_open()) {
            session->close();
        }
    }

}


}//namespace coc
