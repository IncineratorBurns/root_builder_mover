#pragma once

#include "wxGUI.h"

class wxWindow;

namespace wxGUI
{
	class DialogUpdate : public Generated::DialogUpdate_Base
	{
		std::function<void(void)> m_on_cancel;

	protected:
		void OnBtnCancel(wxCommandEvent& event) override;

	public:
		DialogUpdate(wxWindow* a_pParent, std::function<void(void)>&& a_code_on_cancel);
	};
}
