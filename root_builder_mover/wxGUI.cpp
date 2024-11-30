///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxGUI.h"

///////////////////////////////////////////////////////////////////////////
using namespace wxGUI::Generated;

MyFrame1::MyFrame1( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVEBORDER ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Profile") ), wxHORIZONTAL );

	m_comboBox_Profile = new wxComboBox( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox_Profile->SetValidator( wxTextValidator( wxFILTER_NONE, &m_profile_combobox_value ) );

	sbSizer1->Add( m_comboBox_Profile, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_button_profile_delete = new wxButton( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_button_profile_delete, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	sbSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	m_button_download_update = new wxButton( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Update..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_download_update->SetToolTip( wxT("Check if there is an updated list of profiles and download it if there is one. This won't overwrite your custom.cfg file.") );

	sbSizer1->Add( m_button_download_update, 0, wxALL, 5 );


	bSizer1->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Profile Config") ), wxHORIZONTAL );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("MO profile mod folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer7->Add( m_staticText1, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Directories to move:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer7->Add( m_staticText2, 0, wxALL, 5 );

	m_staticText3 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Files to move:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer7->Add( m_staticText3, 0, wxALL, 5 );

	m_staticText31 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Exclude files:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	bSizer7->Add( m_staticText31, 0, wxALL, 5 );


	sbSizer2->Add( bSizer7, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl_path_mods_folder = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl_path_mods_folder->SetToolTip( wxT("Mod Organizer's game profile mods folder.") );

	bSizer5->Add( m_textCtrl_path_mods_folder, 1, wxALL, 2 );

	m_button_browse_mod_dir_path = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );

	m_button_browse_mod_dir_path->SetBitmap( wxNullBitmap );
	bSizer5->Add( m_button_browse_mod_dir_path, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2 );


	bSizer8->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl_directories_to_move = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl_directories_to_move->SetToolTip( wxT("PCRE regular expressions delimited with \"||\".\nNote: white space characters are real characters and thus will impact the match logic.\nExample:\n^bin$||^r4$||^red4ext$\n^ - beginning of a word, $ - end of a word.\nThus, the line will match dirs bin, r4 and red4ext.\n") );

	bSizer4->Add( m_textCtrl_directories_to_move, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );


	bSizer8->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl_file_extensions_to_move = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl_file_extensions_to_move->SetToolTip( wxT("PCRE regular expressions delimited with \"||\".\nNote: white space characters are real characters and thus will impact the match logic.\nExamples:\n1. Same as for dirs but will match files, not dirs.\n2. .*.exe$||.*.dll$ - match all exe and dll files.\n3. .* - match all files.") );

	bSizer6->Add( m_textCtrl_file_extensions_to_move, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );


	bSizer8->Add( bSizer6, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl_file_exclude = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl_file_exclude->SetToolTip( wxT("PCRE regular expressions delimited with \"||\".\nNote: white space characters are real characters and thus will impact the match logic.\nExamples:\n1. Same as above.\n2. ^meta.ini$") );

	bSizer61->Add( m_textCtrl_file_exclude, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2 );


	bSizer8->Add( bSizer61, 1, wxEXPAND, 5 );


	sbSizer2->Add( bSizer8, 1, wxEXPAND, 5 );


	bSizer1->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Log") ), wxVERTICAL );

	m_textCtrl_log = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	sbSizer3->Add( m_textCtrl_log, 1, wxALL|wxEXPAND, 5 );


	bSizer1->Add( sbSizer3, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );

	m_hyperlink1 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("PCRE cheatsheet"), wxT("https://github.com/niklongstone/regular-expression-cheat-sheet"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer91->Add( m_hyperlink1, 0, wxALL, 5 );

	m_hyperlink2 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("REGEX playground (set it to PCRE2)"), wxT("https://regex101.com/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer91->Add( m_hyperlink2, 0, wxALL, 5 );


	bSizer81->Add( bSizer91, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_button6 = new wxButton( this, wxID_ANY, wxT("Simulate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button6, 0, wxALL, 5 );

	m_button5 = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button5, 0, wxALL, 5 );


	bSizer81->Add( bSizer9, 0, 0, 5 );


	bSizer1->Add( bSizer81, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_comboBox_Profile->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MyFrame1::OnProfilesComboItemSelected ), NULL, this );
	m_comboBox_Profile->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( MyFrame1::OnProfilesComboDropDown ), NULL, this );
	m_button_profile_delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnProfileDel ), NULL, this );
	m_button_download_update->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnUpdate ), NULL, this );
	m_button_browse_mod_dir_path->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBrowse ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnSimulate ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnRun ), NULL, this );
}

MyFrame1::~MyFrame1()
{
	// Disconnect Events
	m_comboBox_Profile->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MyFrame1::OnProfilesComboItemSelected ), NULL, this );
	m_comboBox_Profile->Disconnect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( MyFrame1::OnProfilesComboDropDown ), NULL, this );
	m_button_profile_delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnProfileDel ), NULL, this );
	m_button_download_update->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnUpdate ), NULL, this );
	m_button_browse_mod_dir_path->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBrowse ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnSimulate ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame1::OnBtnRun ), NULL, this );

}
