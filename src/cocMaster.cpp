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
    
void Master::setup( std::string _multicastIp, int _port )
{
    SlaveDriverBase::setup();

    port = _port;

    // UDP

    udpEndpoint = asio::ip::udp::endpoint( asio::ip::address::from_string(_multicastIp), (short) port);
    udpSocket = new asio::ip::udp::socket( app::App::get()->io_service(), udpEndpoint.protocol() );

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
//    string text = "MASTER:\n";
//
//    TextBox textbox;
//    textbox.setText(text);
//    gl::TextureRef tex = gl::Texture::create( textbox.render() );
//    gl::ScopedColor col( Color(1,1,1) );
//    gl::draw(tex, pos);
}



void Master::cleanup()
{

}


}//namespace coc
