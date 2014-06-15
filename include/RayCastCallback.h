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

struct IntersectPoints
{
    public:
    sf::Vector2f entryPoint;
    sf::Vector2f exitPoint;

    sf::Vector2f getCenter(){
        return sf::Vector2f((entryPoint.x  + exitPoint.x)/2, (entryPoint.y  + exitPoint.y)/2);
    }

};

struct IntersectComp : std::binary_function<b2Vec2, b2Vec2, bool>
{
    //Polar coordinate system
    //sorting angles in counter-clockwise
    b2Vec2 M;
    IntersectComp(b2Vec2 v) : M(v) {}
    bool operator() ( b2Vec2 o1,  b2Vec2 o2)

    {

        float ang1     = atan( ((o1.y - M.y)/(o1.x - M.x) ) * M_PI / 180);
        float ang2     = atan( (o2.y - M.y)/(o2.x - M.x) * M_PI / 180);
        if(ang1 < ang2) return true;
        else if (ang1 > ang2) return false;

        return true;
    }
};

struct IntersectComp2 : std::binary_function<b2Vec2, b2Vec2, bool>
{

    bool operator() ( b2Vec2 va,  b2Vec2 vb)
    {
        if (va.x > vb.x)
        {
            return true;
        }
        else if (va.x < vb.x)
        {
            return false;
        }
        return false;
    }
};



class RayCastCallback : public b2RayCastCallback
{
    public:
        RayCastCallback();
        virtual ~RayCastCallback();
        float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction);
        void clearPoints();
        void clearIntersects();

        std::vector<sf::Vector2f> getIntersections();
        std::unordered_map<b2Body*,  IntersectPoints, TemplateHasher<b2Body*>> getBodiesToIntersectPoints();
    protected:
    private:
        std::vector<sf::Vector2f> m_intersectionPoints;
        std::unordered_map<b2Body*,  IntersectPoints, TemplateHasher<b2Body*>> m_b2BodiesToIntersections;

};

#endif // RAYCASTCALLBACK_H
