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

float32 RayCastCallback::ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
{
    b2Body* body = fixture->GetBody();

    if(m_b2BodiesToIntersections.find(body) != m_b2BodiesToIntersections.end()){
        m_b2BodiesToIntersections[body].exitPoint = sf::Vector2f(sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE));
    }else{
       IntersectPoints intersectP;
       intersectP.entryPoint = sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE);
       m_b2BodiesToIntersections[body] =  intersectP;
    }

    m_intersectionPoints.push_back(sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE));
    return 1;
}

std::unordered_map<b2Body*,  IntersectPoints, TemplateHasher<b2Body*>> RayCastCallback::getBodiesToIntersectPoints(){
    return m_b2BodiesToIntersections;
}


void RayCastCallback::clearPoints()
{
    m_intersectionPoints.clear();
}

void RayCastCallback::clearIntersects()
{
    m_b2BodiesToIntersections.clear();
}

std::vector<sf::Vector2f> RayCastCallback::getIntersections()
{
    return m_intersectionPoints;
}


