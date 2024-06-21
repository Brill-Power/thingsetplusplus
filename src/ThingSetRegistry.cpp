#include "ThingSetRegistry.hpp"

namespace ThingSet {

ThingSetRegistry::ThingSetRegistry()
{}

ThingSetRegistry &ThingSetRegistry::getInstance()
{
    static ThingSetRegistry instance;
    return instance;
}

void ThingSetRegistry::registerNode(ThingSetNode *node)
{
    unsigned id = node->getId();
    getInstance()._nodeMap[id % NODE_MAP_LOOKUP_BUCKETS].push_back(node);
}

bool ThingSetRegistry::findById(unsigned id, ThingSetNode **node)
{
    NodeList list = getInstance()._nodeMap[id % NODE_MAP_LOOKUP_BUCKETS];
    for (ThingSetNode *n : list) {
        if (n->getId() == id) {
            *node = n;
            return true;
        }
    }
    return false;
}

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::begin()
{
    auto begin = _nodeMap.begin();
    auto end = _nodeMap.end();
    return flatten(begin, end);
}

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::end()
{
    auto end = _nodeMap.end();
    return flatten(end);
}

} // namespace ThingSet