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

class Instance : public enable_shared_from_this<Instance> {
public:
    shared_ptr<Instance> parentPtr;
    weak_ptr<Instance> Parent;

    string Name = "Instance";
    string ClassName = "Instance";

    // Constructor
    Instance(string name = "Instance") 
        : Name(name) {
    }

    // Rule of Five
    virtual ~Instance() {
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
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance(Instance&&) noexcept = default;
    Instance& operator=(Instance&&) noexcept = default;

    // Children Management
    vector<shared_ptr<Instance>> GetChildren() const {
        return Children;
    }

    shared_ptr<Instance> GetParent() const {
        return Parent.lock();
    }

    HierarchyMap<string, string> exportToMap() const {
        HierarchyMap<string, string> hierarchy;
        
        // Add this instance
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


    void SetParent(shared_ptr<Instance> parent) {
        if (parent) {
            // Remove this instance from the current parent's children, if any
            if (auto currentParent = Parent.lock()) {
                auto& currentChildren = currentParent->Children;
                currentChildren.erase(
                    remove(currentChildren.begin(), currentChildren.end(), shared_from_this()),
                    currentChildren.end()
                );
            }

            // Set the new parent and add this instance to its children
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
    bool AddChild(shared_ptr<Instance> child) {
        if (child && !child->Parent.lock()) {
            Children.push_back(child);
            child->Parent = shared_from_this();
            return true;
        }
        return false;
    }

    shared_ptr<Instance> operator[](const string& name) {
        return FindFirstChild(name);
    }

    shared_ptr<Instance> FindFirstChild(const string& name) const {
        auto it = find_if(Children.begin(), Children.end(),
            [&name](const shared_ptr<Instance>& child) {
                return child->Name == name;
            });
        return it != Children.end() ? *it : nullptr;
    }

    bool HasInstance(const string& name) const {
        return FindFirstChild(name) != nullptr;
    }

    bool HasInstance(const shared_ptr<Instance>& inst) const {
        return find(Children.begin(), Children.end(), inst) != Children.end();
    }

   
    

    // Hierarchy Management
    string GetFullName() const {
        string fullName = Name;
        auto current = GetParent();
        while (current && current->IsA("Instance")) {
            fullName = current->Name + "." + fullName;
            current = current->GetParent();
        }
        return fullName;
    }

    vector<shared_ptr<Instance>> GetDescendants() const {
        vector<shared_ptr<Instance>> descendants;
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
    vector<string> AncestorClasses = {"Instance"};
    vector<shared_ptr<Instance>> Children;
};

class PVInstance : public Instance {
public:
    shared_ptr<Transform> transformPtr;
    shared_ptr<UV_MeshRenderer> meshRendererPtr;
    weak_ptr<Transform> transform;
    weak_ptr<UV_MeshRenderer> meshRenderer;

    PVInstance(string name = "PVInstance")
        : Instance(name) {
        try {
            ClassName = "PVInstance";
            AddAncestorClass("PVInstance");

            // First create the components
            transformPtr = make_shared<Transform>();
            if (!transformPtr) throw runtime_error("Failed to create Transform");
            
            meshRendererPtr = make_shared<UV_MeshRenderer>(transformPtr.get());
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