#include "RayCastCallback.h"
#include "Box2DConstants.h"
RayCastCallback::RayCastCallback()
{
    //ctor
}

RayCastCallback::~RayCastCallback()
{
    //dtor
}

float32 RayCastCallback::ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction){
    m_intersectionPoints.push_back(sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE));
}

 void RayCastCallback::clearPoints(){
    m_intersectionPoints.clear();
 }

 std::vector<sf::Vector2f> RayCastCallback::getIntersections(){
   return m_intersectionPoints;
 }


