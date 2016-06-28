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
#include "cocMaster.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace coc{
    
void Master::setup( asio::io_service& _ioService, int _port )
{
    port = _port;

    server = TcpServer::create( _ioService );

    server->connectAcceptEventHandler( &Master::onAccept, this );
    server->connectCancelEventHandler( &Master::onCancel, this );
    server->connectErrorEventHandler( &Master::onError, this );

    accept();
}


void Master::addKeyValuePair( char _key, std::string _value )
{
    msg += _key;
    msg += '=';
    msg += _value;
    msg += ',';
}

void Master::update( float _delta )
{

    addKeyValuePair('F', toString(getElapsedFrames()) );
    addKeyValuePair('T', toString(_delta) );
    writeToAll( msg );
    msg = "";

    for ( auto session : sessions) session->read();
}


void Master::drawDebug( ci::ivec2 pos )
{

    string text = "MASTER:\n";
    text += "numSessions = " + toString( sessions.size() ) + "\n";
    int numSessionsConnected = 0;
    for ( auto session : sessions) if (session) numSessionsConnected++;
    text += "numSessionsConnected = " + toString( numSessionsConnected ) + "\n\n";
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


void Master::writeToAll( string msg ) {

    for ( auto session : sessions) {
        if (session && session->getSocket()->is_open()) {
            session->write( TcpSession::stringToBuffer( msg ) );
//            CI_LOG_V("Wrote: " << msg );
        }
    }


}

void Master::accept()
{
    if ( server ) {

        server->accept( (uint16_t)port );
        CI_LOG_I( "Listening on port " << toString( port ) <<"with max connections " << (int) server->getAcceptor()->max_connections );
    }
}


void Master::onAccept( TcpSessionRef _session )
{
    CI_LOG_I( "Connected" );

    sessions.push_back(_session);
    auto session = sessions.back();
    session->connectCloseEventHandler( [ & ]() {
        CI_LOG_I(  "Session closed" );
    } );
    session->connectErrorEventHandler( &Master::onError, this );
    session->connectReadCompleteEventHandler( &Master::onReadComplete, this );
    session->connectReadEventHandler( &Master::onRead, this );
    session->connectWriteEventHandler( &Master::onWrite, this );

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
    CI_LOG_V( toString( buffer->getSize() ) + " bytes read" );

    string incoming	= TcpSession::bufferToString( buffer );
    if (incoming.length()) received.push_back( incoming );
    while (received.size() > receivedMax) received.pop_front();

//    for ( auto session : sessions) session->read();
}

void Master::onReadComplete()
{
    CI_LOG_I( "Read complete" );
}

void Master::onWrite( size_t bytesTransferred )
{
//        CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}
    
}//namespace coc
