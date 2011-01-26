#include "GravitySwarm.h"

#define RANDOM_0_1() ((random() / (float)0x7fffffff ))

#pragma mark Setup
void GravitySwarm::setup()
{
	srandom( time(NULL) );
	
	_perlinNoise.setSeed( random() );
	_perlinNoiseZ = 0;
	
	_noiseImpact = 0.6f;
	_noiseOffset = 0.0010;
	
	_state = STATE_STARTSCREEN;
	setupOpenGL();
}

void GravitySwarm::startSimulation() 
{
	_state = STATE_SIMULATION;
	
	int buffer = 20;
	_windowRect = Rectf(buffer, buffer, SCREEN_WIDTH - (buffer*2), SCREEN_HEIGHT - (buffer*2) );
	
	gl::clear( Color( 0, 0, 0), false );
	gl::enableAdditiveBlending();
	setupParticles();
	
	WellPointer well = WellPointer(new Well(getWindowCenter(), 999) );
	_listOfWells.push_back( well );
}

void GravitySwarm::setupParticles()
{
	_deadParticleCount = 0;
	_deadParticleMax = 1;
	
	// Explode from the center 
	float arc = (2.0f * M_PI) / (float) NUM_INITIAL_PARTICLES;
	Vec2f pos = Vec2f( SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f );
	float speed = 9.0f;
	for (int i = 0; i <	NUM_INITIAL_PARTICLES; i++)
	{
		float mySpeed = speed + Rand::randFloat(-2.0f, 2.0f);
		
		_listOfParticles.push_back( Particle( pos, 1.0f ) );
		Particle* closestParticle = &_listOfParticles.back();
		closestParticle->mVelocity = Vec2f( cosf( (float) i * arc) * mySpeed, sinf( (float) i * arc) * mySpeed );
		
		// Randomly send this particle in the opposite direction to make the fireworks explosion look less uniform
		if(Rand::randBool()) closestParticle->mVelocity *= Rand::randFloat(0.8f, 1.1f) * -1.0f;
	}
}

void GravitySwarm::setupOpenGL()
{
	_textureWell = gl::Texture( loadImage( loadResource( RES_IMAGE_GRAVITYWELL ) ) );
	_startScreen = gl::Texture( loadImage( loadResource( RES_IMAGE_START ) ) );
	
	gl::enableAdditiveBlending();
	glDisable( GL_TEXTURE_2D );
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glOrthof(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable( GL_LINE_SMOOTH );
	glLineWidth(4);
	gl::setMatricesWindow(Vec2f( SCREEN_WIDTH, SCREEN_HEIGHT), true);
}

#pragma mark Touch Handling
void GravitySwarm::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	{
		bool isTouchingAWell = false;
		for ( vector<WellPointer>::iterator wp = _listOfWells.begin(); wp != _listOfWells.end(); ++wp) 
		{
			if(touchIt->getPos().distance((*wp)->mTouchPosition) < 25)
			{
				isTouchingAWell = true;
				_listOfDraggedWells.push_back( (*wp) );
				break;
			}
		}
		
		// Not touching anything, prob wanna make a new one
		if(!isTouchingAWell)
		{
			Vec2f touchLocation = touchIt->getPos();
			
			WellPointer well = WellPointer(new Well(touchLocation, touchIt->getId()) );
			_listOfWells.push_back( well );
			_listOfDraggedWells.push_back( well );
		}
	}
}

void GravitySwarm::touchesMoved( TouchEvent event ) 
{
	//	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	//	{
	//		for( vector<WellPointer>::iterator wp = _listOfWells.begin(); wp != _listOfWells.end(); ++wp)
	//		{
	//			if( (*wp)->mTouchID == touchIt->getId())
	//			{
	//				(*wp)->mLastTouchPosition = (*wp)->mTouchPosition;
	//				(*wp)->mTouchPosition = touchIt->getPos();
	//				
	//				if( _windowRect.contains( (*wp)->mTouchPosition ) == false ) 
	//				{
	//					(*wp)->mIsDead = true;
	//					break;
	//				}
	//				
	//				break;
	//			}
	//		}
	//	}
}

void GravitySwarm::touchesEnded( TouchEvent event )
{
	
	//	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
	//	{
	//		for(list<Well>::iterator wellIt = _listOfWells.begin(); wellIt != _listOfWells.end(); ++wellIt)
	//		{
	//			if(wellIt->mTouchID == touchIt->getId())
	//			{
	//				wellIt->mIsDead = true;
	//				break;
	//			}
	//		}
	//	}
	
	if( _state == STATE_STARTSCREEN ) {
		startSimulation();
	}
}


#pragma mark Update
void GravitySwarm::update()
{
	if( _state == STATE_STARTSCREEN ) {
		updateStartScreen();
	} else if ( _state == STATE_SIMULATION ) {
		updateSimulation();
	}
}

void GravitySwarm::updateSimulation()
{
	_perlinNoiseZ += _noiseOffset; // move ahead in time, which becomes the z-axis of our 3D noise
	updateParticles();
	updateWells();
}

void GravitySwarm::updateStartScreen()
{
}

void GravitySwarm::updateParticles()
{
	float TWO_PI = M_PI*2;
	for( list<Particle>::iterator partIt = _listOfParticles.begin(); partIt != _listOfParticles.end();)
	{
		// You be dead. - Reset
		if( partIt->mAge++ >= partIt->mLifespan ) {
			partIt->reset(Vec2f( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) ) );
			continue;
		}
		
		//		int wellCount = 0;
		
		//		for( vector<WellPointer>::iterator wp = _listOfWells.begin(); wp != _listOfWells.begin(); ++wp)
		//		{
		//			Vec2f direction = (*wp)->mPosition - partIt->mPosition;
		//			float distanceSquared = direction.lengthSquared();
		//			
		//			if( distanceSquared > (*wp)->minDistanceSquared)
		//			{	
		//				float distance = sqrtf( distanceSquared );
		//				
		//				// normalize
		//				float inScale = (*wp)->mStrengthSquared / distance * partIt->gravityForceScale;
		//				float invs = inScale / distance;
		//				direction *= invs;
		//				
		//				partIt->mVelocity += direction;
		//			}
		//			wellCount++;
		//		}
		
		
		// Add some perlin noise to the velocity
		float noiseDownsample = 0.002;
		
//		partIt->noise = _perlinNoise.fBm( Vec3f( partIt->mPosition.x, partIt->mPosition.y, _perlinNoiseZ ) * 0.00065f );
		partIt->noise = _perlinNoise.noise(partIt->mPosition.x*noiseDownsample, partIt->mPosition.y*noiseDownsample, _perlinNoiseZ);
		float angle = partIt->noise* TWO_PI;
		Vec2f noiseVector( cosf( angle ) , sinf( angle ) );
		partIt->mVelocity += (noiseVector * _noiseImpact);
		partIt->tick();
		
		handleEdges( &*partIt );
		++partIt;
	}
}

void GravitySwarm::handleEdges( Particle* partIt )
{
	// Bounce the particles around, or wrap around the screen
	float bounce = Rand::randFloat();
	if( bounce < 1.85 ) // Wrap
	{
		if(partIt->mPosition.x < 0) {
			partIt->mPosition.x = partIt->mLastPosition.x = partIt->mLastLastPosition.x = getWindowWidth() - 1;
			
		}
		else if (partIt->mPosition.x > getWindowWidth()) {
			partIt->mPosition.x = partIt->mLastPosition.x = partIt->mLastLastPosition.x =  1; 
		}
		if (partIt->mPosition.y > getWindowHeight()) { 
			partIt->mPosition.y = partIt->mLastPosition.y = partIt->mLastLastPosition.y =  1; 
		}
		else if (partIt->mPosition.y < 0) { 
			partIt->mPosition.y = partIt->mLastPosition.y = partIt->mLastLastPosition.y = getWindowHeight() - 1;
		}
	} else // Bounce and lose some energy
	{
		float buffer = 10.0f;
		float bounceFactor = 0.8f;
		
		if(partIt->mPosition.x < 0) {partIt->mPosition.x = buffer; partIt->mVelocity.x *= -bounceFactor;}
		else if (partIt->mPosition.x > getWindowWidth()) {partIt->mPosition.x = getWindowWidth() - buffer; partIt->mVelocity.x *= -bounceFactor;}
		if (partIt->mPosition.y > getWindowHeight()) { partIt->mPosition.y = getWindowHeight() - buffer; partIt->mVelocity.y *= -bounceFactor;}
		else if (partIt->mPosition.y < 0) { partIt->mPosition.y = buffer; partIt->mVelocity.y *= -bounceFactor;}
	}
}
void GravitySwarm::updateWells()
{
	// Kill off wells over time
	float elapsedFrames = (float) getElapsedFrames() * 0.1f;
	
	for(vector<WellPointer>::iterator wp = _listOfWells.begin(); wp != _listOfWells.end();)
	{
		if( (*wp)->mIsDead )
		{
			wp = _listOfWells.erase( wp );
			continue;
		} 
		else 
		{
			(*wp)->mAgePer = 1.0f - ( (*wp)->mAge / (float) (*wp)->mLifespan );
			(*wp)->mAge++;
			(*wp)->mPosition -= ((*wp)->mPosition - (*wp)->mTouchPosition) * 0.25f;
			(*wp)->mForceScale = 1.0f + fabsf(sinf(elapsedFrames + ((*wp)->mTouchID*(*wp)->mTouchID)) * 0.44f);;
			
			float chance = Rand::randFloat();
			if((*wp)->mAgePer > 0.94) { // Extra strength in the beginning 
				(*wp)->mStrengthSquared = (*wp)->mStrength * (*wp)->mStrength * 2.5f;
			} else if(chance < 0.02f) { // Pulse
				(*wp)->mStrengthSquared = (*wp)->mStrength * (*wp)->mStrength * -8.0f;
			} else {					// Normal 
				(*wp)->mStrengthSquared = (*wp)->mStrength * (*wp)->mStrength * (*wp)->mForceScale;
				
				float dx = (*wp)->mTouchPosition.x - (*wp)->mPosition.x;
				float dy = (*wp)->mTouchPosition.y - (*wp)->mPosition.y;
				float minDistance = 60; 
				if( ( dx*dx+dy*dy ) > minDistance*minDistance ) {
					(*wp)->mStrengthSquared *= 2;
					//console() << ( dx*dx+dy*dy ) << std::endl;
				}
				
			}
			++wp;
		}
	}
}
void GravitySwarm::draw()
{
	if (_state == STATE_STARTSCREEN) {
		drawStartScreen();
	} else if ( _state == STATE_SIMULATION ) {
		drawSimulation();
	}
}

void GravitySwarm::drawStartScreen()
{
	if(!_startScreen) return;
	
	gl::clear( Color( 0, 0, 0 ), false );
	
	float blackLevel = math<float>::clamp(((float) getElapsedFrames()-30) / 60, 0, 1.0f);
	glColor4f( 1.0, 1.0, 1.0, blackLevel );
	
	glEnable( GL_TEXTURE_2D );
	gl::draw( _startScreen, Rectf(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT ) );
	glDisable( GL_TEXTURE_2D );
}

void GravitySwarm::drawSimulation()
{
	gl::clear( Color( 0, 0, 0 ), false );
	
	// draw all the particles as lines from mPosition to mLastPosition
	glEnableClientState(GL_VERTEX_ARRAY);
	for( list<Particle>::iterator partIt = _listOfParticles.begin(); partIt != _listOfParticles.end(); ++partIt )
	{
		glColor4f(0.4f + partIt->mVelocity.x / partIt->maxLinearVelocity * 0.5f,
				  0.4f + partIt->mVelocity.y / partIt->maxLinearVelocity * 0.5f,
				  0.4f + partIt->noise * 0.5f, 
				  partIt->mVelocity.lengthSquared() / (partIt->maxLinearVelocity*partIt->maxLinearVelocity) * 0.5f);
		
		Vec2f vertices[2];
		vertices[0] = partIt->mLastLastPosition;
		vertices[1] = partIt->mPosition;
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	
	return;
	if(!_textureWell) return;
	
	gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
	float originalSize = _textureWell.getWidth(); // my width and height happen to be the same
	
	_textureWell.enableAndBind();
	glEnable( GL_TEXTURE_2D );
	
	for(vector<WellPointer>::iterator wp = _listOfWells.begin(); wp != _listOfWells.end(); ++wp)
	{
		float scale		= (*wp)->mForceScale * 1.4f;
		float halfScale	= scale*0.5f;
		Vec2f offset	= (*wp)->mPosition - Vec2f(originalSize * halfScale, originalSize * halfScale);			
		Vec2f bounds	= offset + Vec2f(originalSize * scale, originalSize * scale);
		gl::draw( _textureWell, Rectf( offset.x, offset.y, bounds.x, bounds.y ) );
	}
	glDisable( GL_TEXTURE_2D );	
}

CINDER_APP_COCOA_TOUCH( GravitySwarm, RendererGl(0) )
