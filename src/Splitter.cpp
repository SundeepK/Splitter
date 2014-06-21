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

std::vector<Vec> Splitter::arrangeVertices(std::vector<Vec> vertices)
{
    int count = vertices.size();
    int iCounterClockWise = 1;
    int iClockWise = count ;
    int i;

    Vec referencePointA,referencePointB;
    std::vector<Vec> sortedVertices(count);
    std::vector<Vec> endSorted;

    //sort all vertices in ascending order according to their x-coordinate so you can get two points of a line
    std::sort(vertices.begin(), vertices.end(), Comparator());

    sortedVertices.push_back(vertices[0]);
    referencePointA = vertices[0];          //leftmost point
    referencePointB = vertices[count-1];    //rightmost point

    //you arrange the points by filling our vertices in both clockwise and counter-clockwise directions using the determinant function
    for (i=1;i<count-1;i++)
    {
        PointsDirection determinant = isCCW(referencePointA, referencePointB, vertices[i]);
        if (determinant == CCW)
        {
            sortedVertices.push_back(vertices[i]);iCounterClockWise++;
        }
        else
        {
            endSorted.push_back(vertices[i]);
        }//endif
    }//endif
     std::reverse(endSorted.begin(),endSorted.end());
   sortedVertices[iCounterClockWise] = vertices[count-1];
     sortedVertices.insert(sortedVertices.end(), endSorted.begin(), endSorted.end());


    return sortedVertices;
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

//    cwPoints.push_back(entry);
//    cwPoints.push_back(exit);
//
//    ccwPoints.push_back(entry);
//    ccwPoints.push_back(exit);

    if(!splitBodyByClockWiseOrCounterClockWiseDirection(body, intersectionLine, cwPoints, ccwPoints)){
        return ;
    }

     if(!areValidPoints(cwPoints,ccwPoints)){
        return;
     }

    Vec center  = intersectionLine.getCenter();
    std::sort(cwPoints.begin(), cwPoints.end(), CCWComparator(center));
    std::sort(ccwPoints.begin(), ccwPoints.end(), CCWComparator(center));

//    std::vector<Vec> cwPoints1 = arrangeVertices(cwPoints);
//    std::vector<Vec> ccwPoints2 = arrangeVertices(ccwPoints);

    if(!areVecsValid(cwPoints)) return;
    if(!areVecsValid(ccwPoints)) return;

    std::vector<B2BoxBuilder> builders = getSplitBodies(body,cwPoints,ccwPoints);
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

bool Splitter::areVecsValid(std::vector<Vec>& points){
    if(!areVecPointLengthsValid(points)){
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



