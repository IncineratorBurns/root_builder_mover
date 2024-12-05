///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/valtext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/hyperlink.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

namespace wxGUI
{
	namespace Generated
	{

		///////////////////////////////////////////////////////////////////////////////
		/// Class MyFrame1
		///////////////////////////////////////////////////////////////////////////////
		class MyFrame1 : public wxFrame
		{
			private:

			protected:
				wxComboBox* m_comboBox_Profile;
				wxButton* m_button_profile_delete;
				wxStaticLine* m_staticline1;
				wxButton* m_button_download_update;
				wxStaticText* m_staticText1;
				wxStaticText* m_staticText2;
				wxStaticText* m_staticText5;
				wxStaticText* m_staticText3;
				wxStaticText* m_staticText31;
				wxTextCtrl* m_textCtrl_path_mods_folder;
				wxButton* m_button_browse_mod_dir_path;
				wxTextCtrl* m_textCtrl_directories_to_move;
				wxTextCtrl* m_textCtrl_directories_exclude;
				wxTextCtrl* m_textCtrl_file_extensions_to_move;
				wxTextCtrl* m_textCtrl_file_exclude;
				wxRichTextCtrl* m_richText_Log;
				wxHyperlinkCtrl* m_hyperlink1;
				wxHyperlinkCtrl* m_hyperlink2;
				wxButton* m_button6;
				wxButton* m_button5;

				// Virtual event handlers, overide them in your derived class
				virtual void OnProfilesComboItemSelected( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnProfilesComboDropDown( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnBtnProfileDel( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnBtnUpdate( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnBrowse( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnBtnSimulate( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnBtnRun( wxCommandEvent& event ) { event.Skip(); }


			public:
				wxString m_profile_combobox_value;

				MyFrame1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Mover for the Root Builder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 714,547 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

				~MyFrame1();

		};

	} // namespace Generated
} // namespace wxGUI

