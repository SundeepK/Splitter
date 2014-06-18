#include "Splitter.h"
#include "Box2DConstants.h"
Splitter::Splitter()
{
}

Splitter::~Splitter()
{
}

void Splitter::registerBodySplitCallback(std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)> callback){
    m_functionCallbacks.push_back(callback);
}


//PointsDirection Splitter::isCCW(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3)
//{
//
//    int direction = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
//    if(direction > 0) return PointsDirection::CW;
//    if(direction < 0) return PointsDirection::CCW;
//    return PointsDirection::COLLINEAR;
//}

int Splitter::isCCW(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3)
{

   return p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
}

float32 Splitter::ReportFixture(b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
{
    processIntersection(fixture->GetBody(),point);
    return 1;
}

void Splitter::processIntersection(b2Body* body, const b2Vec2& point){
    if(m_b2BodiesToIntersections.find(body) != m_b2BodiesToIntersections.end()){
        splitBody(body, point);
    }else{
        addBody(body, point);
    }
}

void Splitter::splitBody(b2Body* body, const b2Vec2 point){
    m_b2BodiesToIntersections[body].exitPoint = b2Vec2(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE);
    splitBox2dBody(body,  m_b2BodiesToIntersections[body]);
    m_b2BodiesToIntersections.erase(body);
}

void Splitter::addBody(b2Body* body, const b2Vec2 point){
    LineSegment intersectP;
    intersectP.entryPoint = b2Vec2(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE);
    m_b2BodiesToIntersections[body] =  intersectP;
}

void Splitter::splitBox2dBody(b2Body* body, LineSegment intersectionLine)
{
    std::vector<b2Vec2> cwPoints;
    std::vector<b2Vec2> ccwPoints;

     b2Vec2 entry(intersectionLine.entryPoint.x/Box2DConstants::WORLD_SCALE, intersectionLine.entryPoint.y/Box2DConstants::WORLD_SCALE);
     b2Vec2 exit(intersectionLine.exitPoint.x/Box2DConstants::WORLD_SCALE, intersectionLine.exitPoint.y/Box2DConstants::WORLD_SCALE);


    cwPoints.push_back(entry);
    cwPoints.push_back(exit);

    ccwPoints.push_back(entry);
    ccwPoints.push_back(exit);

    splitBodyByClockWiseOrCounterClockWiseDirection(body, intersectionLine, cwPoints, ccwPoints);

    b2Vec2 center  = intersectionLine.getCenter();
    std::sort(cwPoints.begin(), cwPoints.end(), CCWComparator(center));
    std::sort(ccwPoints.begin(), ccwPoints.end(), CCWComparator(center));

    std::vector<B2BoxBuilder> builders = getSplitBodies(body,cwPoints,ccwPoints);

    for(auto fn : m_functionCallbacks)
        fn(builders, body);
}

std::vector<B2BoxBuilder> Splitter::getSplitBodies(b2Body* body, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints){
    std::vector<B2BoxBuilder> builders;
    builders.push_back(getBox2dBuilder(cwPoints, body));
    builders.push_back(getBox2dBuilder(ccwPoints, body));
    return builders;
}

void Splitter::splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, LineSegment intersectionLine, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints){
    b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
    for(int vertextIndex = 0; vertextIndex < shape->GetVertexCount(); vertextIndex++)
    {
        b2Vec2 vec = body->GetWorldPoint(shape->GetVertex(vertextIndex));
        int direction =  isCCW(intersectionLine.entryPoint, intersectionLine.exitPoint, b2Vec2(vec.x*Box2DConstants::WORLD_SCALE, vec.y*Box2DConstants::WORLD_SCALE));
        if(direction  > 0){
            cwPoints.push_back(vec);
        }else{
            ccwPoints.push_back(vec);
        }
    }
}

void Splitter::clearIntersects()
{
    m_b2BodiesToIntersections.clear();
}


B2BoxBuilder Splitter::getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body)
{
    B2BoxBuilder builder(points, body);
    return builder;
}



