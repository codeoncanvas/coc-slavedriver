#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Text.h"

#include "cocMaster.h"
#include "cocSlave.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MasterAndSlaveApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    coc::Master  master;
    coc::Slave   slave;

    ci::params::InterfaceGlRef gui;
    int fps;
};

void MasterAndSlaveApp::setup()
{
    int port = 20001;
    master.setup( io_service(), port );
    slave.setup( io_service(), "127.0.0.1", port, 0 );

    gui = params::InterfaceGl::create( "Params", ivec2( 200, 110 ) );
    gui->addParam( "Frame rate", &fps, "", true );
}

void MasterAndSlaveApp::mouseDown( MouseEvent event )
{
}

void MasterAndSlaveApp::update()
{
    fps = getAverageFps();

	master.update();
	slave.update();

}

void MasterAndSlaveApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color( 1, 1, 1 ) );

	master.drawDebug( ivec2(0,getWindowHeight()/2) );
	slave.drawDebug( ivec2(getWindowWidth()/2,getWindowHeight()/2) );

    gui->draw();

}

CINDER_APP( MasterAndSlaveApp, RendererGl )
