#include "apis/ProfileManager.h"
#include "apis/PlatformProfileManager.h"

#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

const static wxCmdLineEntryDesc CmdLineOptions[] = {
	{wxCMD_LINE_PARAM, NULL, NULL, wxT_2("<operation>"), wxCMD_LINE_VAL_STRING, 0},
	{wxCMD_LINE_PARAM, NULL, NULL, wxT_2("<file>"), wxCMD_LINE_VAL_STRING, 0},
	{wxCMD_LINE_NONE},
};

/** RegistryHelper is a executable helper for wxLauncher that will allow it to 
set the required registry keys on Windows Vista and Windows 7 because they both by
default virtualize the registry so that the use of HKLM it redirected to HKCU.

Registry Virtualization in Windows Vista <http://msdn.microsoft.com/en-us/library/bb530198.aspx>

The executable that this app is compiled into *must* *not* have a UAC manifest
so that Win7 and WinVista will Virtualize its access to HKLM. Not having the 
UAC manifest allows us to write into the per user virtual location where FSO
is looking because it also does not have a UAC manifest.  This
virtualiszation also allows us to write the cmdline_fso.cfg file to the 
correct location (data/cmdline_fso.cfg) if FSO has been installed into Program Files. */
class RegistryHelper : public wxApp {
public:
	virtual int OnRun();
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
private:
	wxFileName file;
	enum Operation {
		None,
		Push,
		Pull,
	};
	Operation op;
};

IMPLEMENT_APP(RegistryHelper)

int RegistryHelper::OnRun() {
	wxFile stdoutfile(wxFile::fd_stdout);
	if ( !file.IsOk() ) {
		return ProMan::FileIsNotOK;
	}
	
	wxFileConfig* config;
	if (this->op == Push) {
		if ( !file.FileExists() ) {
			return ProMan::InputFileDoesNotExist;
		}
		wxFFileInputStream f(file.GetFullPath());
		config = new wxFileConfig(f);
	} else {
		config = new wxFileConfig();
	}

	if (this->op == Push) {
		return RegistryPushProfile(config);
	} else if (this->op == Pull) {
		ProMan::RegistryCodes ret;
		ret = RegistryPullProfile(config);
		if ( ret == ProMan::NoError ) {
			wxFFileOutputStream f(file.GetFullPath());
			if (config->Save(f)) {
				return ProMan::NoError;
			} else {
				return ProMan::UnknownError;
			}
		} else {
			return ret;
		}
	} else {
		return ProMan::UnknownError;
	}
}

void RegistryHelper::OnInitCmdLine(wxCmdLineParser &parser) {
	parser.SetSwitchChars(wxT_2("-")); // always use - even on windows
	parser.SetLogo(wxT_2("wxLauncher Registry Tool"));
	parser.SetDesc(CmdLineOptions);
}

bool RegistryHelper::OnCmdLineParsed(wxCmdLineParser &parser) {
	wxString operation = parser.GetParam(0).MakeLower();
	if (operation.StartsWith(wxT_2("push")) ) {
		this->op = Push;
	} else if ( operation.StartsWith(wxT_2("pull")) ) {
		this->op = Pull;
	} else {
		parser.Usage();
		return false;
	}
	file.Assign(parser.GetParam(1));
	return true;
}