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

#include <cinder/Utilities.h>
#include "cocSlaveDriverBase.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace coc {

void SlaveDriverBase::setup() {

	bytesInTcp.getMap()['F'] = coc::KV_INT32;	//frame number, used as message ID - must be first in UDP packet
	bytesInTcp.getMap()['T'] = coc::KV_DOUBLE;	//time Delta
	bytesInTcp.getMap()['A'] = coc::KV_DOUBLE;	//absolute time, timeline::appTime

	bytesOutTcp = bytesOutUdp = bytesInUdp = bytesInTcp;
}


}//namespace coc