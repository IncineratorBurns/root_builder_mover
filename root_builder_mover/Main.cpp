#include "Main.h"
#include "wxGUI_Implementation.h"

bool MyApp::OnInit()
{
	auto* l_frame = new wxGUI::wxGUI();
	l_frame->Show();

	return true;
}
