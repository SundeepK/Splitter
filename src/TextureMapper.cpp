#include "TextureMapper.h"

TextureMapper::TextureMapper(float scale) : m_scale(scale) {
}

TextureMapper::~TextureMapper() {
}

float TextureMapper::fl(float num) {
    return floor( num * 1000.00 + 0.5 ) / 1000.00;
}

b2Vec2 TextureMapper::fl(b2Vec2 num) {
    return b2Vec2(fl( num.x), fl( num.y));
}

float TextureMapper::isCollinear(const b2Vec2& p1, const b2Vec2& p2, const b2Vec2& p3) {
    return round(p1.x*p2.y+p2.x*p3.y+p3.x*p1.y-p1.y*p2.x-p2.y*p3.x-p3.y*p1.x) == 0;
}

void TextureMapper::cacheTextureCoords(const b2Body* parentBody, std::vector<b2Vec2> texCoords) {
    m_posToTexCoords.clear();
    int parentVertexCount = parentShape->GetVertexCount();
    for(int i=0; i< parentVertexCount; i++) {
        b2Vec2 vertex = parentBody->GetWorldPoint((parentShape)->GetVertex(i));
        std::pair<b2Vec2,b2Vec2> posToTexPair (fl(scale*vertex), texCoords[i]);
        m_posToTexCoords.insert(posToTexPair);
    }
}

b2Vec2 TextureMapper::calculateNewTextCoord(b2Vec2 edge1, b2Vec2 edge2 ,b2Vec2 vertexToTexture) {
    //(1 - amountThrough) * edge1_textcoords + amountThrough * edge2_texcoords
    float length = getLength(edge1, edge2);
    float newVertLength = getLength(vertexToTexture, edge2);
    float amountThrough = 1.0f - (newVertLength / length);

    b2Vec2 tex1 = textCoords[i];
    b2Vec2 tex2 = textCoords[i2];

    b2Vec2 newTex1 (  tex1.x  * (1.0f -amountThrough),  tex1.y  * (1.0f -amountThrough));
    b2Vec2 newTex2 (  tex2.x  * amountThrough,  tex2.y * amountThrough);
    b2Vec2 newTex = newTex1 + newTex2;
    return newTex;
}

b2vec2 TextureMapper::getTexCoordFromParentBody(b2Vec2 vertex,  const b2Body* parentBody) {
    b2PolygonShape* parentShape =((b2PolygonShape*)parentBody->GetFixtureList()->GetShape());
    int parentVertexCount = parentShape->GetVertexCount();
    for(int i=0; i< parentVertexCount; i++) {
        int i2 = i + 1 < parentVertexCount ? i + 1 : 0;
        b2Vec2 edge1 = m_scale*parentBody->GetWorldPoint((parentShape)->GetVertex(i));
        b2Vec2 edge2 = m_scale*parentBody->GetWorldPoint((parentShape)->GetVertex(i2));

        if(isCollinear( edge1, vertex, edge2)) {
           return calculateNewTextCoord(edge1, edge2, vertex);
        }
    }
    //if we get here then the vertex to texture map is not even part of the parent so incorrect to begin with
    //TODO handle this edge case in mapSplitBody() function
}

b2Vec2 TextureMapper::getTexCoordForChildVertex(b2Vec2 vertex, const b2Body* parentBody) {
    if(m_posToTexCoords.find(fl(vertex)) != m_posToTexCoords.end()) {
        return m_posToTexCoords[fl(vertex)];
    } else {
        return getTexCoordFromParentBody(vertex, parentBody);
    }
}

std::vector getChildTexCoords(const b2Body* childBody, const b2Body* parentBody, std::vector<b2Vec2> texCoords) {
    b2PolygonShape* childShape =((b2PolygonShape*)childBody->GetFixtureList()->GetShape());
    int childVertexCount = childShape->GetVertexCount();
    std::vector<b2Vec2> childTexcoords;
    for(int i=0; i< childVertexCount; i++) {
        b2Vec2 childVertex =m_scale*(childShape)->GetVertex(i);
        childTexcoords.push_back(getTexCoordForChildVertex(childVertex, parentBody));
    }
    return childTexcoords;
}

//does nothing if the number of verticies do not match the number of texcoords
std::vector<b2Vec2> TextureMapper::mapSplitBody(const b2Body* childBody, const b2Body* parentBody, std::vector<b2Vec2> texCoords) {
    b2PolygonShape* parentShape =((b2PolygonShape*)parentBody->GetFixtureList()->GetShape());
    int parentVertexCount = parentShape->GetVertexCount();
    std::vector<b2Vec2> childTexcoords;

    if(parentShape->GetVertexCount() == texcoords.size()) {
        cacheTextureCoords(parentBody, texCoords);
        childTexcoords = getChildTexCoords(childBody, parentBody, texCoords);
    }
    return  childTexcoords;
}

