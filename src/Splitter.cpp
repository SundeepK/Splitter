#include "Splitter.h"
#include "Box2DConstants.h"
#include <iostream>
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

bool Splitter::isValidSegment(const LineSegment& segment){
   return !(segment.entryPoint.x ==  segment.exitPoint.x && segment.entryPoint.y ==  segment.exitPoint.y);
}

void Splitter::splitBody(b2Body* body, const b2Vec2 point){
    Vec v = point;
    m_b2BodiesToIntersections[body].exitPoint = v.mToPix();
    if(isValidSegment(m_b2BodiesToIntersections[body])){
      splitBox2dBody(body,  m_b2BodiesToIntersections[body]);
    }
    m_b2BodiesToIntersections.erase(body);
}

void Splitter::addBody(b2Body* body, const b2Vec2 point){
    Vec v = point;
    LineSegment intersectP;
    intersectP.entryPoint = v.mToPix();
    m_b2BodiesToIntersections[body] =  intersectP;
}

std::vector<Vec> Splitter::sortVecs(std::vector<Vec> vertices)
{
    int count = vertices.size();
    int ccwIndex = 1;

    Vec referencePointA,referencePointB;
    std::vector<Vec> sortedVertices;
    std::vector<Vec> endSorted;

    std::sort(vertices.begin(), vertices.end(), Comparator());
    sortedVertices.push_back(vertices[0]);
    referencePointA = vertices[0];
    referencePointB = vertices[count-1];

    for (int i=1; i<count-1; i++){
        PointsDirection determinant = isCCW(referencePointA, referencePointB, vertices[i]);
        if (determinant == CCW){
            sortedVertices.push_back(vertices[i]);
            ccwIndex++;
        }else{
            endSorted.push_back(vertices[i]);
        }
    }

    std::reverse(endSorted.begin(),endSorted.end());
    sortedVertices.insert(sortedVertices.begin() + ccwIndex, vertices[count-1]);
    sortedVertices.insert(sortedVertices.end(), endSorted.begin(), endSorted.end());
    return sortedVertices;
}

Vec getCentroid(std::vector<Vec>& vs)
{
    int count = vs.size();
	b2Vec2 c; c.Set(0.0f, 0.0f);
	float area = 0.0f;

	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 pRef(0.0f, 0.0f);

	const float inv3 = 1.0f / 3.0f;

	for (int i = 0; i < count; ++i)
	{
		// Triangle vertices.
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = vs[i].toB2v();
		b2Vec2 p3 = i + 1 < count ? vs[i+1].toB2v() : vs[0].toB2v();

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float D = b2Cross(e1, e2);

		float triangleArea = 0.5f * D;
		area += triangleArea;
		c += triangleArea * inv3 * (p1 + p2 + p3);
	}

	// Centroid
		c *= 1.0f / area;
            std::cout << "center x: " << c.x << " center y: " << c.y << std::endl;
            std::cout  << std::endl;

	return Vec(c);

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

    if(!splitBodyByClockWiseOrCounterClockWiseDirection(body, intersectionLine, cwPoints, ccwPoints)){
        return ;
    }

     if(!areValidPoints(cwPoints,ccwPoints)){
        return;
     }

    std::vector<Vec> cwPoints1 = sortVecs(cwPoints);
    std::vector<Vec> ccwPoints2 = sortVecs(ccwPoints);

    for(auto v : cwPoints1){
        std::cout << "x: " << v.x << " y: " << v.y << std::endl;
    }
            std::cout << std::endl;


    if(!areVecsValid(cwPoints1)) return;
    if(!areVecsValid(ccwPoints2)) return;

    std::vector<B2BoxBuilder> builders = getSplitBodies(body,cwPoints1,ccwPoints2);
    callbackHooks(builders, body);
}

bool isDegenerate(std::vector<Vec> vertices){
// Perform welding and copy vertices into local buffer.
	int32 tempCount = 0;
	int n = vertices.size();
	b2Vec2 ps[b2_maxPolygonVertices];
	for (int32 i = 0; i < n; ++i)
	{
		b2Vec2 v = vertices[i].toB2v();

		bool unique = true;
		for (int32 j = 0; j < tempCount; ++j)
		{
			if (b2DistanceSquared(v, ps[j]) < 0.5f * b2_linearSlop)
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			ps[tempCount++] = v;
		}
	}

	n = tempCount;
	if (n < 3)
	{
		return true;
	}
	return false;
}

bool Splitter::hasValidArea(std::vector<Vec>& points){
    int count = points.size();
    int prevIndex = count - 1;
    float area = 0.0f;
    for(int currentIndex = 0; currentIndex < count; currentIndex++){
        Vec currentVec = points[currentIndex];
        Vec nextVec = points[prevIndex];
        float x = (nextVec.x + currentVec.x);
        float y = (nextVec.y - currentVec.y);
        area = area + (x * y);
        prevIndex = currentIndex;
    }
    if(area < 0.0f){
        area = area * -1;
    }
    area /=2;
    return (area > b2_epsilon);
}

bool Splitter::areVecsValid(std::vector<Vec>& points){
    if(!areVecPointLengthsValid(points)){
        return false;
    }

     if(!hasValidArea(points)){
        return false;
     }

//    if(!ComputeCentroid(points)){
//        return false;
//    }

     if(isDegenerate(points)){
        return false;
     }

    return true;

}

bool Splitter::ComputeCentroid(std::vector<Vec>& vs)
{
    int count = vs.size();
	b2Vec2 c; c.Set(0.0f, 0.0f);
	float area = 0.0f;

	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 pRef(0.0f, 0.0f);

	const float inv3 = 1.0f / 3.0f;

	for (int i = 0; i < count; ++i)
	{
		// Triangle vertices.
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = vs[i].toB2v();
		b2Vec2 p3 = i + 1 < count ? vs[i+1].toB2v() : vs[0].toB2v();

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float D = b2Cross(e1, e2);

		float triangleArea = 0.5f * D;
		area += triangleArea;
	}

	// Centroid
	return (area > b2_epsilon);

}



bool Splitter::areVecPointLengthsValid( std::vector<Vec>& vertices){
    int count = vertices.size();
    for (int i=0; i<count; ++i)
    {
        int i1 = i;
        int i2 = i + 1 < count ? i + 1 : 0;
        b2Vec2 edge = vertices[i2].toB2v() - vertices[i1].toB2v();
        if (edge.LengthSquared() <= b2_epsilon * b2_epsilon)
        {
            return false;
        }
    }
    return true;
}


bool Splitter::areValidPoints(std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints){
    return  (isValidSize(cwPoints) &&  isValidSize(ccwPoints) );
}

bool Splitter::isValidSize(std::vector<Vec>& cwPoints){
    return (cwPoints.size() >= 3 &&  cwPoints.size() <= 8 );
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

bool Splitter::splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, LineSegment intersectionLine, std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints){
    b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
    for(int vertextIndex = 0; vertextIndex < shape->GetVertexCount(); vertextIndex++)
    {
        Vec vec = body->GetWorldPoint(shape->GetVertex(vertextIndex));
        PointsDirection direction =  isCCW(intersectionLine.entryPoint, intersectionLine.exitPoint, vec.mToPix());

        if(direction == COLLINEAR){
            return false;
        }

        if(direction == CW){
            cwPoints.push_back(vec);
        }else{
            ccwPoints.push_back(vec);
        }
    }

    return true;
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



