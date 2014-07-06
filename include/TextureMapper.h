#ifndef TEXTUREMAPPER_H
#define TEXTUREMAPPER_H

#include "B2vecHasher.h"

class TextureMapper
{
    public:
        TextureMapper(float scale);
        virtual ~TextureMapper();
        std::vector<b2Vec2> mapSplitBody(b2Body* child, b2Body* parent, std::vector<b2Vec2> texcoords);

    protected:
    private:
        float const m_scale;
        std::unordered_map<b2Vec2,  b2Vec2, B2vecHasher> m_posToTexCoords;


};

#endif // TEXTUREMAPPER_H
