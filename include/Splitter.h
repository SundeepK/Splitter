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

struct CCWComparator : std::binary_function<Vec, Vec, bool>
{
    //Polar coordinate system
    //sorting angles in counter-clockwise
    Vec M;
    CCWComparator(Vec v) : M(v) {}
    bool operator() ( Vec o1,  Vec o2){

        float ang1     = atan( ((o1.y - M.y)/(o1.x - M.x) ) * M_PI / 180);
        float ang2     = atan( (o2.y - M.y)/(o2.x - M.x) * M_PI / 180);
        if(ang1 < ang2) return true;
        else if (ang1 > ang2) return false;

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
        void registerBodySplitCallback(B2BodySplitCallback& callback);
        void registerBodySplitCallback(std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)> callback );
        void clearIntersects();

    protected:
    private:

        struct LineSegment{
            public:
                Vec entryPoint;
                Vec exitPoint;
                Vec getCenter(){
                    return Vec((entryPoint.x  + exitPoint.x)/2, (entryPoint.y  + exitPoint.y)/2);
                }
        };

        PointsDirection isCCW(Vec p1, Vec p2, Vec p3);
        void splitBox2dBody(b2Body* body, LineSegment intersectionLine);
        void processIntersection(b2Body* body, const b2Vec2& point);
        void splitBody(b2Body* body, const b2Vec2 point);
        void addBody(b2Body* body, const b2Vec2 point);
        std::vector<B2BoxBuilder> getSplitBodies(b2Body* body, std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints);
        void splitBodyByClockWiseOrCounterClockWiseDirection(b2Body* body, LineSegment intersectionLine, std::vector<Vec>& cwPoints,  std::vector<Vec>& ccwPoints);
        B2BoxBuilder getBox2dBuilder(std::vector<Vec> points, b2Body* body);

        std::unordered_map<b2Body*,  LineSegment, TemplateHasher<b2Body*>> m_b2BodiesToIntersections;
        std::vector<B2BodySplitCallback> m_callbacks;
        std::vector<std::function<void(std::vector<B2BoxBuilder> splitBodies, b2Body* body)>> m_functionCallbacks;
};

#endif // RAYCASTCALLBACK_H
