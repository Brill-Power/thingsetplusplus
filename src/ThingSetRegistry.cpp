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

// void ThingSetRegistry::iterateNodes(std::function<void(ThingSetNode *)> action)
// {
//     auto nodeMap = getInstance()._nodeMap;
//     // for (auto it(flatten(nodeMap.begin(), nodeMap.end())); it != nodeMap.end(); it++) {
//     //     action(it);
//     // }
// }

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::begin()
{
    auto commence = _nodeMap.begin();
    NodeMap::iterator begin = _nodeMap.begin();
    NodeMap::iterator end = _nodeMap.end();
    auto flat = flatten(begin, end);
    return flat;
}

FlatteningIterator<NodeMap::iterator> ThingSetRegistry::end()
{
    auto end = _nodeMap.end();
    return flatten(end);
}

} // namespace ThingSet