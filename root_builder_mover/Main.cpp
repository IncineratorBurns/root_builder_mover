#include "Main.h"
#include "MainFrame.h"

bool MyApp::OnInit()
{
	auto* l_frame = new wxGUI::MainFrame();
	l_frame->Show();

	return true;
}
