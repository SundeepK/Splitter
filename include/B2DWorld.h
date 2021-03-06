#ifndef B2DWORLD_H
#define B2DWORLD_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include <math.h>
#include "SFMLB2dDebugDraw.h"
#include "ActionController.h"
#include "Action.h"
#include <functional>
#include <iostream>
#include "Splitter.h"
#include "B2Builder.h"


struct segment {
    b2Vec2 p1;
    b2Vec2 p2;
};

struct Texcoords {
public:
    std::vector<b2Vec2> textCoords;
};

struct PhysicsComponent  {
	float previousAngle;
	float smoothedAngle;
	b2Vec2 previousPosition;
	b2Vec2 smoothedPosition;
};

class B2DWorld
{
public:
    B2DWorld(float gravity);
    virtual ~B2DWorld();
    void update(float dt);
    b2Body* createB2Body(B2Builder* builder);
    void setDebugDraw(SFMLB2dDebugDraw& box2dDEbugDrawer);
    void rayCast(const sf::Vector2f& point1, const sf::Vector2f& point2)  ;
    void deleteBody(b2Body* body);
    void registerBodySplitCallback(std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)> callback );
    void registerBodySplitCallback(B2BodySplitCallback* callback );
    void clearIntersects();
    b2Body* GetBodyList();


protected:
private:
    void assertAccumilation();
    void step(float dt);
    void interpolateStates();
    void resetStates();
    void drawSquare(b2Vec2* points,b2Vec2 center,float angle);

    b2World m_world;
    float m_fixedTimestepAccumulator = 0.0f;
    float m_fixedTimestepAccumulatorRatio = 0.0f;
    Splitter m_raycastCallback;

    const float FIXED_TIMESTEP = 1.0f / 60.0f;
    const int MAX_STEPS = 5;
    const int VELOCITY_ITERATIONS = 8;
    const int POSITION_ITERATIONS = 3;

};

#endif // B2DWORLD_H
