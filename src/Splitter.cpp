#include "Splitter.h"
#include "Box2DConstants.h"
Splitter::Splitter()
{
    //ctor
}

Splitter::~Splitter()
{
    //dtor
}

PointsDirection Splitter::isCCW(b2Vec2 p1, b2Vec2 p2, b2Vec2 p3)
{
    int direction = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
    if(direction > 0) return PointsDirection.CW;
    if(direction < 0) return PointsDirection.CCW;
    if(direction == 0) return PointsDirection.COLLINEAR;
}

float32 Splitter::ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
{
    b2Body* body = fixture->GetBody();

    if(m_b2BodiesToIntersections.find(body) != m_b2BodiesToIntersections.end())
    {
        m_b2BodiesToIntersections[body].exitPoint = sf::Vector2f(sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE));

    }
    else
    {
        IntersectionLineSegment intersectP;
        intersectP.entryPoint = sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE);
        m_b2BodiesToIntersections[body] =  intersectP;
    }

    m_intersectionPoints.push_back(sf::Vector2f(point.x*Box2DConstants::WORLD_SCALE, point.y*Box2DConstants::WORLD_SCALE));

    return 1;
}

void Splitter::splitBox2dBody(b2Body body*, LineSegment intersectionLine)
{
    b2PolygonShape* shape =((b2PolygonShape*)body->GetFixtureList()->GetShape());
    std::vector<b2Vec2> shapePoint1;
    std::vector<b2Vec2> shapePoint2;

    b2Vec2 entry(intersectionLine.entryPoint.x/Box2DConstants::WORLD_SCALE, intersectionLine.entryPoint.y/Box2DConstants::WORLD_SCALE);
    b2Vec2 exit(intersectionLine.exitPoint.x/Box2DConstants::WORLD_SCALE, intersectionLine.exitPoint.y/Box2DConstants::WORLD_SCALE);

    shapePoint1.push_back(entry);
    shapePoint1.push_back(exit);
    shapePoint2.push_back(entry);
    shapePoint2.push_back(exit);

    for(int vertextIndex = 0; vertextIndex < shape->GetVertexCount(); vertextIndex++)
    {
        b2Vec2 vec = body->GetWorldPoint(shape->GetVertex(vertextIndex));

        PointsDirection direction =  isCCW(intersectionLine.entryPoint, intersectionLine.exitPoint, sf::Vector2f(vec.x *Box2DConstants::WORLD_SCALE, vec.y *Box2DConstants::WORLD_SCALE));

        if(determinant > 0)
        {
            shapePoint1.push_back(vec);
        }
        else
        {
            shapePoint2.push_back(vec);
        }
    }

    sf::Vector2f c  = intersectionLine.getCenter();

    b2Vec2 center(c.x/Box2DConstants::WORLD_SCALE,c.y/Box2DConstants::WORLD_SCALE);
    std::sort(shapePoint1.begin(), shapePoint1.end(), CCWComparator(center ));
    std::sort(shapePoint2.begin(), shapePoint2.end(), CCWComparator(center ));

    B2BoxBuilder builder = getBox2dBuilder(shapePoint1, body);
    b2Body* b = box2DWorld.createB2Body(&builder);

    B2BoxBuilder builder2 = getBox2dBuilder(shapePoint2, body);
    b2Body* b2 = box2DWorld.createB2Body(&builder2);

    drawCircle(App,intersectionLine.getCenter(), sf::Color::White);

}


std::unordered_map<b2Body*,  IntersectionLineSegment, TemplateHasher<b2Body*>> Splitter::getBodiesToIntersectPoints()
{
    return m_b2BodiesToIntersections;
}


void Splitter::clearPoints()
{
    m_intersectionPoints.clear();
}

void Splitter::clearIntersects()
{
    m_b2BodiesToIntersections.clear();
}

std::vector<sf::Vector2f> Splitter::getIntersections()
{
    return m_intersectionPoints;
}


