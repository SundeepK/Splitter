#ifndef RAYCASTCALLBACK_H
#define RAYCASTCALLBACK_H

#include <vector>
#include <Box2D/Common/b2Settings.h>
#include <Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class RayCastCallback : public b2RayCastCallback
{
    public:
        RayCastCallback();
        virtual ~RayCastCallback();
        float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction);
        void clearPoints();
        std::vector<sf::Vector2f> getIntersections();
    protected:
    private:
        std::vector<sf::Vector2f> m_intersectionPoints;
};

#endif // RAYCASTCALLBACK_H
