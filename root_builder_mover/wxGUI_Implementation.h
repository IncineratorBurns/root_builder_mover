#pragma once

#include <map>

#include "Profiles.h"
#include "wxGUI.h"

class wxLog;

namespace wxGUI
{
	class wxGUI : public Generated::MyFrame1
	{
		std::map<wxString, wxString> m_config;

		wxLog* m_logger;

		void SetConfig(const wxVector<std::pair<wxString, wxString>>& a_key_vals);

		void ReadConfFromStream(std::map<wxString, Profile>& a_dst, wxInputStream& a_src);
		void ReadConf(std::map<wxString, Profile>& a_dst, const wxString& a_conf_filename);
		void SelectProfile(const wxString& a_name, bool a_setProfileComboVal) const;
		void ProfileListRefresh() const;
		Profile ProfileFromGUI() const;

	protected:

		void OnProfilesComboItemSelected(wxCommandEvent& event) override;
		void OnBtnProfileDel(wxCommandEvent& event) override;
		void OnBrowse(wxCommandEvent& event) override;
		void OnBtnRun(wxCommandEvent& event) override;
		void OnBtnUpdate(wxCommandEvent& event) override;
		void OnBtnSimulate(wxCommandEvent& event) override;
		void PossiblySaveCurrentProfile() const;
		void OnProfilesComboDropDown(wxCommandEvent& event) override;

	public:

		wxGUI();
		~wxGUI();
	};
}
