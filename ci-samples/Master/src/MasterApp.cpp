#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "cocMaster.h"
#include "cocSlave.h"

#define PORT				20001
#define MULTICAST_IP		"239.255.0.1"

using namespace ci;
using namespace ci::app;
using namespace std;

class MasterApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    coc::Master  master;
    
    ci::params::InterfaceGlRef gui;
    int fps;
    
    float lastTime;
    float delta;
};

void MasterApp::setup()
{
    master.setup( io_service(), MULTICAST_IP, PORT );

    gui = params::InterfaceGl::create( "Params", ivec2( 200, 110 ) );
    gui->addParam( "Frame rate", &fps, "", true );
    
    setWindowSize(640,640);
    
    lastTime = getElapsedSeconds();
}

void MasterApp::mouseDown( MouseEvent event )
{
    
}

void MasterApp::update()
{
    
    fps = getAverageFps();
    
    
    delta = getElapsedSeconds() - lastTime;
    master.update(delta, getElapsedSeconds());
    lastTime = getElapsedSeconds();
    
    
}

void MasterApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::color( Color( 1, 1, 1 ) );
    
    master.drawDebug( ivec2(getWindowWidth()/2,getWindowHeight()/2) );
    
    gui->draw();
    
}

CINDER_APP( MasterApp, RendererGl )
