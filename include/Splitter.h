#ifndef RAYCASTCALLBACK_H
#define RAYCASTCALLBACK_H

#include <vector>
#include <Box2D/Common/b2Settings.h>
#include <Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <unordered_map>
#include "TemplateHasher.h"
#include "B2BodySplitCallback.h"
#include "B2BoxBuilder.h"
#include "Vec.h"
#include <algorithm>

struct CCWComparator : std::binary_function<b2Vec2, b2Vec2, bool>
{
    //Polar coordinate system
    //sorting angles in counter-clockwise
    b2Vec2 M;
    CCWComparator(b2Vec2 v) : M(v) {}
    bool operator() ( b2Vec2 o1,  b2Vec2 o2){

        float ang1     = atan( ((o1.y - M.y)/(o1.x - M.x) ) * M_PI / 180);
        float ang2     = atan( (o2.y - M.y)/(o2.x - M.x) * M_PI / 180);
        if(ang1 < ang2) return true;
        else if (ang1 > ang2) return false;

        return true;
    }
};

struct Comparator2 : std::binary_function<b2Vec2, b2Vec2, int>
{
    int operator() ( b2Vec2 va,  b2Vec2 vb){
        if (va.x > vb.x)
        {
            return 1;
        }
        else if (va.x < vb.x)
        {
            return -1;
        }

        return 0;
    }
};


struct Comparator : std::binary_function<b2Vec2, b2Vec2, bool>
{
    bool operator() ( b2Vec2 va,  b2Vec2 vb){
        if (va.x > vb.x)
        {
            return true;
        }
        else if (va.x < vb.x)
        {
            return false;
        }

        return true;
    }
};

enum PointsDirection{
   CCW,
   CW,
   COLLINEAR
};

class Splitter : public b2RayCastCallback
{
public:

        Splitter();
        virtual ~Splitter();
        float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction);
        void registerBodySplitCallback(B2BodySplitCallback* callback);
        void registerBodySplitCallback(std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)> callback );
        void clearIntersects();

    protected:
    private:

        struct LineSegment{
            public:
                b2Vec2 entryPoint;
                b2Vec2 exitPoint;
                b2Vec2 getCenter(){
                    return b2Vec2((entryPoint.x  + exitPoint.x)/2, (entryPoint.y  + exitPoint.y)/2);
                }
        };

        PointsDirection isCCW(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3);
        void splitBox2dBody(b2Body* body, const LineSegment& intersectionLine);
        void processIntersection(b2Body* body, const b2Vec2& point);
        void splitBody(b2Body* body, const b2Vec2 point);
        void addBody(b2Body* body, const b2Vec2 point);
        std::vector<B2BoxBuilder> getSplitBodies(b2Body* body, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints);
        bool splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, const LineSegment& intersectionLine, std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints);
        B2BoxBuilder getBox2dBuilder(std::vector<b2Vec2> points, b2Body* body);
        void callbackHooks(std::vector<B2BoxBuilder>& builders, b2Body* body);
        bool isValidSegment(const LineSegment& segment);
        bool areValidPoints(std::vector<b2Vec2>& cwPoints,  std::vector<b2Vec2>& ccwPoints);
        bool isValidSize(std::vector<b2Vec2>& cwPoints);
        bool ComputeCentroid(std::vector<b2Vec2>& vs);
        bool areVecsValid( std::vector<b2Vec2> points);
        bool areVecPointLengthsValid( std::vector<b2Vec2>& points);
        std::vector<b2Vec2> sortVecs(std::vector<b2Vec2>& vertices);
        bool hasValidArea(std::vector<b2Vec2>& points);
        void setUpPointsWithLineSegment(const LineSegment& intersectionLine);
        bool isSplitSuccessful(b2Body* body, const LineSegment& intersectionLine);
        bool areSplitBodiesValid();
        void notifyListenersOfSplitBodiesFor(b2Body* body);
        bool isSamePoint(const b2Vec2& pointToCheck , const LineSegment& intersectionLine);


        std::unordered_map<b2Body*,  LineSegment, TemplateHasher<b2Body*>> m_b2BodiesToIntersections;
        std::vector<B2BodySplitCallback*> m_callbacks;
        std::vector<std::function<void(std::vector<B2BoxBuilder>& splitBodies, b2Body* body)>> m_functionCallbacks;
        std::vector<b2Vec2> m_cwPoints;
        std::vector<b2Vec2> m_ccwPoints;

        float scale =  Box2DConstants::WORLD_SCALE;

};

#endif // RAYCASTCALLBACK_H
