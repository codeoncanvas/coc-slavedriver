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
    
void Master::setup( asio::io_service& _ioService, int _port )
{
    port = _port;

    server = TcpServer::create( _ioService );

    server->connectAcceptEventHandler( &Master::onAccept, this );
    server->connectCancelEventHandler( &Master::onCancel, this );
    server->connectErrorEventHandler( &Master::onError, this );

    accept();
}


bool Master::allRepliesReceived()
{
    if (getElapsedSeconds() < 3) return true;
    return numReplies == sessions.size();
}


void Master::update( float _delta )
{
    if ( allRepliesReceived()) {
        lastFrameSent = getElapsedFrames();
        addKeyValuePair('F', toString(lastFrameSent) );
        addKeyValuePair('T', toString(_delta) );
        writeToAll( msg );
        msg = "";

        for ( auto session : sessions) session->read();

        numReplies = 0;
    }
    else {
        CI_LOG_E("Not all replies in time, only " << numReplies << " of " << sessions.size() );
    }

}


void Master::drawDebug( ci::ivec2 pos )
{

    string text = "MASTER:\n";
    text += "numSessions = " + toString( sessions.size() ) + "\n";
    int numSessionsConnected = 0;
    for ( auto session : sessions) if (session) numSessionsConnected++;
    text += "numSessionsConnected = " + toString( numSessionsConnected ) + "\n\n";
    for ( string &s : receivedStrings) {
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

void Master::processKeyValuePair( char _key, std::string _value )
{
    switch (_key) {
        case 'F':
            numReplies++;
            break;
        case 'S':
            //
            break;
        default:
            receivedMessages.push_back( SlaveDriverMessage( _key, _value ) );
            break;
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

    string incoming	= TcpSession::bufferToString( buffer );

    processBuffer( incoming );

}

void Master::onReadComplete()
{
//    CI_LOG_I( "Read complete" );
}

void Master::onWrite( size_t bytesTransferred )
{
//        CI_LOG_V( toString( bytesTransferred ) + " bytes written" );
}
    
}//namespace coc
