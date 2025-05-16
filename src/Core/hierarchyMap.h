#ifndef HIERARCHY_MAP_H
#define HIERARCHY_MAP_H

#include <map>
#include <string>
#include <memory>
#include <sstream>
// #include "imgui.h"

template<typename K, typename V>
class HierarchyMap {
public:
    struct Value {
        V data;
        std::map<K, std::shared_ptr<Value>> children;
        bool selected = false;
    };

    void insert(const K& key, const V& value) {
        auto node = std::make_shared<Value>();
        node->data = value;
        map[key] = node;
    }

    void addChild(const K& parentKey, const K& childKey, const V& childValue) {
        if (map.find(parentKey) != map.end()) {
            auto childNode = std::make_shared<Value>();
            childNode->data = childValue;
            map[parentKey]->children[childKey] = childNode;
            map[childKey] = childNode;
        }
    }
   

    std::shared_ptr<Value> getSelected() const {
        for (const auto& pair : map) {
            if (pair.second->selected) {
                return pair.second;
            }
        }
        return nullptr;
    }

private:
    std::map<K, std::shared_ptr<Value>> map;
};

#endif