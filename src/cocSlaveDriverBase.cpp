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

	bytesInTcp.getMap()['F'] = coc::KV_INT32;
	bytesInTcp.getMap()['T'] = coc::KV_DOUBLE;
	bytesInTcp.getMap()['S'] = coc::KV_DOUBLE;

	bytesOutTcp = bytesInTcp;
}


}//namespace coc