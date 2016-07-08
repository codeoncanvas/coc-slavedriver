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

namespace coc {

class SlaveDriverBase {

public:

	void setup();

	virtual void drawDebug( ci::ivec2 pos ) {};

protected:

	coc::KeyValByteManager			bytesInTcp, bytesOutTcp;

	bool 							disableNagle = false;
	int32_t							port;


};//class cocSlaveDriverBase
}//namespace coc



