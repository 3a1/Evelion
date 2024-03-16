#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>

class Memory
{
private:
	std::uintptr_t processId = 0;
	void* processHandle = nullptr;

public:
	// Constructor that finds the process id
	// and opens a handle
	Memory(std::string_view processName) noexcept
	{
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(::PROCESSENTRY32);

		const HANDLE snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (::Process32Next(snapShot, &entry))
		{
			if (!processName.compare(entry.szExeFile))
			{
				processId = entry.th32ProcessID;
				processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
				break;
			}
		}

		// Free handle
		if (snapShot)
			::CloseHandle(snapShot);
	}

	// Destructor that frees the opened handle
	~Memory()
	{
		if (processHandle)
			::CloseHandle(processHandle);
	}

	// Returns the base address of a module by name
	std::uintptr_t GetModuleAddress(std::string_view moduleName) const noexcept
	{
		DWORD_PTR dwModuleBaseAddress = 0;
		DWORD_PTR result = 0;

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 ModuleEntry32;
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do {
					if (_stricmp(ModuleEntry32.szModule, moduleName.data()) == 0)
					{
						dwModuleBaseAddress = reinterpret_cast<DWORD_PTR>(ModuleEntry32.modBaseAddr);
						result = dwModuleBaseAddress;
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
		}
		return result;
	}

	// Read process memory
	template <typename T>
	const T Read(const std::uintptr_t address) const noexcept
	{
		T value = { };
		::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	// Write process memory
	template <typename T>
	void Write(const std::uintptr_t address, const T& value) const noexcept
	{
		::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}

	bool ReadModuleMemory(std::string_view moduleName, void* buffer, size_t size) const noexcept
	{
		std::uintptr_t moduleBase = GetModuleAddress(moduleName);
		if (moduleBase == 0)
			return false;

		return ReadProcessMemory(processHandle, reinterpret_cast<const void*>(moduleBase), buffer, size, nullptr) != 0;
	}

	bool ReadHugeMemory(std::uintptr_t moduleBase, void* buffer, size_t size) const noexcept
	{
		if (moduleBase == 0)
			return false;

		return ReadProcessMemory(processHandle, reinterpret_cast<const void*>(moduleBase), buffer, size, nullptr) != 0;
	}

	template <typename T>
	const T ReadModuleBuffer(const void* baseAddress, const std::uintptr_t offset) const noexcept
	{
		T value = { };
		::memcpy(&value, reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(baseAddress) + offset), sizeof(T));
		return value;
	}
};