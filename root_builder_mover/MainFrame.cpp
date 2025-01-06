#include "MainFrame.h"

#include <filesystem>
namespace fs = std::filesystem;
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/dirdlg.h>
#include <wx/url.h>
#include <wx/log.h>
#include <wx/filedlg.h>
#include <wx/mstream.h>
#include "MultiLineRenderer.h"
#include "DialogUpdate.h"
#include "Logging.h"

#include "Config.h"
#include "FileSystemUtils.h"
#include "Utils.h"
#include "Profiles.h"

#include <curl/curl.h>
#include <curl/multi.h>

const wxString CONFIG_APP_GROUP = "root_builder_mover";
const char* const CONFIG_FILENAME_BUNDLED_CFG = "bundled.cfg";
const char* const CONFIG_FILENAME_CUSTOM_CFG = "custom.cfg";

const wxString CONFIG_APP_UPDATE_URL = "update_url";
const wxString CONFIG_PROFILE_VAL_UNSET = "unset";

// CONFIG UPDATE RELATED CODE

const size_t ALLOC_SIZE = 32768;
char* g_update_buffer = nullptr;
size_t g_update_buffer_size = 0;
size_t g_update_buffer_cursor = 0;
bool g_cancel_update = false;
bool g_update_applied = false;
const char* const CONFIG_FIRST_LINE = "[root_builder_mover]";

struct ThreadEventPayload
{
	LogItemType m_type;
	wxString m_meta;
	wxString m_message;
};

void msg_main_thread(LogItemType a_logType, const wxString& a_msg)
{
	wxThreadEvent* l_event = new wxThreadEvent;
	ThreadEventPayload l_payload{
		a_logType, META_UPDATER, a_msg
	};
	l_event->SetPayload(l_payload);
	wxTheApp->GetTopWindow()->GetEventHandler()->QueueEvent(l_event);
}

void SetConfig(std::map<wxString, wxString>& a_dst, const wxVector<std::pair<wxString, wxString>>& a_key_vals)
{
	for (const auto& [l_key, l_val] : a_key_vals)
		a_dst[l_key] = l_val;
}

void ReadConfFromStream(std::map<wxString, wxString>* a_gconf_dst, std::map<wxString, Profile>& a_profiles_dst,
                        wxInputStream& a_src)
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
		{
			if (a_gconf_dst != nullptr)
				SetConfig(*a_gconf_dst, l_entries);
		}
		else
			a_profiles_dst[l_group_name] = Profile(l_group_name, l_entries);

		l_fileConfig.SetPath("/");
	}
}

void ReadConf(std::map<wxString, Profile>& a_dst, const wxString& a_conf_filename)
{
	if (fs::exists(a_conf_filename.c_str().AsWChar()))
	{
		wxFileInputStream l_fileStream(a_conf_filename);
		ReadConfFromStream(&g_config, a_dst, l_fileStream);

		PrintInfo(wxString::Format("Config %s loaded.", a_conf_filename), META_CORE);
	}
	else
		PrintInfo(wxString::Format("Config %s not found.", a_conf_filename), META_CORE);
}

bool update_diff()
{
	wxMemoryInputStream l_inputStream(g_update_buffer, g_update_buffer_cursor);

	if (!l_inputStream.IsOk())
		return {};

	std::map<wxString, wxString> l_gconf;
	std::map<wxString, Profile> l_profiles;

	ReadConfFromStream(&l_gconf, l_profiles, l_inputStream);

	bool l_gconfs_same = l_gconf == g_config;
	bool l_profiles_same = l_profiles == ProfilesDB::Instance().m_bundled_profiles;

	return !l_gconfs_same || !l_profiles_same;
}

bool apply_update()
{
	// backup old bundled.cfg:
	auto l_path_bundled_cfg = std::filesystem::path(CONFIG_FILENAME_BUNDLED_CFG);
	auto l_path_bundled_cfg_bak = l_path_bundled_cfg;
	l_path_bundled_cfg_bak += ".bak";

	auto lf_backup_delete = [&l_path_bundled_cfg_bak]()
	{
		auto l_ret = true;

		if (std::filesystem::exists(l_path_bundled_cfg_bak))
			l_ret = STLFSFuncAndLog(false, false, std::filesystem::remove, "std::remove", META_UPDATER,
			                        l_path_bundled_cfg_bak);

		return l_ret;
	};

	auto lf_backup_make = [&l_path_bundled_cfg, &l_path_bundled_cfg_bak, &lf_backup_delete]()
	{
		if (std::filesystem::exists(l_path_bundled_cfg))
		{
			bool l_success = false;

			// remove the old bak:
			l_success = lf_backup_delete();

			// copy the cfg to bak:
			if (l_success)
				l_success = STLFSFuncAndLog(false, false, std::filesystem::copy_file, "std::copy_file", META_UPDATER,
				                            l_path_bundled_cfg, l_path_bundled_cfg_bak);

			if (!l_success)
			{
				msg_main_thread(LogItemType::Error, "Can't proceed with bundled.cfg update as making a backup failed.");
				lf_backup_delete();
				return false;
			}
		}

		// If original cfg doesn't exist, return true, so the logic would proceed further and retrieve the cfg from the server.
		return true;
	};

	auto lf_backup_restore = [&l_path_bundled_cfg, &l_path_bundled_cfg_bak]()
	{
		bool l_success = true;

		if (std::filesystem::exists(l_path_bundled_cfg_bak))
		{
			// remove the active cfg:
			if (std::filesystem::exists(l_path_bundled_cfg))
				l_success = STLFSFuncAndLog(false, false, std::filesystem::remove, "std::remove", META_UPDATER,
				                            l_path_bundled_cfg);

			// copy the bak to cfg:
			if (l_success)
				l_success = STLFSFuncAndLog(false, false, std::filesystem::copy_file, "std::copy_file", META_UPDATER,
				                            l_path_bundled_cfg_bak, l_path_bundled_cfg);

			if (l_success)
				l_success = STLFSFuncAndLog(false, false, std::filesystem::remove, "std::remove", META_UPDATER,
				                            l_path_bundled_cfg_bak);

			if (!l_success)
				msg_main_thread(LogItemType::Error,
				                "Restoring a backup bundled.cfg failed during the update for some reason. Please try again, or if nothing helps download the file from https://raw.githubusercontent.com/IncineratorBurns/root_builder_mover/refs/heads/master/root_builder_mover/bundled.cfg and save it into the folder where this .exe is located.\nAlso, remember that keeping game-related stuff in Program Files might cause issues related to access and ownership of files on modern systems. Keep them outside, like in D:\\Games.");
		}

		return l_success;
	};

	if (!lf_backup_make())
		return false;

	// Open file for writing, associate an output stream with it:
	wxFile l_out_file(l_path_bundled_cfg.wstring(), wxFile::OpenMode::write);
	if (!l_out_file.IsOpened())
	{
		msg_main_thread(LogItemType::Error,
		                "Can't proceed with bundled.cfg update as bundled.cfg can't be open for writing.");
		return false;
	}
	wxFileOutputStream l_outputStream(l_out_file);
	if (!l_outputStream.IsOk())
	{
		msg_main_thread(LogItemType::Error,
		                "Can't proceed with bundled.cfg update as bundled.cfg can't be open for output streaming.");
		return false;
	}

	// Try writing new data into it:
	auto l_result = l_outputStream.WriteAll((const void*)&g_update_buffer[0], g_update_buffer_cursor);

	if (!l_result)
	{
		auto l_bytes_written = l_outputStream.LastWrite();
		msg_main_thread(LogItemType::Error, wxString::Format(
			                "Error during the update. The updated data length is %d bytes, but bytes written is only %d. The update is rolled back.",
			                g_update_buffer_cursor, l_bytes_written));

		l_outputStream.Close();
		l_out_file.Close();
		// Restore backup:
		lf_backup_restore();
		return false;
	}

	l_outputStream.Close();
	l_out_file.Close();
	lf_backup_delete();

	return true;
}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	// If we ran out of space (which is unlikely), realloc the buffer.

	if (g_update_buffer_size <= g_update_buffer_cursor + nmemb)
	{
		size_t l_new_size = g_update_buffer_size + nmemb > ALLOC_SIZE ? nmemb : ALLOC_SIZE;
		g_update_buffer = (char*)realloc(g_update_buffer, l_new_size);
		g_update_buffer_size += l_new_size;
	}

	// Copy the chars:
	for (size_t i = 0; i < nmemb; ++i)
		g_update_buffer[g_update_buffer_cursor + i] = ptr[i];

	// Update the cursor into the buffer
	g_update_buffer_cursor += nmemb;

	// size is always one according to documentation. no idea why they return size*nmemb but ok I guess lol.
	return size * nmemb;
}

void run_curl(const wxString& a_url)
{
	// Reset the cursor on each run. After the run's finished, inspect the success flag.
	// The buffer[0; g_update_buffer_cursor] will hold the new file data.
	g_update_buffer_cursor = 0;
	g_update_applied = false;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	CURL* l_curl = curl_easy_init();
	CURLM* l_curlm = curl_multi_init();

	curl_easy_setopt(l_curl, CURLOPT_URL, a_url.c_str().AsChar());
	curl_easy_setopt(l_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(l_curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(l_curl, CURLOPT_CONNECTTIMEOUT_MS, 4000);
	curl_easy_setopt(l_curl, CURLOPT_TIMEOUT_MS, 8000);
	curl_easy_setopt(l_curl, CURLOPT_WRITEFUNCTION, write_callback);

	int l_curl_running_handles = 1;
	curl_multi_add_handle(l_curlm, l_curl);

	// We don't want to allow this function to work for more than some reasonable amount of time.
	auto l_toc = std::chrono::system_clock::now();
	bool l_failed = false;
	wxString l_msg = "Update is a great success. Very nice.";

	do
	{
		CURLMcode mc = curl_multi_perform(l_curlm, &l_curl_running_handles);

		if (mc == CURLM_OK && l_curl_running_handles >= 1)
			/* wait for activity, timeout or "nothing" */
			mc = curl_multi_poll(l_curlm, nullptr, 0, 1000, nullptr);

		if (mc != CURLM_OK)
		{
			l_msg = wxString::Format("Aborting the update because curl_multi_poll() failed, code %d.\n", (int)mc);
			l_failed = true;
			break;
		}

		auto l_toe = std::chrono::system_clock::now();
		if (l_toe - l_toc >= std::chrono::milliseconds(10000))
		{
			l_msg = "The update is taking way too long. Something must be wrong. Aborting.";
			l_failed = true;
			break;
		}

		if (g_cancel_update)
		{
			l_msg = "Update is cancelled.";
			l_failed = true;
			break;
		}

		/* if there are still transfers, loop */
	}
	while (l_curl_running_handles > 0);

	curl_easy_cleanup(l_curl);
	curl_multi_cleanup(l_curlm);
	curl_global_cleanup();

	if (g_update_buffer_cursor == 0)
	{
		l_msg = "Update URL is unreachable. Aborting.";
		l_failed = true;
	}
	// Check if the first line in the buffer is [root_builder_mover]. If it's not, the buffer contains invalid data:
	else if (std::string(g_update_buffer, std::strlen(CONFIG_FIRST_LINE)) != CONFIG_FIRST_LINE)
	{
		l_msg = wxString::Format("Update server is live but data looks wrong:\n%s",
		                         std::string_view(g_update_buffer, g_update_buffer_cursor));
		l_failed = true;
	}
	else
	{
		if (update_diff())
		{
			msg_main_thread(LogItemType::Info, "bundled.cfg is newer on the update server. Applying the update.");
			l_failed = !apply_update();
		}
		else
			l_msg = "Update is not required. Your bundled.cfg is the latest version.";
	}

	g_update_applied = !l_failed;
	msg_main_thread(l_failed ? LogItemType::Error : LogItemType::Success, l_msg);
}

// END

void wxGUI::MainFrame::SelectProfile(const wxString& a_name, bool a_setProfileComboVal) const
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

void wxGUI::MainFrame::ProfileListRefresh() const
{
	auto l_selected_profile = m_comboBox_Profile->GetValue();
	m_comboBox_Profile->Clear();

	auto l_all_profiles = ProfilesDB::Instance().GetMergedProfilesMap();

	for (const auto& l_profile : l_all_profiles)
		m_comboBox_Profile->Append(l_profile.first);

	m_comboBox_Profile->SetValue(l_selected_profile);
}

Profile wxGUI::MainFrame::ProfileFromGUI() const
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

void wxGUI::MainFrame::OnProfilesComboItemSelected(wxCommandEvent& event)
{
	SelectProfile(m_comboBox_Profile->GetValue(), false);
}

void wxGUI::MainFrame::OnBtnProfileDel(wxCommandEvent& event)
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

void wxGUI::MainFrame::OnBrowse(wxCommandEvent& event)
{
	wxDirDialog l_dlg(NULL, "Select mods directory inside Mod Organizer's profile for your game", "",
	                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	auto l_result = l_dlg.ShowModal();
	if (l_result == wxID_OK)
		m_textCtrl_path_mods_folder->SetValue(l_dlg.GetPath());
}

void wxGUI::MainFrame::OnBtnRun(wxCommandEvent& event)
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

void wxGUI::MainFrame::delete_update_thread()
{
	if (m_update_thread != nullptr)
	{
		if (m_update_thread->joinable())
			m_update_thread->join();

		delete m_update_thread;
		m_update_thread = nullptr;
	}
}

void wxGUI::MainFrame::OnThreadEvent(wxThreadEvent& a_event)
{
	auto l_payload = a_event.GetPayload<ThreadEventPayload>();
	switch (l_payload.m_type)
	{
	case LogItemType::All:
		break;
	case LogItemType::Info:
		PrintInfo(l_payload.m_message, l_payload.m_meta);
		break;
	case LogItemType::Error:
		PrintError(l_payload.m_message, l_payload.m_meta);
		delete_update_thread();
		if (m_p_dialog_update != nullptr)
			if (m_p_dialog_update->IsVisible())
				m_p_dialog_update->Close();
		break;
	case LogItemType::Success:
		PrintSuccess(l_payload.m_message, l_payload.m_meta);
		delete_update_thread();
		if (m_p_dialog_update != nullptr)
			if (m_p_dialog_update->IsVisible())
				m_p_dialog_update->Close();
		if (g_update_applied)
		{
			ReadConf(ProfilesDB::Instance().m_bundled_profiles, CONFIG_FILENAME_BUNDLED_CFG);
			ProfileListRefresh();
		}
		break;
	}
}

void wxGUI::MainFrame::OnBtnUpdate(wxCommandEvent& event)
{
	try
	{
		if (m_update_thread == nullptr)
		{
			m_update_thread = new std::thread(run_curl, g_config[CONFIG_APP_UPDATE_URL]);

			if (m_p_dialog_update == nullptr)
				m_p_dialog_update = new DialogUpdate(this, [this]()
				{
					g_cancel_update = true;
					this->delete_update_thread();
				});

			m_p_dialog_update->ShowModal();
		}
	}
	catch (const std::exception& l_ex)
	{
		PrintError(l_ex.what(), META_UPDATER);
	}
}

wxGUI::MainFrame::MainFrame()
	: MainFrame_Base(nullptr)
{
	//m_logger = new wxLogTextCtrl(m_textCtrl_log);
	//m_logger->SetActiveTarget(m_logger);
	//LoggerCtrl::Instance(m_richText_Log);

	SetIcon(wxICON(AAAICON));

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

	Bind(wxEVT_THREAD, &MainFrame::OnThreadEvent, this);
}

wxGUI::MainFrame::~MainFrame()
{
	PossiblySaveCurrentProfile();

	WriteConf(ProfilesDB::Instance().m_custom_profiles, CONFIG_FILENAME_CUSTOM_CFG);

	if (m_update_thread != nullptr)
	{
		m_update_thread->join();
		delete m_update_thread;
	}

	free(g_update_buffer);
}

void wxGUI::MainFrame::OnBtnSimulate(wxCommandEvent& event)
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

void wxGUI::MainFrame::PossiblySaveCurrentProfile() const
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

void wxGUI::MainFrame::OnProfilesComboDropDown(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();
}

void wxGUI::MainFrame::OnBtnLogClear(wxCommandEvent& event)
{
	Logger::Instance().Clear();
}

void wxGUI::MainFrame::OnBtnLogExport(wxCommandEvent& event)
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

void wxGUI::MainFrame::OnFilterChoice(wxCommandEvent& event)
{
	auto l_choice = m_choice_Filter->GetSelection();
	Logger::Instance().Filter(static_cast<LogItemType>(l_choice));
}

void wxGUI::MainFrame::OnBtnSave(wxCommandEvent& event)
{
	PossiblySaveCurrentProfile();
	WriteConf(ProfilesDB::Instance().m_custom_profiles, CONFIG_FILENAME_CUSTOM_CFG);
}

void wxGUI::MainFrame::WriteConf(const std::map<wxString, Profile>& a_profiles, const char* a_filename)
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
