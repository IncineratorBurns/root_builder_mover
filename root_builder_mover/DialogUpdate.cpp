#include "DialogUpdate.h"

void wxGUI::DialogUpdate::OnBtnCancel(wxCommandEvent& event)
{
	m_on_cancel();
	EndModal(wxID_CANCEL);
}

wxGUI::DialogUpdate::DialogUpdate(wxWindow* a_pParent, std::function<void()>&& a_code_on_cancel)
	: Generated::DialogUpdate_Base(a_pParent),
	m_on_cancel(std::move(a_code_on_cancel))
{
	
}
