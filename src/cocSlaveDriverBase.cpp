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

	addType('F', coc::KV_INT32);	//frame number, used as message ID - must be first in UDP packet
	addType('T',coc::KV_DOUBLE);	//time Delta
	addType('A', coc::KV_DOUBLE);	//absolute time, timeline::appTime

}


void SlaveDriverBase::addType( char key, coc::KeyValByteType type )
{
	bytesInUdp.getMap()[key] = type;
	bytesOutUdp.getMap()[key] = type;
}


}//namespace coc