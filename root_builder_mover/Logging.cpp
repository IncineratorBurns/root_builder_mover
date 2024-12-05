#include "Logging.h"

#include <system_error>
#include <wx/log.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>

bool PrintLog(const std::error_code& l_ec, int a_counter, const char* a_func_name, const wxString& a_msg1,
              const wxString& a_msg2)
{
	if (l_ec.value() == 0)
		//wxLogMessage("( %d ) RET: SUCCESS: %s [ %s ] -> [ %s ]", a_counter, a_func_name, a_msg1, a_msg2);
	{
		SetTextStyleNormal();
		LoggerCtrl::Instance().m_pLogCtrl->WriteText(wxString::Format("( %d ) RET: SUCCESS: %s [ %s ] -> [ %s ]\n", a_counter, a_func_name, a_msg1, a_msg2));
	}
	else
		//wxLogMessage("( %d ) RET: ERROR: %s [ %s ] -> [ %s ]. MSG: %s", a_counter, a_func_name, a_msg1, a_msg2, l_ec.message());
	{
		SetTextStyleNormal();
		LoggerCtrl::Instance().m_pLogCtrl->WriteText(wxString::Format("( %d ) RET: ERROR: %s [ %s ] -> [ %s ]. MSG: %s\n", a_counter, a_func_name, a_msg1, a_msg2, l_ec.message()));
	}

	return l_ec.value() == 0;
}

bool PrintLog(const std::error_code& l_ec, int a_counter, const char* a_func_name, const wxString& a_msg)
{
	if (l_ec.value() == 0)
		//wxLogMessage("( %d ) RET: SUCCESS: %s [ %s ]", a_counter, a_func_name, a_msg);
	{
		SetTextStyleNormal();
		LoggerCtrl::Instance().m_pLogCtrl->WriteText(wxString::Format("( %d ) RET: SUCCESS: %s [ %s ]\n", a_counter, a_func_name, a_msg));
	}
	else
		//wxLogMessage("( %d ) RET: ERROR: %s [ %s ]. MSG: %s", a_counter, a_func_name, a_msg, l_ec.message());
	{
		SetTextStyleNormal();
		LoggerCtrl::Instance().m_pLogCtrl->WriteText(wxString::Format("( %d ) RET: ERROR: %s [ %s ]. MSG: %s\n", a_counter, a_func_name, a_msg, l_ec.message()));
	}

	return l_ec.value() == 0;
}

bool PrintLog(const wxString& a_message, int a_counter)
{
	//wxLogMessage("( %d ) MOD: %s", a_counter, a_message);
	SetTextStyleNormal();
	LoggerCtrl::Instance().m_pLogCtrl->WriteText(wxString::Format("( %d ) MOD: %s\n", a_counter, a_message));
	return true;
}

bool PrintSeparator()
{
	//wxLogMessage("");
	SetTextStyleNormal();
	LoggerCtrl::Instance().m_pLogCtrl->WriteText("\n");
	return true;
}

bool PrintLineSeparator()
{
	//wxLogMessage("-----------------------------------------------------------------------------------------------");
	SetTextStyleNormal();
	LoggerCtrl::Instance().m_pLogCtrl->WriteText("-----------------------------------------------------------------------------------------------\n");
	return true;
}

void SetTextStyleError()
{
	//LoggerCtrl::Instance().m_pLogCtrl->SetDefaultStyle(wxTextAttr(*wxRED, wxNullColour, wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false)));
	auto l_pCtrl = LoggerCtrl::Instance().m_pLogCtrl;
	l_pCtrl->BeginTextColour(*wxRED);
	l_pCtrl->BeginBold();
}

void SetTextStyleNormal()
{
	//LoggerCtrl::Instance().m_pLogCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK, wxNullColour, wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false)));
	auto l_pCtrl = LoggerCtrl::Instance().m_pLogCtrl;
	l_pCtrl->BeginTextColour(*wxBLACK);
	l_pCtrl->EndBold();
}

LoggerCtrl& LoggerCtrl::Instance(wxRichTextCtrl* a_ctrl)
{
	static LoggerCtrl l_instance;

	if (l_instance.m_pLogCtrl == nullptr && a_ctrl != nullptr)
		l_instance.m_pLogCtrl = a_ctrl;

	return l_instance;
}


