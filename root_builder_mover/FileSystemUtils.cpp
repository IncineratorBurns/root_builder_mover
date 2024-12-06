#include "FileSystemUtils.h"

#include <windows.h>

#include <filesystem>
#include <wx/log.h>

#include "Logging.h"

// STL

bool STLFSFuncAndLog(bool a_simulation,
                     bool (*a_stl_fs_function)(const std::filesystem::path& a_path, std::error_code& a_ec),
                     const char* a_func_name,
                     const wxString& a_meta, const std::filesystem::path& a_path)
{
	std::error_code l_ec;
	if (!a_simulation)
		a_stl_fs_function(a_path, l_ec);
	return PrintLog(l_ec, a_meta, a_func_name, a_path.wstring());
}

bool STLFSFuncAndLog(bool a_simulation,
                     std::uintmax_t (*a_stl_fs_function)(const std::filesystem::path& a_path, std::error_code& a_ec),
                     const char* a_func_name, const wxString& a_meta, const std::filesystem::path& a_path)
{
	std::error_code l_ec;
	if (!a_simulation)
		a_stl_fs_function(a_path, l_ec);
	return PrintLog(l_ec, a_meta, a_func_name, a_path.wstring());
}

bool STLFSFuncAndLog(bool a_simulation,
                     void (*a_stl_fs_function)(const std::filesystem::path& a_old_path,
                                               const std::filesystem::path& a_new_path, std::error_code& a_ec),
                     const char* a_func_name, const wxString& a_meta, const std::filesystem::path& a_old_path,
                     const std::filesystem::path& a_new_path)
{
	std::error_code l_ec;
	if (!a_simulation)
		a_stl_fs_function(a_old_path, a_new_path, l_ec);
	return PrintLog(l_ec, a_meta, a_func_name, a_old_path.wstring(), a_new_path.wstring());
}

void MoveFileWithSTL(bool a_simulation, const wxString& a_meta, int& a_fail_counter,
                     const std::filesystem::path& a_old_path,
                     const std::filesystem::path& a_new_path)
{
	auto l_new_file_parent = a_new_path.parent_path();
	if (!std::filesystem::exists(l_new_file_parent))
		STLFSFuncAndLog(a_simulation, std::filesystem::create_directories, FUNC_NAME_CREATE_DIRS, a_meta,
		                l_new_file_parent);

	if (!STLFSFuncAndLog(a_simulation, std::filesystem::rename, FUNC_NAME_RENAME, a_meta, a_old_path,
	                     a_new_path))
		++a_fail_counter;
}

std::pair<std::filesystem::path, std::filesystem::path> PartitionPath(const std::filesystem::path& a_path,
	const wxString& a_token,
	int a_token_in_first_or_second)
{
	auto l_ret = std::make_pair(std::filesystem::path(), std::filesystem::path());

	std::filesystem::path* l_dst = &l_ret.first;

	bool l_token_encountered = false;
	for (const auto& l_it : a_path)
	{
		if (!l_token_encountered && l_it.c_str() == a_token)
		{
			if (a_token_in_first_or_second == 1)
				l_dst = &l_ret.second;
			else
			{
				*l_dst /= l_it;
				l_dst = &l_ret.second;
				l_token_encountered = true;
				continue;
			}

			l_token_encountered = true;
		}

		*l_dst /= l_it;
	}

	return l_ret;
}

// WINAPI

std::wstring WinAPIPath(const std::filesystem::path& a_path)
{
	std::filesystem::path l_ret = R"(\\?\)";
	l_ret += a_path;
	return l_ret.wstring();
}

void MoveWithWinAPI(bool a_simulate, int l_counter, int& l_fail_counter, const std::filesystem::path& l_mod_child,
                    const std::filesystem::path& l_root_path)
{
	wxLogMessage("( %d ) SRC: %s", l_counter, l_mod_child.wstring());
	wxLogMessage("( %d ) DST: %s", l_counter, l_root_path.c_str());

	if (!a_simulate)
	{
		// Prepend "\\?\" to paths, to safeguard against the MAX_PATH limitation:
		auto l_src = WinAPIPath(l_mod_child);
		auto l_dst = WinAPIPath(l_root_path);

		auto l_ret = MoveFileExW(l_src.c_str(), l_dst.c_str(), MOVEFILE_COPY_ALLOWED);


		if (l_ret != 0)
			wxLogMessage("( %d ) RET: SUCCESS", l_counter);
		else
		{
			auto l_ec = GetLastError();
			auto l_error_message = wxSysErrorMsgStr(l_ec);
			wxLogMessage("( %d ) RET: ERROR: %s", l_counter, l_error_message);
			++l_fail_counter;
		}
	}
}
