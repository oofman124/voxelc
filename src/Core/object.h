#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "transform.h"
#include "Rendering/meshRenderer.h"
#include "hierarchyMap.h"

using namespace std;

class Object : public enable_shared_from_this<Object> {
public:
    shared_ptr<Object> parentPtr;
    weak_ptr<Object> Parent;

    string Name = "Object";
    string ClassName = "Object";

    // Constructor
    Object(string name = "Object") 
        : Name(name) {
    }

    // Rule of Five
    virtual ~Object() {
        // Clear parent relationship
        if (auto parentPtr = Parent.lock()) {
            auto& parentChildren = parentPtr->Children;
            parentChildren.erase(
                remove(parentChildren.begin(), parentChildren.end(), shared_from_this()),
                parentChildren.end()
            );
        }

        // Clear children (this will trigger their destructors)
        Children.clear();
    }
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) noexcept = default;
    Object& operator=(Object&&) noexcept = default;

    // Children Management
    vector<shared_ptr<Object>> GetChildren() const {
        return Children;
    }

    shared_ptr<Object> GetParent() const {
        return Parent.lock();
    }

    HierarchyMap<string, string> exportToMap() const {
        HierarchyMap<string, string> hierarchy;
        
        // Add this object
        hierarchy.insert(Name, ClassName);
        
        // Add children recursively
        for (const auto& child : Children) {
            hierarchy.addChild(Name, child->Name, child->ClassName);
            // Recursively add child's children
            for (const auto& grandchild : child->GetChildren()) {
                auto childMap = child->exportToMap();
                hierarchy.addChild(child->Name, grandchild->Name, grandchild->ClassName);
            }
        }
        
        return hierarchy;
    }


    void SetParent(shared_ptr<Object> parent) {
        if (parent) {
            // Remove this object from the current parent's children, if any
            if (auto currentParent = Parent.lock()) {
                auto& currentChildren = currentParent->Children;
                currentChildren.erase(
                    remove(currentChildren.begin(), currentChildren.end(), shared_from_this()),
                    currentChildren.end()
                );
            }

            // Set the new parent and add this object to its children
            Parent = parent;
            parent->Children.push_back(shared_from_this());
        } else {
            // If no parent is provided, just clear the current parent
            if (auto currentParent = Parent.lock()) {
                auto& currentChildren = currentParent->Children;
                currentChildren.erase(
                    remove(currentChildren.begin(), currentChildren.end(), shared_from_this()),
                    currentChildren.end()
                );
            }
            Parent.reset();
        }
    }
    bool AddChild(shared_ptr<Object> child) {
        if (child && !child->Parent.lock()) {
            Children.push_back(child);
            child->Parent = shared_from_this();
            return true;
        }
        return false;
    }

    shared_ptr<Object> operator[](const string& name) {
        return FindFirstChild(name);
    }

    shared_ptr<Object> FindFirstChild(const string& name) const {
        auto it = find_if(Children.begin(), Children.end(),
            [&name](const shared_ptr<Object>& child) {
                return child->Name == name;
            });
        return it != Children.end() ? *it : nullptr;
    }

    bool HasInstance(const string& name) const {
        return FindFirstChild(name) != nullptr;
    }

    bool HasInstance(const shared_ptr<Object>& inst) const {
        return find(Children.begin(), Children.end(), inst) != Children.end();
    }

   
    

    // Hierarchy Management
    string GetFullName() const {
        string fullName = Name;
        auto current = GetParent();
        while (current && current->IsA("Object")) {
            fullName = current->Name + "." + fullName;
            current = current->GetParent();
        }
        return fullName;
    }

    vector<shared_ptr<Object>> GetDescendants() const {
        vector<shared_ptr<Object>> descendants;
        for (const auto& child : Children) {
            descendants.push_back(child);
            auto childDescendants = child->GetDescendants();
            descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
        }
        return descendants;
    }

    bool IsA(const string& className) const {
        return find(AncestorClasses.begin(), AncestorClasses.end(), className) 
            != AncestorClasses.end();
    }

protected:
    void AddAncestorClass(const string& ancestorClass) {
        if (find(AncestorClasses.begin(), AncestorClasses.end(), ancestorClass) 
            == AncestorClasses.end()) {
            AncestorClasses.push_back(ancestorClass);
        }
    }

private:
    vector<string> AncestorClasses = {"Object"};
    vector<shared_ptr<Object>> Children;
};

class PVObject : public Object {
public:
    shared_ptr<Transform> transformPtr;
    shared_ptr<UV_MeshRenderer> meshRendererPtr;
    weak_ptr<Transform> transform;
    weak_ptr<UV_MeshRenderer> meshRenderer;

    PVObject(string name = "PVObject")
        : Object(name) {
        try {
            ClassName = "PVObject";
            AddAncestorClass("PVObject");

            // First create the components
            transformPtr = make_shared<Transform>();
            if (!transformPtr) throw runtime_error("Failed to create Transform");
            
            meshRendererPtr = make_shared<UV_MeshRenderer>(transformPtr);
            if (!meshRendererPtr) throw runtime_error("Failed to create MeshRenderer");
            
            // Assign weak pointers
            transform = transformPtr;
            meshRenderer = meshRendererPtr;

        }
        catch (const exception& e) {
            // Clean up any allocated resources
            transformPtr.reset();
            meshRendererPtr.reset();
            throw;  // Re-throw the exception
        }
    }
};
#endif // INSTANCE_H