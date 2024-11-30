#pragma once
#include <map>
#include <wx/string.h>
#include <wx/vector.h>

struct Profile
{
	static const char* const PROFILE_KEY_PATH;
	static const char* const PROFILE_KEY_DIRS;
	static const char* const PROFILE_KEY_FILES;
	static const char* const PROFILE_KEY_FILES_EXCLUDE;

	static const std::map<wxString, wxString> s_messages;

	wxString m_name;

	std::map<wxString, wxString> m_key_vals{
		{ PROFILE_KEY_PATH, "" }, { PROFILE_KEY_DIRS, "" }, { PROFILE_KEY_FILES, "" }
	};

	Profile() = default;
	Profile(const wxString& a_name, const wxString& a_mod_path, const wxString& a_dirs,
	        const wxString& a_files, const wxString& a_files_exclude);
	Profile(const wxString& a_name, const wxVector<std::pair<wxString, wxString>>& a_entries);

	wxVector<wxString> GetDirsVec() const;
	wxVector<wxString> GetFilesVec() const;
	wxVector<wxString> GetFileExcludesVec() const;

	const wxString& operator[](const char* a_key) const;
	wxString& operator[](const char* a_key);
	bool operator==(const Profile& a_profile) const;

	void Execute(bool a_simulate) const;

	bool IsEmpty() const;

	wxString Message() const;
};

// Because of the Static Initialization Order Fiasco this is a singleton. 
struct ProfilesDB
{
	std::map<wxString, Profile> m_bundled_profiles;
	std::map<wxString, Profile> m_custom_profiles;

	std::map<wxString, Profile> GetMergedProfilesMap() const;

	// Regardless of input or profile existence this always returns a valid Profile object.
	// Further logic is expected to do the sanity check.
	Profile GetProfile(const wxString& a_name) const;

	static ProfilesDB& Instance();

	ProfilesDB(const ProfilesDB&) = delete;
	void operator=(const ProfilesDB&) = delete;

private:
	ProfilesDB()
	{
	}
};
