#ifndef TEXTUREMAPPER_H
#define TEXTUREMAPPER_H

#include "B2vecHasher.h"
#include <unordered_map>
#include <Box2D/Box2D.h>

class TextureMapper {
public:
    TextureMapper(float scale);
    virtual ~TextureMapper();
    std::vector<b2Vec2> mapSplitBody(const b2Body* childBody, const b2Body* parentBody, std::vector<b2Vec2> texCoords);

protected:
private:

    float fl(float num);
    b2Vec2 fl(b2Vec2 num);
    float getLength(b2Vec2 edge1, b2Vec2 edge2);
    float isCollinear(const b2Vec2& p1, const b2Vec2& p2, const b2Vec2& p3);
    b2Vec2 calculateNewTextCoord(float amountThrough, b2Vec2 texCoordsOfEdge1, b2Vec2 texCoordsOfEdge2, std::vector<b2Vec2> parentTexCoords);
    b2Vec2 getTexCoordFromParentBody(b2Vec2 vertex,  const b2Body* parentBody, std::vector<b2Vec2> parentTexCoords);
    b2Vec2 getTexCoordForChildVertex(b2Vec2 vertex, const b2Body* parentBody, std::vector<b2Vec2> parentTexCoords);
    std::vector<b2Vec2> getChildTexCoords(const b2Body* childBody, const b2Body* parentBody, std::vector<b2Vec2> texCoords);
    float getPercetangeThrough(b2Vec2 edge1, b2Vec2 edge2 ,b2Vec2 vertexToTexture);
    void cacheTextureCoords(const b2Body* parentBody, std::vector<b2Vec2> texCoords);

    float const m_scale;
    std::unordered_map<b2Vec2,  b2Vec2, B2vecHasher> m_posToTexCoords;


};

#endif // TEXTUREMAPPER_H
