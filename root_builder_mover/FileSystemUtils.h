#pragma once
#include <filesystem>
#include <system_error>

class wxString;
const char* const FUNC_NAME_REMOVE = "DELETE";
const char* const FUNC_NAME_REMOVE_ALL = "DELETE RECURSIVELY";
const char* const FUNC_NAME_RENAME = "MOVE FILE";
const char* const FUNC_NAME_CREATE_DIR = "CREATE DIRECTORY";
const char* const FUNC_NAME_CREATE_DIRS = "CREATE DIRECTORIES";

// STL

bool STLFSFuncAndLog(bool a_simulation, bool (*a_stl_fs_function)(const std::filesystem::path& a_path, std::error_code& a_ec), const char* a_func_name, const
                     wxString& a_meta,
                     const std::filesystem::path& a_path);

bool STLFSFuncAndLog(bool a_simulation, std::uintmax_t(*a_stl_fs_function)(const std::filesystem::path& a_path, std::error_code& a_ec), const char* a_func_name, const
                     wxString& a_meta,
                     const std::filesystem::path& a_path);

bool STLFSFuncAndLog(bool a_simulation, void (*a_stl_fs_function)(const std::filesystem::path& a_old_path, const std::filesystem::path& a_new_path, std::error_code& a_ec), const char* a_func_name, const
                     wxString& a_meta,
                     const std::filesystem::path& a_old_path, const std::filesystem::path& a_new_path);

void MoveFileWithSTL(bool a_simulation, const wxString& a_meta, int& a_fail_counter, const std::filesystem::path& a_old_path,
                     const std::filesystem::path& a_new_path);

// Split a path into two partitions relative to a first encountered a_token.
// a_token_in_first_or_second == 1  ? ret.second begins with a_token.
// a_token_in_first_or_second == -1 ? ret.first ends with a_token.
std::pair<std::filesystem::path, std::filesystem::path> PartitionPath(const std::filesystem::path& a_path, const wxString& a_token, int a_token_in_first_or_second);

// WINAPI

std::wstring WinAPIPath(const std::filesystem::path& a_path);

void MoveWithWinAPI(bool a_simulate, int l_counter, int& l_fail_counter, const std::filesystem::path& l_mod_child,
                    const std::filesystem::path& l_root_path);

