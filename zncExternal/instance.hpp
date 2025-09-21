#pragma once
#include "memory.hpp"
#include "rbx.hpp"
#include "Update/offsets.hpp"
#include <vector>

class instance {
public:
    uintptr_t address;

    std::vector<uintptr_t> jobs{};

    instance() : address(0) {}
    instance(uintptr_t addr) : address(addr) {}

    std::string get_name() const { 
        uintptr_t name_ptr = memory::read<uintptr_t>(address + offsets::name);
        if (!name_ptr) return "";
        return memory::readstring(name_ptr);
    }

    static std::string get_class_name(uintptr_t address)
    {
        uintptr_t class_ptr = memory::read<uintptr_t>(address + offsets::classdescriptor);
        uintptr_t class_name_ptr = memory::read<uintptr_t>(class_ptr + 0x8);
        std::string name = memory::readstring(class_name_ptr);

        return name;
    }

    std::vector<instance> get_children() const {
        std::vector<instance> children;
        uintptr_t start = memory::read<uintptr_t>(address + offsets::children);
        uintptr_t end = memory::read<uintptr_t>(start + offsets::children_end);

        for (uintptr_t ptr = memory::read<uintptr_t>(start); ptr != end; ptr += 0x10) {
            uintptr_t child_addr = memory::read<uintptr_t>(ptr);
            children.emplace_back(child_addr);
        }

        return children;
    }

	std::vector<instance> get_descendants() const { // get all children and their children recursively
		std::vector<instance> descendants; // vector to hold all descendants

		auto children = get_children(); // get direct children
		for (auto& child : children) { // for each child
			descendants.push_back(child); // add the child to the descendants list

			auto sub_descendants = child.get_descendants(); // get the child's descendants recursively
			descendants.insert(descendants.end(), sub_descendants.begin(), sub_descendants.end()); // add the child's descendants to the main list
        }

		return descendants; // return the full list of descendants
    }

    instance find_first_child(const std::string& target) const {
        for (const auto& child : get_children()) {
            if (child.get_name() == target)
                return child;
        }
        return instance(0);
    }

    bool valid() const {
        return address != 0;
    }
};
