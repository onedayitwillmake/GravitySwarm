/*
 *  Particle.cpp
 *  GravitySwarm
 *
 *  Created by Mario.Gonzalez on 6/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/Rand.h"
#include "cinder/Vector.h"
using namespace ci;

class Particle
{
public:
	// Properties
	Vec2f mPosition, mVelocity, mLastPosition, mLastLastPosition;
	
	int		mLifespan;
	int		mAge;
	float	mAgePer;
	bool	mIsDead;
	
	float	maxLinearVelocity;
	float	damping;
	float	noise;
	
	float gravityForceScale;
	
	Particle( Vec2f aPosition, float aForceScale )
	{
		mPosition = aPosition;
		mLastPosition = aPosition;
		reset();
	}
	
	void reset()
	{
		mVelocity = Vec2f::zero();
		noise = 0.0f;
		
		gravityForceScale = 1.0f;//Rand::randFloat(1.0f, 1.5f);
		damping = Rand::randFloat(0.980f, 0.995f);
		maxLinearVelocity = 15.0f;//Rand::randFloat(10.0f, 12.0f);
		
		mLifespan = Rand::randInt(40, 250);
		
		mAge = 1;
		mAgePer = 0;
		mIsDead = false;
	}
	
	void reset(Vec2f aPosition)
	{
		mLastPosition = aPosition;
		mPosition = aPosition;
		reset();
	}
	
	void tick()
	{
	//	mLastLastPosition = mLastPosition;
		mLastPosition = mPosition;
		mAgePer = 1.0f - ( mAge / (float) mLifespan );
		
//		// cap the velocity 
//		float maxVel = maxLinearVelocity;
		float velLengthSquared = mVelocity.lengthSquared() + 0.01f;
		float extra = 5.0f;
		if( velLengthSquared > maxLinearVelocity*maxLinearVelocity + (extra*mAgePer))
		{
			mVelocity.normalize();
			mVelocity *= maxLinearVelocity + (extra*mAgePer);
		}
		
//		// Move the particles according to their velocities, then dampen
		mPosition += mVelocity;
		mVelocity *= damping;
	}
};

