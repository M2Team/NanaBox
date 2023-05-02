#pragma once

#include "pch.h"

#include <filesystem>

bool IsPackagedMode();

std::filesystem::path GetLocalStateFolderPath();

std::wstring GetCurrentProcessModulePath();

bool IsCurrentProcessElevated();
