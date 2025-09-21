#pragma once
#include "instance.hpp"
#include <iostream>

namespace rbx {

    inline instance get_datamodel() {
		uintptr_t base = memory::get_module_base(L"RobloxPlayerBeta.exe"); // we get the base of roblox
		uintptr_t fake_dm = memory::read<uintptr_t>(base + offsets::fakedatamodel); // we read the fakedatamodel from the base + offset
		uintptr_t real_dm = memory::read<uintptr_t>(fake_dm + offsets::fakedm_to_dm); // we read the real datamodel from the fakedatamodel + offset
		return instance(real_dm); // we return the datamodel instance
    }

    inline instance get_service(const std::string& name) {
        instance dm = get_datamodel();
		return dm.find_first_child(name); // we return the service instance
	} // we get the service from the datamodel (datamodel is game)

    inline instance get_local_humanoid() {
        instance workspace = get_service("Workspace"); 
        instance players = get_service("Players");
       
		uintptr_t local_player_ptr = memory::read<uintptr_t>(players.address + offsets::localplayer); // Players + localplayer
		instance local_player(local_player_ptr); // we make an instance of the local player

		instance character = local_player.find_first_child("Character"); // we get the character from the local player
        if (!character.valid())
            character = workspace.find_first_child(local_player.get_name()); // checks workspace for the character because thats where our head, hrp is etc

		return character.find_first_child("Humanoid"); // we return the humanoid
    }

	inline instance get_hrp() {
		instance workspace = get_service("Workspace");
		instance players = get_service("Players");

		uintptr_t local_player_ptr = memory::read<uintptr_t>(players.address + offsets::localplayer); // Players + localplayer
		instance local_player(local_player_ptr); // we make an instance of the local player

		instance character = local_player.find_first_child("Character"); // we get the character from the local player
		if (!character.valid())
			character = workspace.find_first_child(local_player.get_name()); // checks workspace for the character because thats where our head, hrp is etc

		return character.find_first_child("HumanoidRootPart"); // we return the hrp
	}

	inline instance get_char() {
		instance workspace = get_service("Workspace");
		instance players = get_service("Players");

		uintptr_t local_player_ptr = memory::read<uintptr_t>(players.address + offsets::localplayer);
		instance local_player(local_player_ptr);

		std::string player_name = local_player.get_name();
		instance character = workspace.find_first_child(player_name); // Character in Workspace

		return character;
	}



    inline float get_current_walkspeed() {
		instance humanoid = get_local_humanoid(); // game.Players.LocalPlayer.Character.Humanoid
		if (!humanoid.valid()) return 16.0f; // if there is a humanoid then we return the default walkspeed

		return memory::read<float>(humanoid.address + offsets::walkspeed); // we are reading the current walkspeed and not writing it
    }

	inline float get_current_jumppower() {
		instance humanoid = get_local_humanoid(); // game.Players.LocalPlayer.Character.Humanoid
		if (!humanoid.valid()) return 50.0f; // if there is a humanoid then we return the default jumppower

		return memory::read<float>(humanoid.address + offsets::jumppower); // we are reading the current jumppower and not writing it
	}

	inline float get_current_health() {
		instance humanoid = get_local_humanoid(); // game.Players.LocalPlayer.Character.Humanoid
		if (!humanoid.valid()) return 100.0f; // if there is a humanoid then we return the normal health

		return memory::read<float>(humanoid.address + offsets::health); // we are reading the current health and not writing it
	}

	inline float get_gravity() {
		instance workspace = get_service("Workspace"); // we get the workspace service
		if (!workspace.valid()) return false; // if there is no workspace we return false

		uintptr_t ptr1 = memory::read<uintptr_t>(workspace.address + offsets::This);
		if (!ptr1) return false; // if there is no this pointer we return false

		uintptr_t ptr2 = memory::read<uintptr_t>(ptr1 + offsets::GravityInfo);
		if (!ptr2) return false; // if there is no gravityinfo we return false

		float gravity = memory::read<float>(ptr2 + offsets::Gravity); // we read the gravity from the address
	}

	inline void set_walkspeed(float speed) {
		instance humanoid = get_local_humanoid();
		if (!humanoid.valid()) return; // if there is no humanoid we return
		memory::write<float>(humanoid.address + offsets::walkspeed, speed); // we write the walkspeed
		memory::write<float>(humanoid.address + offsets::walkspeedcheck, speed); // we write the walkspeedcheck to prevent kicks
	}

    inline void set_jumppower(float power) {
		instance humanoid = get_local_humanoid(); // game.Players.LocalPlayer.Character.Humanoid
		if (!humanoid.valid()) return; // if there is no humanoid we return
		memory::write<float>(humanoid.address + offsets::jumppower, power); // no jumppowercheck so we just write it
    }

	inline void set_health(float newhealth) {
		instance humanoid = get_local_humanoid(); // game.Players.LocalPlayer.Character.Humanoid
		if (!humanoid.valid()) return; // if there is no humanoid we return
		memory::write<float>(humanoid.address + offsets::health, newhealth);
	}

    inline bool set_gravity(float g) {
		instance workspace = get_service("Workspace"); // we get the workspace service
		if (!workspace.valid()) return false; // if there is no workspace we return false

        uintptr_t ptr1 = memory::read<uintptr_t>(workspace.address + offsets::This);
		if (!ptr1) return false; // if there is no this pointer we return false

        uintptr_t ptr2 = memory::read<uintptr_t>(ptr1 + offsets::GravityInfo);
		if (!ptr2) return false; // if there is no gravityinfo we return false

        uintptr_t gravity_addr = ptr2 + offsets::Gravity; // offset
		memory::write<float>(gravity_addr, g); // we write the gravity to the address

		float verify = memory::read<float>(gravity_addr); // we read it back to verify
		return (verify == g); // if it is the same as what we wrote then we return true
    }

	inline instance get_char_from_player(const instance& player) {
		if (!player.valid()) return instance(0);
		instance workspace = get_service("Workspace");
		return workspace.find_first_child(player.get_name());
	}

}