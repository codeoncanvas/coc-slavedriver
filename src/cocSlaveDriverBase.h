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

#include "cinder/app/App.h"
#include "cocKeyValByteManager.h"
#include <asio/asio.hpp>
#include <asio/ip/udp.hpp>

namespace coc {

class SlaveDriverBase {

public:

	void setup();
	void addType( char key, coc::KeyValByteType type );

	virtual void drawDebug( ci::ivec2 pos ) {};
	virtual void send() {};
	virtual void cleanup() {};

	coc::KeyValByteManager & getUdpIn() { return bytesInUdp; }
	coc::KeyValByteManager & getUdpOut() { return bytesOutUdp; }

protected:

	coc::KeyValByteManager			bytesInUdp, bytesOutUdp;

	bool 							disableNagle = false;
	int32_t							port;


};//class cocSlaveDriverBase
}//namespace coc



