#include <Splitter.h>
#include "Box2DConstants.h"
#include <iostream>
Splitter::Splitter() {
}

Splitter::~Splitter() {
}

void Splitter::registerBodySplitCallback(std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)> callback) {
    m_functionCallbacks.push_back(callback);
}


void Splitter::registerBodySplitCallback(B2BodySplitCallback* callback) {
    m_callbacks.push_back(callback);
}

PointsDirection Splitter::isCCW(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3) {
    int direction = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
    if(direction > 0) return PointsDirection::CW;
    if(direction < 0) return PointsDirection::CCW;
    return PointsDirection::COLLINEAR;
}

float32 Splitter::ReportFixture(b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction) {
	std::cout << "in callback" << std::endl;
    processIntersection(fixture->GetBody(),point);
    return 1;
}

void Splitter::processIntersection(b2Body* body, const b2Vec2& point) {
    if(m_b2BodiesToIntersections.find(body) != m_b2BodiesToIntersections.end()) {
        splitBody(body, point);
    } else {
        addBody(body, point);
    }
}

bool Splitter::isValidSegment(const LineSegment& segment) {
    return !(segment.entryPoint.x ==  segment.exitPoint.x && segment.entryPoint.y ==  segment.exitPoint.y);
}

void Splitter::splitBody(b2Body* body, const b2Vec2 point) {
    b2Vec2 v = point;
    m_b2BodiesToIntersections[body].exitPoint = scale * v ;
    if(isValidSegment(m_b2BodiesToIntersections[body])) {

        splitBox2dBody(body,  m_b2BodiesToIntersections[body]);
    }
    m_b2BodiesToIntersections.erase(body);
}

void Splitter::addBody(b2Body* body, const b2Vec2 point) {
    b2Vec2 v = point;
    LineSegment intersectP;
    intersectP.entryPoint = scale * v ;
    m_b2BodiesToIntersections[body] =  intersectP;
}

std::vector<b2Vec2> Splitter::sortVecs(std::vector<b2Vec2>& vertices) {
    int count = vertices.size();
    int ccwIndex = 1;

    b2Vec2 referencePointA,referencePointB;
    std::vector<b2Vec2> sortedVertices;
    std::vector<b2Vec2> endSorted;

    std::sort(vertices.begin(), vertices.end(), Comparator());
    sortedVertices.push_back(vertices[0]);
    referencePointA = vertices[0];
    referencePointB = vertices[count-1];

    for (int i=1; i<count-1; i++) {
        PointsDirection determinant = isCCW(referencePointA, referencePointB, vertices[i]);
        if (determinant == CCW) {
            sortedVertices.push_back(vertices[i]);
            ccwIndex++;
        } else {
            endSorted.push_back(vertices[i]);
        }
    }

    std::reverse(endSorted.begin(),endSorted.end());
    sortedVertices.insert(sortedVertices.begin() + ccwIndex, vertices[count-1]);
    sortedVertices.insert(sortedVertices.end(), endSorted.begin(), endSorted.end());
    return sortedVertices;
}

void Splitter::setUpPointsWithLineSegment(const LineSegment& intersectionLine){
    b2Vec2 entry = intersectionLine.entryPoint;
    b2Vec2 exit = intersectionLine.exitPoint;
    m_cwPoints.clear();
    m_ccwPoints.clear();
    m_cwPoints.push_back(b2Vec2(entry.x/scale, entry.y/scale));
    m_cwPoints.push_back(b2Vec2(exit.x/scale, exit.y/scale));
    m_ccwPoints.push_back(b2Vec2(entry.x/scale, entry.y/scale));
    m_ccwPoints.push_back(b2Vec2(exit.x/scale, exit.y/scale));
}

bool Splitter::isSplitSuccessful(b2Body* body, const LineSegment& intersectionLine){
    if(!splitBodyByClockWiseOrCounterClockWiseDirection(body, intersectionLine, m_cwPoints, m_ccwPoints)) {
        return false ;
    }
    if(!areValidPoints(m_cwPoints,m_ccwPoints)) {
        return false;
    }
    return true;
}

bool Splitter::areSplitBodiesValid(){
    m_cwPoints = sortVecs(m_cwPoints);
    m_ccwPoints = sortVecs(m_ccwPoints);
    if(!areVecsValid(m_cwPoints)){
        return false;
    }
    if(!areVecsValid(m_ccwPoints)) {
        return false;
    }
    return true;
}

void Splitter::notifyListenersOfSplitBodiesFor(b2Body* body){
    std::vector<B2BoxBuilder> builders = getSplitBodies(body,m_cwPoints,m_ccwPoints);
    callbackHooks(builders, body);
}

void Splitter::splitBox2dBody(b2Body* body, const LineSegment& intersectionLine) {
    setUpPointsWithLineSegment(intersectionLine);
    if(isSplitSuccessful(body, intersectionLine)){
        if(areSplitBodiesValid()){
            notifyListenersOfSplitBodiesFor(body);
        }
    }
}

bool isDegenerate(const std::vector<b2Vec2>& vertices) {
    int32 tempCount = 0;
    int n = vertices.size();
    b2Vec2 ps[b2_maxPolygonVertices];
    for (int32 i = 0; i < n; ++i) {
        b2Vec2 v = vertices[i];
        bool unique = true;
        for (int32 j = 0; j < tempCount; ++j) {
            if (b2DistanceSquared(v, ps[j]) < 0.5f * b2_linearSlop) {
                unique = false;
                break;
            }
        }
        if (unique) {
            ps[tempCount++] = v;
        }
    }

    n = tempCount;
    if (n < 3) {
        return true;
    }
    return false;
}

bool Splitter::hasValidArea(std::vector<b2Vec2>& points) {
    int count = points.size();
    int prevIndex = count - 1;
    float area = 0.0f;
    for(int currentIndex = 0; currentIndex < count; currentIndex++) {
        b2Vec2 currentVec = points[currentIndex];
        b2Vec2 nextVec = points[prevIndex];
        float x = (nextVec.x + currentVec.x);
        float y = (nextVec.y - currentVec.y);
        area = area + (x * y);
        prevIndex = currentIndex;
    }
    if(area < 0.0f) {
        area = area * -1;
    }
    area /=2;

    return (area > 0.f);
}

bool Splitter::areVecsValid(std::vector<b2Vec2> points) {
    if(!areVecPointLengthsValid(points)) {
        return false;
    }

//    if(!hasValidArea(points)){
//        return false;
//     }

    if(isDegenerate(points)) {
        return false;
    }
    return true;
}

bool Splitter::areVecPointLengthsValid( std::vector<b2Vec2>& vertices) {
    int count = vertices.size();
    for (int i=0; i<count; ++i) {
        int i1 = i;
        int i2 = i + 1 < count ? i + 1 : 0;
        b2Vec2 edge = vertices[i2] - vertices[i1];
        if (edge.LengthSquared() <= b2_epsilon * b2_epsilon) {
            return false;
        }
    }
    return true;
}


bool Splitter::areValidPoints(std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints) {
    return  (isValidSize(cwPoints) &&  isValidSize(ccwPoints) );
}

bool Splitter::isValidSize(std::vector<b2Vec2>& cwPoints) {
    return (cwPoints.size() >= 3 &&  cwPoints.size() <= 8 );
}


void Splitter::callbackHooks(std::vector<B2BoxBuilder>& builders, b2Body* body) {
    for(auto fn : m_functionCallbacks)
        fn(builders, body);

    for(auto* callback : m_callbacks)
        callback->onb2BodySplit(builders, body);
}

std::vector<B2BoxBuilder> Splitter::getSplitBodies(b2Body* body, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints) {
    std::vector<B2BoxBuilder> builders;
    builders.push_back(getBox2dBuilder(cwPoints, body));
    builders.push_back(getBox2dBuilder(ccwPoints, body));
    return builders;
}

bool Splitter::isSamePoint(const b2Vec2& pointToCheck , const LineSegment& intersectionLine){
        b2Vec2 diffFromEntryPoint = scale * pointToCheck - intersectionLine.entryPoint;
        b2Vec2 diffFromExitPoint = scale * pointToCheck - intersectionLine.exitPoint;
        return !(diffFromEntryPoint.x == 0 && diffFromEntryPoint.y == 0) || !(diffFromExitPoint.x == 0 && diffFromExitPoint.y == 0);
}

bool Splitter::splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, const LineSegment& intersectionLine, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints) {
    b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
    for(int vertextIndex = 0; vertextIndex < shape->GetVertexCount(); vertextIndex++) {
        b2Vec2 pointToCheck = body->GetWorldPoint(shape->GetVertex(vertextIndex));
        if (isSamePoint(pointToCheck, intersectionLine)) {
            PointsDirection direction =  isCCW(intersectionLine.entryPoint, intersectionLine.exitPoint, scale * pointToCheck);
            if(direction == COLLINEAR) {
                return false;
            }
            if(direction == CW) {
                cwPoints.push_back(pointToCheck);
            } else {
                ccwPoints.push_back(pointToCheck);
            }
        }
    }
    return true;
}

void Splitter::clearIntersects() {
    m_b2BodiesToIntersections.clear();
}


B2BoxBuilder Splitter::getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body) {
    B2BoxBuilder builder(points, body);
    return builder;
}



