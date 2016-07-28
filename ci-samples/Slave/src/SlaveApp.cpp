#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "cocSlave.h"

#define PORT				20001
#define MULTICAST_IP		"239.255.0.1"
#define SLAVE_ID            1

using namespace ci;
using namespace ci::app;
using namespace std;

class SlaveApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void cleanup() override;
    
    coc::Slave   slave;
    
    ci::params::InterfaceGlRef gui;
    int fps;
    
    float lastTime;
    float delta;
};

void SlaveApp::setup()
{
    
    slave.setup( io_service(), "127.0.0.1", MULTICAST_IP, PORT, SLAVE_ID );
    
    gui = params::InterfaceGl::create( "Params", ivec2( 200, 110 ) );
    gui->addParam( "Frame rate", &fps, "", true );
    
    setWindowSize(640,640);
    
    lastTime = getElapsedSeconds();
}

void SlaveApp::mouseDown( MouseEvent event )
{
    
}

void SlaveApp::update()
{
    
    fps = getAverageFps();
    
    slave.update();
    slave.send();
    
}

void SlaveApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color( 1, 1, 1 ) );
    
    slave.drawDebug( ivec2(getWindowWidth()/2,0) );
    
    gui->draw();
    
}

void SlaveApp::cleanup()
{
    slave.cleanup();
}

CINDER_APP( SlaveApp, RendererGl )
