// Cinder
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/CinderMath.h"
// Project
#include "Resources.h"
#include "Well.cpp"
#include "Particle.cpp"
// STL
#include <list>
// Boost
#pragma mark Namespaces
using std::list;
using std::vector;
using namespace ci;
using namespace ci::app;

#pragma mark typedefs
typedef boost::shared_ptr<Well> WellPointer;

#pragma mark Constants
static const float	SCREEN_WIDTH			= 768.0f;
static const float	SCREEN_HEIGHT			= 1024.0f;
static const int	NUM_INITIAL_PARTICLES	= 1300;


#pragma mark -
#pragma mark Header
#pragma mark -


class GravitySwarm : public AppCocoaTouch
{
public:	
	void	setup();
	void	startSimulation();
	void	initParticles();
	void	setupOpenGL();
	
	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );
	
	void	update();
	void	updateSimulation();
	void	updateStartScreen();
	
	void	updateParticles();
	void	handleEdges( Particle* partIt );
	void	updateWells();
	
	void	draw();
	void	drawStartScreen();
	void	drawSimulation();
	
	
	Perlin					_perlinNoise;
	list<Particle>			_listOfParticles;
	
	vector<WellPointer>		_listOfWells;
	vector<WellPointer>		_listOfDraggedWells;
	
	ci::Rectf			_windowRect;
	
	gl::Texture		_textureWell;
	gl::Texture		_startScreen;
	
	float	_perlinNoiseZ;
	float	_noiseImpact; // how much noise effects the particles
	float	_noiseOffset; // how fast to move the noise
	
	enum	APP_STATES { STATE_STARTSCREEN, STATE_SIMULATION };
	int		_state;			// internal state
};
