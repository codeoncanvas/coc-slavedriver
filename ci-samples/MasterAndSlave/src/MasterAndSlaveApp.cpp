#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Text.h"

#include "cocMaster.h"
#include "cocSlave.h"

#define NUM_SLAVES	5

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
    coc::Slave   slaves[NUM_SLAVES];

    ci::params::InterfaceGlRef gui;
    int fps;
};

void MasterAndSlaveApp::setup()
{
    int port = 20001;
    master.setup( io_service(), port );

	for (int i=0; i<NUM_SLAVES; i++) {
		slaves[i].setup( io_service(), "127.0.0.1", port, i );
	}



    gui = params::InterfaceGl::create( "Params", ivec2( 200, 110 ) );
    gui->addParam( "Frame rate", &fps, "", true );

	setWindowSize(640,640);
}

void MasterAndSlaveApp::mouseDown( MouseEvent event )
{
}

void MasterAndSlaveApp::update()
{
    fps = getAverageFps();

	master.update();
	for (int i=0; i<NUM_SLAVES; i++) slaves[i].update();

}

void MasterAndSlaveApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color( 1, 1, 1 ) );

	master.drawDebug( ivec2(0,getWindowHeight()/2) );
	for (int i=0; i<NUM_SLAVES; i++) slaves[i].drawDebug( ivec2(getWindowWidth()/2,i*120) );

    gui->draw();

}

CINDER_APP( MasterAndSlaveApp, RendererGl )
