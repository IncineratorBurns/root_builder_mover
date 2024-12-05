#pragma once
#include <system_error>

class wxRichTextCtrl;
class wxString;

bool PrintLog(const std::error_code& l_ec, int a_counter, const char* a_func_name, const wxString& a_msg1, const wxString& a_msg2);
bool PrintLog(const std::error_code& l_ec, int a_counter, const char* a_func_name, const wxString& a_msg);
bool PrintLog(const wxString& a_message, int a_counter);
bool PrintSeparator();
bool PrintLineSeparator();
void SetTextStyleError();
void SetTextStyleNormal();

class LoggerCtrl
{
public:
    static LoggerCtrl& Instance(wxRichTextCtrl* a_ctrl = nullptr);
    wxRichTextCtrl* m_pLogCtrl = nullptr;

private:
    LoggerCtrl() {}

public:
    LoggerCtrl(const LoggerCtrl&) = delete;
    void operator=(const LoggerCtrl&) = delete;
};
