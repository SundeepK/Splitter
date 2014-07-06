#ifndef B2VECHASHER_H
#define B2VECHASHER_H

#include <Box2D/Box2D.h>

struct B2vecHasher : public std::unary_function<b2Vec2, std::size_t>
{
public:
    std::size_t operator()(const b2Vec2 & t) const
    {
        size_t seed = 0;
        hash_combine(seed, t);
        return seed;
    }

    void hash_combine(std::size_t & seed, const b2Vec2 & v) const
    {
        std::hash<int> hasher;
        seed ^= hasher(v.x) + hasher(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};


#endif // B2VECHASHER_H
