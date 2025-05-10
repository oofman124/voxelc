#ifndef HIERARCHY_MAP_H
#define HIERARCHY_MAP_H

#include <map>
#include <string>
#include <memory>
#include <sstream>
#include "imgui.h"

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

    void renderImGuiTree(const K& rootKey) const {
        auto it = map.find(rootKey);
        if (it != map.end()) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            if (it->second->children.empty()) {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }
            if (it->second->selected) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool opened = ImGui::TreeNodeEx((rootKey + ": " + it->second->data).c_str(), flags);
            
            if (ImGui::IsItemClicked()) {
                // Deselect all
                for (auto& pair : map) {
                    pair.second->selected = false;
                }
                it->second->selected = true;
            }

            if (opened) {
                for (const auto& child : it->second->children) {
                    renderImGuiTree(child.first);
                }
                ImGui::TreePop();
            }
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