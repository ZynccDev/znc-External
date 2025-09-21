#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

namespace memory {
    inline HANDLE handle = nullptr;
    inline DWORD pid = 0;

    inline bool attach(const std::wstring& proc_name) {
        PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snapshot == INVALID_HANDLE_VALUE) return false;

        while (Process32Next(snapshot, &entry)) {
            if (proc_name == entry.szExeFile) {
                pid = entry.th32ProcessID;
                handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                CloseHandle(snapshot);
                return handle != nullptr;
            }
        }

        CloseHandle(snapshot);
        return false;
    }

    template<typename T>
	T read(uintptr_t addr) { // read memory at address
        T buffer{};
        ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(T), nullptr);
        return buffer;
    }

    template<typename T>
	void write(uintptr_t addr, T value) { // write memory at address
        WriteProcessMemory(handle, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr);
    }

    inline std::string readstring(uintptr_t string) {
		auto length = read<int>(string + 0x18); // read the length of the string from the address + 0x18 (string length offset)

		if (length >= 16) // if length is greater than or equal to 16 we read from the pointer
            string = read<uintptr_t>(string); // read the pointer to the actual string

		std::string str; // string to return
		for (int i = 0; char ch = read<char>(string + i); i += sizeof(char)) { // read each character until we hit a null terminator
            if (ch == '\0') break; // null terminator means string end
			str.push_back(ch); // add character to string
        }

		return str; // return the string
    }

	inline uintptr_t get_module_base(const std::wstring& mod_name) { // get roblox base address
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); // take a snapshot of all modules in the process
		MODULEENTRY32 modEntry = { sizeof(MODULEENTRY32) }; // module entry structure
		uintptr_t base = 0; // base address to return

		if (snapshot == INVALID_HANDLE_VALUE) return 0; // if snapshot is invalid return 0

		while (Module32Next(snapshot, &modEntry)) { // iterate through modules
			if (mod_name == modEntry.szModule) { // if module name matches
				base = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr); // get the base address
				break; // break the loop
            }
        }

		CloseHandle(snapshot); // close the snapshot handle
		return base; // return the base address
    }
}
