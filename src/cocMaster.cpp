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
    
void Master::setup( asio::io_service& _ioService, std::string _ip, int _port )
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

    clientUdp = UdpClient::create( _ioService );

    clientUdp->connectConnectEventHandler( &Master::onConnect, this );
    clientUdp->connectErrorEventHandler( &Master::onError, this );
//    clientUdp->connectResolveEventHandler( [ & ]()
//    {
//        console()<< "Endpoint resolved"<<endl;
//    } );

    clientUdp->connect( _ip, port);
}

void Master::onConnect( UdpSessionRef session )
{
    sessionUdp = session;
    sessionUdp->connectErrorEventHandler( &Master::onError, this );
//    sessionUdp->connectWriteEventHandler( &Master::onWrite, this );

    sessionUdp->getSocket()->set_option(asio::socket_base::broadcast(true));

//    sessionUdp->connectReadCompleteEventHandler( &Master::onReadComplete, this );
//    sessionUdp->connectReadEventHandler( &Master::onRead, this );

}


bool Master::allRepliesReceived()
{
    if (getElapsedSeconds() < 3) return true;
    return numReplies == sessions.size();
}


void Master::update( float _delta )
{
    lastFrameSent = getElapsedFrames();

    // TCP

    bytesOutTcp.addPair('F', (uint32_t)lastFrameSent );
    bytesOutTcp.addPair('T', (double)_delta );

    writeToAll( bytesOutTcp.getBuffer() );

    bytesOutTcp.clear();

    for ( auto session : sessions) session->read();


    // UDP

    bytesOutUdp.addPair('F', (uint32_t)lastFrameSent );
    bytesOutUdp.addPair('T', (double)_delta );

    if ( sessionUdp && sessionUdp->getSocket()->is_open() ) {
        sessionUdp->write( bytesOutUdp.getBuffer() );
    }

    bytesOutUdp.clear();



//    if ( allRepliesReceived()) {
//          //process
//
//        numReplies = 0;
//    }
//    else {
//        CI_LOG_E("Not all replies in time, only " << numReplies << " of " << sessions.size() );
//    }

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
//            case 'F':
//            {
//                coc::KeyValByte<int32_t>* tmp = (coc::KeyValByte<int32_t>*) kv;
//            }
//                break;
//            case 'T':
//            {
//                coc::KeyValByte<double>* tmp = (coc::KeyValByte<double>*) kv;
//            }
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
    
}//namespace coc
