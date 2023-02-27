#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <stdio.h>

using namespace std;

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

DWORD GetProcId(const std::string_view procName) {
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
			do {
				if (!procName.compare(procEntry.szExeFile)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const std::string_view modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!modName.compare(modEntry.szModule)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

int main() {

	DWORD procId = GetProcId("ac_client.exe"); // Get Process Id PID
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	uintptr_t moduleBase = GetModuleBaseAddress(procId, "ac_client.exe");

	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x17E0A8;

	//cout << "0x" << dynamicPtrBaseAddr << endl;

	std::vector<unsigned int> healthoffset = { 0xEC }; // Health Offset
	uintptr_t healthAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, healthoffset);

	std::vector<unsigned int> pistol_ammo_offset = { 0x12C }; // Pistol Ammo Offset
	uintptr_t pistol_ammo_Addr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, pistol_ammo_offset);

	std::vector<unsigned int> rifle_ammo_offset = { 0x140 }; // Rifle Ammo Offset
	uintptr_t rifle_ammo_Addr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, rifle_ammo_offset);

	/*cout << "0x" << healthAddr << endl;
	cout << "0x" << pistol_ammo_Addr << endl;
	cout << "0x" << rifle_ammo_Addr << endl;*/
	


	int h = 100;
	int pistol = 10;
	int rifle = 100;
	cout << "God Mode: On" << endl;
	cout << "Unlimited Pistol Ammo: On" << endl;
	cout << "Unlimited Rifle Ammo: On" << endl;
	//WriteProcessMemory(hProcess, (BYTE*)healthAddr, &h, sizeof(h), nullptr);
	
	//DWORD health_mem_address = 0x008E279C; //this has to be change everytime restart the game (0x<change here>)
	//DWORD rifle_ammo_mem_address = 0x008E27F0; //this has to be change everytime restart the game (0x<change here>)
	//DWORD pistol_ammo_mem_address = 0x0074E28C; //this has to be change everytime restart the game (0x<change here>)

	while (true) {

		WriteProcessMemory(hProcess, (BYTE*)healthAddr, &h, sizeof(h), nullptr);
		WriteProcessMemory(hProcess, (BYTE*)pistol_ammo_Addr, &pistol, sizeof(pistol), nullptr);
		WriteProcessMemory(hProcess, (BYTE*)rifle_ammo_Addr, &rifle, sizeof(rifle), nullptr);

	//	mem.Write<int>(health2, h);
	//	/*mem.Write<int>(health_mem_address, 10000);
	//	mem.Write<int>(rifle_ammo_mem_address, 10000);
	//	mem.Write<int>(pistol_ammo_mem_address, 10000);

	}
	return 0;




}
