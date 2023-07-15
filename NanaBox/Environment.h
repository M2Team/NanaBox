#pragma once

#include "pch.h"

#include <filesystem>

std::filesystem::path GetLocalStateFolderPath();

std::wstring GetCurrentProcessModulePath();
