#include "wxGUI_Implementation.h"

#include <filesystem>
#include <wx/msgdlg.h>

#include "Logging.h"
#include "MultiLineRenderer.h"
#include "Profiles.h"
#include "Utils.h"
namespace fs = std::filesystem;

#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/dirdlg.h>
#include <wx/url.h>
#include <wx/log.h>
#include <wx/filedlg.h>

const wxString CONFIG_APP_GROUP = "root_builder_mover";
const char* const CONFIG_FILENAME_BUNDLED_CFG = "bundled.cfg";
const char* const CONFIG_FILENAME_CUSTOM_CFG = "custom.cfg";

const wxString CONFIG_APP_UPDATE_URL = "update_url";
const wxString CONFIG_PROFILE_VAL_UNSET = "unset";

void wxGUI::wxGUI::SetConfig(const wxVector<std::pair<wxString, wxString>>& a_key_vals)
{
	for (const auto& [l_key, l_val] : a_key_vals)
		m_config[l_key] = l_val;
}

void wxGUI::wxGUI::ReadConfFromStream(std::map<wxString, Profile>& a_dst, wxInputStream& a_src)
{
	if (!a_src.IsOk())
		return;

	wxFileConfig l_fileConfig(a_src);

	wxString l_group_name;
	long l_group_index;

	for (bool l_group_detected = l_fileConfig.GetFirstGroup(l_group_name, l_group_index); l_group_detected;
	     l_group_detected = l_fileConfig.GetNextGroup(l_group_name, l_group_index))
	{
		l_fileConfig.SetPath(l_group_name);

		wxString l_key;
		long l_entry_index;

		wxVector<std::pair<wxString, wxString>> l_entries;
		for (bool l_entry_detected = l_fileConfig.GetFirstEntry(l_key, l_entry_index); l_entry_detected;
		     l_entry_detected = l_fileConfig.GetNextEntry(l_key, l_entry_index))
		{
			auto l_val = l_fileConfig.Read(l_key);
			l_entries.push_back({ l_key, l_val });
		}

		if (l_group_name == CONFIG_APP_GROUP)
			SetConfig(l_entries);
		else
			a_dst[l_group_name] = Profile(l_group_name, l_entries);

		l_fileConfig.SetPath("/");
	}
}

void wxGUI::wxGUI::ReadConf(std::map<wxString, Profile>& a_dst, const wxString& a_conf_filename)
{
	if (fs::exists(a_conf_filename.c_str().AsWChar()))
	{
		wxFileInputStream l_fileStream(a_conf_filename);
		ReadConfFromStream(a_dst, l_fileStream);

		PrintInfo(wxString::Format("Config %s loaded.", a_conf_filename), META_CORE);
	}
	else
		PrintInfo(wxString::Format("Config %s not found.", a_conf_filename), META_CORE);
}

void wxGUI::wxGUI::SelectProfile(const wxString& a_name, bool a_setProfileComboVal) const
{
	auto l_selected_profile = ProfilesDB::Instance().GetProfile(a_name);

	m_textCtrl_path_mods_folder->SetValue(l_selected_profile[Profile::PROFILE_KEY_PATH]);
	m_textCtrl_directories_to_move->SetValue(l_selected_profile[Profile::PROFILE_KEY_DIRS]);
	m_textCtrl_directories_exclude->SetValue(l_selected_profile[Profile::PROFILE_KEY_DIRS_EXCLUDE]);
	m_textCtrl_file_extensions_to_move->SetValue(l_selected_profile[Profile::PROFILE_KEY_FILES]);
	m_textCtrl_file_exclude->SetValue(l_selected_profile[Profile::PROFILE_KEY_FILES_EXCLUDE]);

	if (a_setProfileComboVal)
		m_comboBox_Profile->SetValue(l_selected_profile.m_name);
}

void wxGUI::wxGUI::ProfileListRefresh() const
{
	auto l_selected_profile = m_comboBox_Profile->GetValue();
	m_comboBox_Profile->Clear();

	auto l_all_profiles = ProfilesDB::Instance().GetMergedProfilesMap();

	for (const auto& l_profile : l_all_profiles)
		m_comboBox_Profile->Append(l_profile.first);

	m_comboBox_Profile->SetValue(l_selected_profile);
}

Profile wxGUI::wxGUI::ProfileFromGUI() const
{
	return {
		m_comboBox_Profile->GetValue(),
		m_textCtrl_path_mods_folder->GetValue(),
		m_textCtrl_directories_to_move->GetValue(),
		m_textCtrl_directories_exclude->GetValue(),
		m_textCtrl_file_extensions_to_move->GetValue(),
		m_textCtrl_file_exclude->GetValue()
	};
}

void wxGUI::wxGUI::OnProfilesComboItemSelected(wxCommandEvent& event)
{
	SelectProfile(m_comboBox_Profile->GetValue(), false);
}

void wxGUI::wxGUI::OnBtnProfileDel(wxCommandEvent& event)
{
	if (m_comboBox_Profile->IsTextEmpty())
		return;

	if (ProfilesDB::Instance().m_custom_profiles.count(m_comboBox_Profile->GetValue()) == 0)
	{
		wxMessageBox("This is a bundled profile template, can't delete it.");
		return;
	}

	ProfilesDB::Instance().m_custom_profiles.erase(m_comboBox_Profile->GetValue());

	SelectProfile("", true);

	ProfileListRefresh();
}

void wxGUI::wxGUI::OnBrowse(wxCommandEvent& event)
{
	wxDirDialog l_dlg(NULL, "Select mods directory inside Mod Organizer's profile for your game", "",
	                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	auto l_result = l_dlg.ShowModal();
	if (l_result == wxID_OK)
		m_textCtrl_path_mods_folder->SetValue(l_dlg.GetPath());
}

void wxGUI::wxGUI::OnBtnRun(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();

	auto l_profile_name = m_comboBox_Profile->GetValue();

	if (l_profile_name.empty())
	{
		PrintInfo("No profile selected. Can't do anything.", META_CORE);
		return;
	}

	auto l_profile = ProfilesDB::Instance().GetProfile(l_profile_name);
	l_profile.Execute(false);
}

void wxGUI::wxGUI::OnBtnUpdate(wxCommandEvent& event)
{
	try
	{
		wxURL l_url(m_config.at(CONFIG_APP_UPDATE_URL));

		auto l_error = l_url.GetError();

		if (l_error != wxURL_NOERR)
		{
			auto l_err_msg = std::array<std::pair<const char*, const char*>, 7>{
				std::make_pair("wxURL_NOERR", "No error."),
				std::make_pair("wxURL_SNTXERR", "Syntax error in the URL string."),
				std::make_pair("wxURL_NOPROTO", "Found no protocol which can get this URL."),
				std::make_pair("wxURL_NOHOST", "A host name is required for this protocol."),
				std::make_pair("wxURL_NOPATH", "A path is required for this protocol."),
				std::make_pair("wxURL_CONNERR", "Connection error."),
				std::make_pair("wxURL_PROTOERR", "An error occurred during negotiation.")
			};

			PrintError(wxString::Format("Failed to get the update file. %s (%s)", l_err_msg[l_error].second, l_err_msg[l_error].first),
			           META_UPDATER);
			return;
		}

		//wxString l_htmldata;
		wxInputStream* l_inputStream = l_url.GetInputStream();

		if (l_inputStream && l_inputStream->IsOk())
		{
			//wxStringOutputStream l_outputStream(&l_htmldata);
			//l_inputStream->Read(l_outputStream);
			//wxLogMessage(l_htmldata);
			ReadConfFromStream(ProfilesDB::Instance().m_bundled_profiles, *l_inputStream);
			WriteConf(ProfilesDB::Instance().m_bundled_profiles, CONFIG_FILENAME_BUNDLED_CFG);
		}
		delete l_inputStream;
	}
	catch (const std::exception& l_ex)
	{
		PrintError(l_ex.what(), META_UPDATER);
	}
}

wxGUI::wxGUI::wxGUI()
	: MyFrame1(nullptr)
{
	//m_logger = new wxLogTextCtrl(m_textCtrl_log);
	//m_logger->SetActiveTarget(m_logger);
	//LoggerCtrl::Instance(m_richText_Log);

	Logger::Instance(m_dataViewListCtrl_Log);


	m_dataViewListCtrl_Log->GetColumn(2)->SetWidth(FromDIP(200));

	// Add a multiline text column to the log dataview:
	auto l_renderer = new MultiLineCustomRenderer();
	auto l_column = new wxDataViewColumn("Info", l_renderer, 3, FromDIP(200), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	l_column->SetMinWidth(FromDIP(150));
	m_dataViewListCtrl_Log->AppendColumn(l_column);

	m_dataViewListCtrl_Log->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	ReadConf(ProfilesDB::Instance().m_bundled_profiles, CONFIG_FILENAME_BUNDLED_CFG);
	ReadConf(ProfilesDB::Instance().m_custom_profiles, CONFIG_FILENAME_CUSTOM_CFG);
	ProfileListRefresh();
}

wxGUI::wxGUI::~wxGUI()
{
	PossiblySaveCurrentProfile();

	WriteConf(ProfilesDB::Instance().m_custom_profiles, CONFIG_FILENAME_CUSTOM_CFG);
}

void wxGUI::wxGUI::OnBtnSimulate(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();

	auto l_profile_name = m_comboBox_Profile->GetValue();

	if (l_profile_name.empty())
	{
		PrintInfo("No profile selected. Can't do anything.", META_CORE);
		return;
	}

	auto l_profile = ProfilesDB::Instance().GetProfile(l_profile_name);
	l_profile.Execute(true);
}

void wxGUI::wxGUI::PossiblySaveCurrentProfile() const
{
	auto l_profile_from_gui = ProfileFromGUI();

	// If all fields are empty, do nothing:
	if (l_profile_from_gui.IsEmpty())
		return;

	// Don't save the profile if it's not been modified from the one in bundles:
	if (ProfilesDB::Instance().m_bundled_profiles.count(l_profile_from_gui.m_name) == 1 && ProfilesDB::Instance().
		m_bundled_profiles.at(l_profile_from_gui.m_name) == l_profile_from_gui)
		return;

	// If profile name is empty, generate it.
	if (l_profile_from_gui.m_name.empty())
		l_profile_from_gui.m_name = wxString::Format("Profile %s", wxNow());

	ProfilesDB::Instance().m_custom_profiles[l_profile_from_gui.m_name] = l_profile_from_gui;
	ProfileListRefresh();
}

void wxGUI::wxGUI::OnProfilesComboDropDown(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();
}

void wxGUI::wxGUI::OnBtnLogClear(wxCommandEvent& event)
{
	Logger::Instance().Clear();
}

void wxGUI::wxGUI::OnBtnLogExport(wxCommandEvent& event)
{
	wxFileDialog l_dlg(this, "Save log as text file", wxEmptyString, "root_builder_mover.log.txt");
	if (l_dlg.ShowModal() == wxID_CANCEL)
		return;

	wxFile l_out(l_dlg.GetPath(), wxFile::write);
	if (!l_out.IsOpened())
	{
		PrintError(wxString::Format("Can't open file %s for writing.", l_dlg.GetPath()), "[ EXPORT ]");
		return;
	}

	for (const auto& l_item : Logger::Instance().m_all_events)
		if (Logger::Instance().m_filter == LogItemType::All || Logger::Instance().m_filter == l_item.m_type)
			l_out.Write(wxString::Format("%d\t%s\t%s\t%s\n\n", l_item.m_sn, l_item.TypeLiteral(), l_item.m_meta,
			                             l_item.m_info));

	if (l_out.Error())
	{
		PrintError(wxString::Format("Couldn't export the log. Last error is %s",
			wxSysErrorMsgStr(l_out.GetLastError())), META_CORE);

		l_out.Close();
	}

	l_out.Close();

	PrintSuccess("Log exported successfully", META_CORE);
}

void wxGUI::wxGUI::OnFilterChoice(wxCommandEvent& event)
{
	auto l_choice = m_choice_Filter->GetSelection();
	Logger::Instance().Filter(static_cast<LogItemType>(l_choice));
}

void wxGUI::wxGUI::OnBtnSave(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();
	WriteConf(ProfilesDB::Instance().m_custom_profiles, CONFIG_FILENAME_CUSTOM_CFG);
}

void wxGUI::wxGUI::WriteConf(const std::map<wxString, Profile>& a_profiles, const char* a_filename)
{
	try
	{
		if (fs::exists(a_filename))
			fs::remove(a_filename);

		wxFileOutputStream l_fileOutputStream(a_filename);
		if (!l_fileOutputStream.IsOk())
		{
			PrintError(wxString::Format("Couldn't open %s for writing.", a_filename), META_CORE);
			wxMessageBox(wxString::Format("Couldn't open %s for writing.", a_filename));
		}
		else
		{
			wxFileConfig l_profiles_cfg;

			for (const auto& l_profile : a_profiles)
			{
				l_profiles_cfg.SetPath(wxString("/") + l_profile.first);
				for (const auto& l_key_val : l_profile.second.m_key_vals)
					l_profiles_cfg.Write(l_key_val.first, l_key_val.second);
			}

			auto l_ret = l_profiles_cfg.Save(l_fileOutputStream);
			if (l_ret)
				PrintSuccess(wxString::Format("Saved %s successfully.", a_filename), META_CORE);
			else
				PrintError(wxString::Format("Failed saving %s.", a_filename), META_CORE);
		}
	}
	catch (std::exception& l_exception)
	{
		PrintError(wxString::Format("WriteConf has failed because %s", l_exception.what()), META_CORE);
		wxMessageBox(wxString::Format("WriteConf has failed because %s", l_exception.what()));
	}
}
