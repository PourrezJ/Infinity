#include "pch.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "types.h"
#include "Scripts.h"
#include "Plugins.h"
#include "DayZUtils.h"
#include "Patterns.h"
#include "Engine.h"

namespace fs = std::filesystem;

void* Infinity::PluginSystem::pScriptModule;


void Infinity::PluginSystem::LoadPlugins(void* pThis)
{
	pScriptModule = pThis;

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	std::string directory = std::string(buffer).substr(0, pos) + "\\Plugins\\";


	if (!fs::exists(directory))
	{
		std::cout << "Plugin directory does not exist. Should exist @ " << directory << std::endl;
		return;
	}

	std::cout << "Loading plugins from: " << directory << std::endl;
	std::string ext(".dll");
	for (auto& p : fs::recursive_directory_iterator(directory))
	{
		if (p.path().extension() == ext)
		{
			std::cout << "\tFound Plugin: " << p.path().stem().string() << std::endl;
			HMODULE hLib = LoadLibrary(p.path().c_str());
			if (hLib)
			{
				ONPLUGINLOAD OnPluginLoad = (ONPLUGINLOAD)GetProcAddress(hLib, "?OnPluginLoad@@YAXXZ");
				OnPluginLoad();
			}

		}
	}
}

bool Infinity::Enfusion::GetProfilePath(char* pResult)
{
	printf("[Plugin] GetProfilePath: Looking for profile path from plugin\n");
	return DayZ::Engine::GetProfilePath(pResult);
}
bool Infinity::Enfusion::RegisterKeyPath(const char* directory, const char* key, bool allow_write)
{

	if (fs::exists(directory))
	{
		void* pFileHandler = DayZ::Engine::GetFileHandler();
		printf("[Plugin] RegisterKeyPath: File Handler @ %p\n", pFileHandler);

		DayZ::Engine::RegisterPathKey(pFileHandler, directory, key, allow_write);

		return true;
	}
	return false;
}
void Infinity::Enfusion::PrintToConsole(const char* format, ...)
{
	char buffer[1024];
	va_list argptr;
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	std::string plugin_format = "[Plugin] ";
	plugin_format += buffer;

	DayZ::Utils::PrintToConsole(plugin_format.c_str()); //already formatted :)
}

void* Infinity::Utils::FindPattern(const char* binary_ninja_pattern)
{
	printf("[Plugin] FindPattern: Looking for pattern in DayZ: %s\n", binary_ninja_pattern);
	return Patterns::FindBinaryNinjaPattern(std::string(binary_ninja_pattern), GetModuleHandle(NULL), 0);
}
void* Infinity::Utils::FindPattern(const char* pattern, const char* mask)
{
	printf("[Plugin] FindPattern: Looking for pattern in DayZ: %s | %s\n", pattern, mask);
	return Patterns::FindPatternMask(pattern, mask, GetModuleHandle(NULL), 0);
}

bool Infinity::Enfusion::Enscript::RunFunction(void* object, const char* function, long long arg4)
{
	printf("[Plugin] RunFunction: Running function %s @ %p...\n", function, object);
	return DayZ::Scripts::RunFunction(object, function, arg4);
}
void Infinity::Enfusion::Enscript::RegisterFunction(const char* name, void* function)
{
	printf("[Plugin] RegisterFunction: Registering plugin function %s @ %p...\n", name, function);
	DayZ::Scripts::RegisterGlobal(Infinity::PluginSystem::pScriptModule, name, function);
}
void* Infinity::Enfusion::Enscript::RegisterObject(const char* name)
{
	printf("[Plugin] RegisterObject: Registering plugin object %s\n", name);
	return DayZ::Scripts::RegisterObject(Infinity::PluginSystem::pScriptModule, name);
}
void Infinity::Enfusion::Enscript::RegisterFunctionForObject(void* pObject, const char* name, void* function, bool use_special_stack)
{
	printf("[Plugin] RegisterFunction: Registering plugin function %s @ %p...\n", name, function);
	DayZ::Scripts::Register(Infinity::PluginSystem::pScriptModule, pObject, name, function, use_special_stack);
}

void Infinity::Logging::Print(const char* format, ...)
{
	printf("[Plugin] \t");
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stdout, format, argptr);
	va_end(argptr);
	printf("\n");
}
void Infinity::Logging::Error(const char* format, ...)
{
	printf("(E) [Plugin] \t");
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
	printf("\n");
}