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

PointsDirection Splitter::isCCW(Vec p1, Vec p2, Vec p3)
{

    int direction = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
    if(direction > 0) return PointsDirection::CW;
    if(direction < 0) return PointsDirection::CCW;
    return PointsDirection::COLLINEAR;
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
    Vec v = point;
    m_b2BodiesToIntersections[body].exitPoint = v.mToPix();
    splitBox2dBody(body,  m_b2BodiesToIntersections[body]);
    m_b2BodiesToIntersections.erase(body);
}

void Splitter::addBody(b2Body* body, const b2Vec2 point){
    Vec v = point;
    LineSegment intersectP;
    intersectP.entryPoint = v.mToPix();
    m_b2BodiesToIntersections[body] =  intersectP;
}

void Splitter::splitBox2dBody(b2Body* body, LineSegment intersectionLine)
{
    std::vector<Vec> cwPoints;
    std::vector<Vec> ccwPoints;

    Vec entry = intersectionLine.entryPoint;
    Vec exit = intersectionLine.exitPoint;

    cwPoints.push_back(entry.pixToM());
    cwPoints.push_back(exit.pixToM());

    ccwPoints.push_back(entry.pixToM());
    ccwPoints.push_back(exit.pixToM());

    splitBodyByClockWiseOrCounterClockWiseDirection(body, intersectionLine, cwPoints, ccwPoints);

    Vec center  = intersectionLine.getCenter();
    std::sort(cwPoints.begin(), cwPoints.end(), CCWComparator(center));
    std::sort(ccwPoints.begin(), ccwPoints.end(), CCWComparator(center));

    std::vector<B2BoxBuilder> builders = getSplitBodies(body,cwPoints,ccwPoints);
    callbackHooks(builders, body);
}

void Splitter::callbackHooks(std::vector<B2BoxBuilder>& builders, b2Body* body){
    for(auto fn : m_functionCallbacks)
        fn(builders, body);

    for(auto* callback : m_callbacks)
        callback->onb2BodySplit(builders, body);
}

std::vector<B2BoxBuilder> Splitter::getSplitBodies(b2Body* body, std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints){
    std::vector<B2BoxBuilder> builders;
    builders.push_back(getBox2dBuilder(cwPoints, body));
    builders.push_back(getBox2dBuilder(ccwPoints, body));
    return builders;
}

void Splitter::splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, LineSegment intersectionLine, std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints){
    b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
    for(int vertextIndex = 0; vertextIndex < shape->GetVertexCount(); vertextIndex++)
    {
        Vec vec = body->GetWorldPoint(shape->GetVertex(vertextIndex));
        PointsDirection direction =  isCCW(intersectionLine.entryPoint, intersectionLine.exitPoint, vec.mToPix());
        if(direction == CW){
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


B2BoxBuilder Splitter::getBox2dBuilder(std::vector<Vec> points, b2Body* body)
{
    std::vector<b2Vec2> vecs;
    for(Vec v : points){
        vecs.push_back(v.toB2v());
    }
    B2BoxBuilder builder(vecs, body);
    return builder;
}



