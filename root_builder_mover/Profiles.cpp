#include "Profiles.h"


#include <strsafe.h>

#include <filesystem>
#include <iostream>

#include "Logging.h"
namespace fs = std::filesystem;
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/string.h>

#include "Utils.h"
#include "FileSystemUtils.h"

const char* const Profile::PROFILE_KEY_PATH = "path";
const char* const Profile::PROFILE_KEY_DIRS = "dirs";
const char* const Profile::PROFILE_KEY_DIRS_EXCLUDE = "dirs_exclude";
const char* const Profile::PROFILE_KEY_FILES = "files";
const char* const Profile::PROFILE_KEY_FILES_EXCLUDE = "files_exclude";
const std::map<wxString, wxString> Profile::s_messages = {
	{ "cyberpunk", "Wake up, samurai. We've got a city to burn." }, { "skyrim", "Fus-Ro-Dah!" },
	{ "fallout", "War... War never changes." }
};

Profile::Profile(const wxString& a_name, const wxString& a_mod_path, const wxString& a_dirs,
                 const wxString& a_dirs_exclude,
                 const wxString& a_files, const wxString& a_files_exclude)
	: m_name(a_name)
{
	m_key_vals[PROFILE_KEY_PATH] = a_mod_path;
	m_key_vals[PROFILE_KEY_DIRS] = a_dirs;
	m_key_vals[PROFILE_KEY_DIRS_EXCLUDE] = a_dirs_exclude;
	m_key_vals[PROFILE_KEY_FILES] = a_files;
	m_key_vals[PROFILE_KEY_FILES_EXCLUDE] = a_files_exclude;
}

Profile::Profile(const wxString& a_name, const wxVector<std::pair<wxString, wxString>>& a_entries)
	: m_name(a_name)
{
	for (const auto& [l_key, l_val] : a_entries)
		m_key_vals[l_key] = l_val;
}

wxVector<wxString> Profile::Dirs() const
{
	return Utils::ReplaceDelimiterAndTokenize(m_key_vals.at(PROFILE_KEY_DIRS));
}

wxVector<wxString> Profile::DirsExclude() const
{
	return Utils::ReplaceDelimiterAndTokenize(m_key_vals.at(PROFILE_KEY_DIRS_EXCLUDE));
}

wxVector<wxString> Profile::Files() const
{
	return Utils::ReplaceDelimiterAndTokenize(m_key_vals.at(PROFILE_KEY_FILES));
}

wxVector<wxString> Profile::FilesExclude() const
{
	return Utils::ReplaceDelimiterAndTokenize(m_key_vals.at(PROFILE_KEY_FILES_EXCLUDE));
}

const wxString& Profile::operator[](const char* a_key) const
{
	return m_key_vals.at(a_key);
}

wxString& Profile::operator[](const char* a_key)
{
	return m_key_vals[a_key];
}

bool Profile::operator==(const Profile& a_profile) const
{
	return m_name == a_profile.m_name && m_key_vals == a_profile.m_key_vals;
}

ProfilesDB& ProfilesDB::Instance()
{
	static ProfilesDB l_ret;
	return l_ret;
}

std::map<wxString, Profile> ProfilesDB::GetMergedProfilesMap() const
{
	std::map<wxString, Profile> l_ret;
	for (const auto& l_profile : m_bundled_profiles)
		l_ret[l_profile.first] = l_profile.second;
	for (const auto& l_profile : m_custom_profiles)
		l_ret[l_profile.first] = l_profile.second;
	return l_ret;
}


struct Match
{
	wxString m_name;
	fs::path m_path;
	std::vector<fs::path> m_files;
	std::vector<fs::path> m_empty_dirs;
};

void Profile::Execute(bool a_simulate) const
{
	Logger::Instance().Clear();

	if (m_key_vals.at(PROFILE_KEY_PATH).empty())
	{
		PrintInfo("Mod path is empty, can't continue.", "Profile config");
		return;
	}

	auto l_modspath = fs::path(m_key_vals.at(PROFILE_KEY_PATH).ToStdWstring());
	if (!fs::exists(l_modspath))
	{
		PrintInfo("Mods path points to something that doesn't exist, can't continue.", "Profile config");
		return;
	}

	if (m_key_vals.at(PROFILE_KEY_DIRS).empty() && m_key_vals.at(PROFILE_KEY_FILES).empty())
	{
		PrintInfo("Dirs and files to move are both empty. Nothing to move.", "Profile config");
		return;
	}

	int l_counter = 0;
	int l_fail_counter = 0;

	if (a_simulate)
		PrintInfo("This run is a simulation. No changes will be applied to your system.", META_CORE);

	auto l_file_regexes = Files();
	auto l_file_exclude_regexes = FilesExclude();
	auto l_dir_regexes = Dirs();
	auto l_dir_excludes = DirsExclude();

	wxVector<wxString>* l_active_regex_match;
	wxVector<wxString>* l_active_regex_exclude;

	for (auto& l_mod_dir : fs::directory_iterator{ l_modspath }) // iterator is an element in MO mods path
	{
		std::vector<Match> l_matches_in_mod;

		// Detect all the files and directories matching the set regexes:
		for (auto& l_mod_child : fs::directory_iterator(l_mod_dir)) // iterator is an item in mod's dir
		{
			// The name of currently processed file or directory inside mod's folder shall be this:
			auto l_object_name = wxString(l_mod_child.path().filename());

			if (l_mod_child.is_directory())
			{
				l_active_regex_match = &l_dir_regexes;
				l_active_regex_exclude = &l_dir_excludes;
			}
			else
			{
				l_active_regex_match = &l_file_regexes;
				l_active_regex_exclude = &l_file_exclude_regexes;
			}

			bool l_exclude = false;
			for (wxRegEx l_excl : *l_active_regex_exclude)
			{
				if (l_excl.Matches(l_object_name))
				{
					l_exclude = true;
					break;
				}
			}

			if (l_exclude)
				continue;

			for (wxRegEx l_regex : *l_active_regex_match)
			{
				if (!l_regex.Matches(l_object_name))
					continue;

				l_matches_in_mod.emplace_back(l_object_name, l_mod_child.path());
			}
		}

		// For each matching directory, traverse it recuresively to detect all the regular files and save their paths.
		// Also, save all the empty directories - their paths are also to be recreated.
		for (auto& l_match : l_matches_in_mod)
			if (is_directory(l_match.m_path))
				for (auto l_it : fs::recursive_directory_iterator(l_match.m_path))
					if (l_it.is_regular_file())
						l_match.m_files.push_back(l_it.path());
					else if (l_it.is_directory() && fs::is_empty(l_it))
						l_match.m_empty_dirs.emplace_back(l_it.path());

		// For each detected file path, reconstruct the directory trees relative to the root folder within the mod:
		for (auto& l_match : l_matches_in_mod)
		{
			for (auto& l_path : l_match.m_files)
			{
				auto l_new_path = l_match.m_path.parent_path();
				l_new_path /= "root";

				fs::path l_path_starting_with_match_name = PartitionPath(l_path, l_match.m_name, 1).second;
				l_new_path /= l_path_starting_with_match_name;

				if (fs::exists(l_new_path))
					STLFSFuncAndLog(a_simulate, fs::remove_all, FUNC_NAME_REMOVE_ALL, l_mod_dir.path().filename().wstring(), l_new_path);

				MoveFileWithSTL(a_simulate, l_mod_dir.path().filename().wstring(), l_fail_counter, l_path, l_new_path);

				++l_counter;
			}

			for (auto& l_path : l_match.m_empty_dirs)
			{
				auto l_new_path = l_match.m_path.parent_path();
				l_new_path /= "root";

				fs::path l_path_starting_with_match_name = PartitionPath(l_path, l_match.m_name, 1).second;
				l_new_path /= l_path_starting_with_match_name;

				if (fs::exists(l_new_path) && !is_directory(l_new_path))
				{
					STLFSFuncAndLog(a_simulate, fs::remove_all, FUNC_NAME_REMOVE_ALL, l_mod_dir.path().filename().wstring(), l_new_path);
					STLFSFuncAndLog(a_simulate, fs::create_directories, FUNC_NAME_CREATE_DIRS, l_mod_dir.path().filename().wstring(), l_new_path);
				}
				else if (!fs::exists(l_new_path))
					STLFSFuncAndLog(a_simulate, fs::create_directories, FUNC_NAME_CREATE_DIRS, l_mod_dir.path().filename().wstring(), l_new_path);

				++l_counter;
			}

			STLFSFuncAndLog(a_simulate, std::filesystem::remove_all, FUNC_NAME_REMOVE_ALL, l_mod_dir.path().filename().wstring(),
			                l_match.m_path);
		}

		//MoveWithWinAPI(a_simulate, l_counter, l_fail_counter, l_mod_child.path(), l_root_path);
	}

	PrintInfo(wxString::Format("Errors: %d.", l_fail_counter), META_CORE);

	if (a_simulate)
		PrintInfo("This run was a simulation, so nothing has been touched. Click Run to apply these changes.", META_CORE);
	else
		PrintInfo(wxString::Format("All done. %s", Message()), META_CORE);
}

bool Profile::IsEmpty() const
{
	for (const auto& [l_key, l_value] : m_key_vals)
		if (!l_value.empty())
			return false;

	return m_name.empty();
}

wxString Profile::Message() const
{
	for (const auto& [l_msg_profile_name, l_msg] : Profile::s_messages)
		if (l_msg_profile_name.Lower().Contains(m_name.Lower()))
			return l_msg;

	return "";
}

Profile ProfilesDB::GetProfile(const wxString& a_name) const
{
	if (a_name.empty())
		return Profile();

	auto l_merged_profiles = GetMergedProfilesMap();

	if (l_merged_profiles.count(a_name) == 1)
		return l_merged_profiles.at(a_name);
	else
		return Profile();
}
