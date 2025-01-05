#pragma once

#include <map>
//#include <wx/webrequest.h
#include <thread>

#include "Profiles.h"
#include "wxGUI.h"

class wxMessageDialog;
class wxLog;

void SetConfig(std::map<wxString, wxString>& a_dst, const wxVector<std::pair<wxString, wxString>>& a_key_vals);
void ReadConfFromStream(std::map<wxString, wxString>* a_gconf_dst, std::map<wxString, Profile>& a_profiles_dst, wxInputStream& a_src);
void ReadConf(std::map<wxString, Profile>& a_dst, const wxString& a_conf_filename);

namespace wxGUI
{
	class DialogUpdate;

	class MainFrame : public Generated::MainFrame_Base
	{
		std::thread* m_update_thread = nullptr;
		void delete_update_thread();
		DialogUpdate* m_p_dialog_update = nullptr;

		void SelectProfile(const wxString& a_name, bool a_setProfileComboVal) const;
		void ProfileListRefresh() const;
		Profile ProfileFromGUI() const;

		void OnThreadEvent(wxThreadEvent& a_event);

	protected:

		void OnProfilesComboItemSelected(wxCommandEvent& event) override;
		void OnBtnProfileDel(wxCommandEvent& event) override;
		void OnBrowse(wxCommandEvent& event) override;
		void OnBtnRun(wxCommandEvent& event) override;
		void OnBtnUpdate(wxCommandEvent& event) override;
		void OnBtnSimulate(wxCommandEvent& event) override;
		void PossiblySaveCurrentProfile() const;
		void OnProfilesComboDropDown(wxCommandEvent& event) override;
		void OnBtnLogClear(wxCommandEvent& event) override;
		void OnBtnLogExport(wxCommandEvent& event) override;
		void OnFilterChoice(wxCommandEvent& event) override;
		void OnBtnSave(wxCommandEvent& event) override;

		void WriteConf(const std::map<wxString, Profile>& a_profiles, const char* a_filename);

	public:

		MainFrame();
		~MainFrame();
	};
}
