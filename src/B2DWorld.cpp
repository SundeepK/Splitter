#include "B2DWorld.h"
#include <GL/gl.h>
#include <GL/glu.h>
B2DWorld::B2DWorld(float gravity) : m_world(b2Vec2(0.f, gravity))
{
    m_world.SetAutoClearForces(true);
    m_world.SetAllowSleeping(true);
}

B2DWorld::~B2DWorld()
{
}

void B2DWorld::setDebugDraw(SFMLB2dDebugDraw& box2dDEbugDrawer){
    m_world.SetDebugDraw(&box2dDEbugDrawer);
}


b2Body* B2DWorld::createB2Body(B2Builder* builder){
    return builder->build(m_world);
}

//void B2DWorld::update(float dt, ActionController<std::string>& actionController,  sf::RenderWindow& win){
//    m_fixedTimestepAccumulator += dt;
//    const int steps = static_cast<int>(floor(m_fixedTimestepAccumulator / FIXED_TIMESTEP));
//
//    if (steps > 0)
//	{
//		m_fixedTimestepAccumulator -= steps * FIXED_TIMESTEP;
//	}
//
//	assertAccumilation();
//    m_fixedTimestepAccumulatorRatio = m_fixedTimestepAccumulator / FIXED_TIMESTEP;
//    const int clampedSteps = std::min(steps, MAX_STEPS);
//	for (int i = 0; i < clampedSteps; ++ i)
//	{
//		resetStates();
//		actionController.triggerCallbacks(m_fixedTimestepAccumulatorRatio);
//		step(FIXED_TIMESTEP);
//	}
//
//	interpolateStates();
//	m_world.DrawDebugData();
//
//}



void B2DWorld::step(float dt){
    m_world.Step(dt, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

void B2DWorld::interpolateStates(){

	const float oneMinusRatio = 1.0f - m_fixedTimestepAccumulatorRatio;

	for (b2Body * b = m_world.GetBodyList (); b != NULL; b = b->GetNext ())
	{
		if (b->GetType () == b2_staticBody)
		{
			continue;
		}

		PhysicsComponent *c   = (PhysicsComponent*) b->GetUserData();
		c->smoothedPosition =  m_fixedTimestepAccumulatorRatio * b->GetPosition () + oneMinusRatio * c->previousPosition;
		c->smoothedAngle = floor (m_fixedTimestepAccumulatorRatio * b->GetAngle () + oneMinusRatio * c->previousAngle);
	}

}

void B2DWorld::update(float accumulator){

		step(FIXED_TIMESTEP);

//	m_world.ClearForces();

    //smooth positions via interpolation
	m_world.DrawDebugData();


}

void B2DWorld::assertAccumilation(){
	assert (
		"Accumulator must have a value lesser than the fixed time step" &&
		m_fixedTimestepAccumulator < FIXED_TIMESTEP + FLT_EPSILON
	);
}



void B2DWorld::resetStates(){

for (b2Body * b = m_world.GetBodyList (); b != NULL; b = b->GetNext ())
	{
		if (b->GetType () == b2_staticBody)
		{
			continue;
		}
 		PhysicsComponent *c   = (PhysicsComponent*) b->GetUserData();

		c->smoothedPosition = c->previousPosition = b->GetPosition ();
		c->smoothedAngle = c->previousAngle = b->GetAngle ();
	}

}





