#include "Profiles.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/string.h>

#include "Utils.h"

const char* const Profile::PROFILE_KEY_PATH = "path";
const char* const Profile::PROFILE_KEY_DIRS = "dirs";
const char* const Profile::PROFILE_KEY_FILES = "files";
const char* const Profile::PROFILE_KEY_FILES_EXCLUDE = "files_exclude";
const std::map<wxString, wxString> Profile::s_messages = {
	{ "cyberpunk", "Wake up, samurai. We've got a city to burn." }, { "skyrim", "Fus-Ro-Dah!" },
	{ "fallout", "War... War never changes." }
};

Profile::Profile(const wxString& a_name, const wxString& a_mod_path, const wxString& a_dirs,
                 const wxString& a_files, const wxString& a_files_exclude)
	: m_name(a_name)
{
	m_key_vals[PROFILE_KEY_PATH] = a_mod_path;
	m_key_vals[PROFILE_KEY_DIRS] = a_dirs;
	m_key_vals[PROFILE_KEY_FILES] = a_files;
	m_key_vals[PROFILE_KEY_FILES_EXCLUDE] = a_files_exclude;
}

Profile::Profile(const wxString& a_name, const wxVector<std::pair<wxString, wxString>>& a_entries)
	: m_name(a_name)
{
	for (const auto& [l_key, l_val] : a_entries)
		m_key_vals[l_key] = l_val;
}

wxVector<wxString> Profile::GetDirsVec() const
{
	auto l_val = m_key_vals.at(PROFILE_KEY_DIRS);
	l_val.Replace("||", "\n", true);
	return Utils::Tokenize(l_val, "\n");
}

wxVector<wxString> Profile::GetFilesVec() const
{
	auto l_val = m_key_vals.at(PROFILE_KEY_FILES);
	l_val.Replace("||", "\n", true);
	return Utils::Tokenize(l_val, "\n");
}

wxVector<wxString> Profile::GetFileExcludesVec() const
{
	auto l_val = m_key_vals.at(PROFILE_KEY_FILES_EXCLUDE);
	l_val.Replace("||", "\n", true);
	return Utils::Tokenize(l_val, "\n");
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

void Profile::Execute(bool a_simulate) const
{
	if (m_key_vals.at(PROFILE_KEY_PATH).empty())
	{
		wxLogMessage("Mod path is empty, can't continue.");
		return;
	}

	auto l_modspath = fs::path(m_key_vals.at(PROFILE_KEY_PATH).ToStdWstring());
	if (!fs::exists(l_modspath))
	{
		wxLogMessage("Mods path points to a directory or file that doesn't exist, can't continue.");
		return;
	}

	if (m_key_vals.at(PROFILE_KEY_DIRS).empty() && m_key_vals.at(PROFILE_KEY_FILES).empty())
	{
		wxLogMessage("Dirs and files to move are both empty. Nothing to move.");
		return;
	}

	int l_counter = 0;

	if (a_simulate)
		wxLogMessage("!!! This is a simulation. Nothing will be moved.");

	auto l_file_regexes = GetFilesVec();
	auto l_file_exclude_regexes = GetFileExcludesVec();
	auto l_dir_regexes = GetDirsVec();
	auto l_dir_excludes = wxVector<wxString>();

	wxVector<wxString>* l_active_regex_match;
	wxVector<wxString>* l_active_regex_exclude;

	for (auto& l_mod_dir : fs::directory_iterator{ l_modspath }) // iterator is an element in MO mods path
	{
		for (auto& l_mod_child : fs::directory_iterator(l_mod_dir)) // iterator is an item in mod's dir
		{
			// We need the name of this directory, and this is how we do it in C++:
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

				fs::path l_root_path(l_mod_dir.path());
				l_root_path /= "root";

				if (!a_simulate && !fs::exists(l_root_path))
					fs::create_directory(l_root_path);

				l_root_path /= l_object_name.ToStdWstring();

				wxLogMessage("( %d ) Source: %s\n\tDest: %s", l_counter++, l_mod_child.path().wstring(),
				             l_root_path.c_str());

				if (!a_simulate)
					fs::rename(l_mod_child, l_root_path);
			}
		}
	}

	wxLogMessage("Operations executed: %d", l_counter);

	if (a_simulate)
		wxLogMessage("!!! This run was a simulation, so nothing has been moved. Click Run to apply these changes.");
	else
		wxLogMessage("All done. %s", Message());
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
		if (l_msg_profile_name.Contains(m_name.Lower()))
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
