/*
Copyright (C) 2009-2010 wxLauncher Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <wx/wx.h>
#include <wx/settings.h>
#include "tabs/ModsPage.h"
#include "global/ids.h"
#include "global/ProfileKeys.h"
#include "controls/ModList.h"
#include "apis/SkinManager.h"
#include "apis/ProfileManager.h"
#include "apis/TCManager.h"
#include "datastructures/FSOExecutable.h"
#include "generated/configure_launcher.h"


#include "global/MemoryDebugging.h" // Last include for memory debugging

const int TEXT_WRAP_WIDTH = TAB_AREA_WIDTH - 225;

ModsPage::ModsPage(wxWindow* parent): wxPanel(parent, wxID_ANY) {
	wxLogDebug(_T("ModsPage is at %p."), this);
	
	SkinSystem::RegisterTCSkinChanged(this);

	TCManager::RegisterTCChanged(this);
	wxCommandEvent nullEvent;
	this->OnTCChanged(nullEvent);
}

BEGIN_EVENT_TABLE(ModsPage, wxPanel)
EVT_COMMAND(wxID_NONE, EVT_TC_CHANGED, ModsPage::OnTCChanged)
EVT_COMMAND(wxID_NONE, EVT_TC_SKIN_CHANGED, ModsPage::OnTCSkinChanged)
END_EVENT_TABLE()

void ModsPage::OnTCChanged(wxCommandEvent &WXUNUSED(event)) {
	wxString tcPath;

	ProMan::GetProfileManager()->ProfileRead(PRO_CFG_TC_ROOT_FOLDER, &tcPath, wxEmptyString);

	wxSizer* currentSizer = this->GetSizer();
	if ( currentSizer != NULL ) {
		currentSizer->Clear(true);
	}
	if ( tcPath.IsEmpty()) {
		SkinSystem::GetSkinSystem()->ResetTCSkin();
		
		wxStaticText* noTC = new wxStaticText(this, wxID_ANY, _("To view a list of available mods, you must first select the root folder of an FS2 Open game on the Basic Settings page."),
											  wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
		noTC->SetFont(SkinSystem::GetSkinSystem()->GetMessageFont());
		noTC->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		noTC->Wrap(TEXT_WRAP_WIDTH);

		wxStaticBitmap* infoImage =
			new wxStaticBitmap(this, ID_MODS_PAGE_INFO_IMAGE, SkinSystem::GetSkinSystem()->GetBigInfoIcon());

		wxBoxSizer* noTCSizer = new wxBoxSizer(wxVERTICAL);
		noTCSizer->AddStretchSpacer(1);
		noTCSizer->Add(infoImage,0, wxALL | wxCENTER);
		noTCSizer->AddSpacer(10);
		noTCSizer->Add(noTC, 0, wxALL | wxCENTER);
		noTCSizer->AddStretchSpacer(1);

#if IS_LINUX
		this->SetSizerAndFit(noTCSizer);
#else
		this->SetSizer(noTCSizer);
#endif
	} else if ( !wxFileName::DirExists(tcPath)  ) {
		SkinSystem::GetSkinSystem()->ResetTCSkin();
		
		wxStaticText* nonexistentTC = new wxStaticText(this, wxID_ANY,
			_("The selected root folder does not exist.\n\nSelect a valid root folder for an FS2 Open game on the Basic Settings page."),
			wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
		nonexistentTC->SetFont(SkinSystem::GetSkinSystem()->GetMessageFont());
		nonexistentTC->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		nonexistentTC->Wrap(TEXT_WRAP_WIDTH);

		wxStaticBitmap* warningImage =
			new wxStaticBitmap(this, ID_MODS_PAGE_WARNING_IMAGE, SkinSystem::GetSkinSystem()->GetBigWarningIcon());

		wxBoxSizer* nonexistentTCSizer = new wxBoxSizer(wxVERTICAL);
		nonexistentTCSizer->AddStretchSpacer(1);
		nonexistentTCSizer->Add(warningImage,0, wxALL | wxCENTER);
		nonexistentTCSizer->AddSpacer(10);
		nonexistentTCSizer->Add(nonexistentTC, 0, wxALL | wxCENTER);
		nonexistentTCSizer->AddStretchSpacer(1);

#if IS_LINUX
		this->SetSizerAndFit(nonexistentTCSizer);
#else
		this->SetSizer(nonexistentTCSizer);
#endif
	} else if ( !FSOExecutable::HasFSOExecutables(wxFileName(tcPath, wxEmptyString)) ) {
		SkinSystem::GetSkinSystem()->ResetTCSkin();
		
		wxStaticText* invalidTC = new wxStaticText(this, wxID_ANY,
			_("The selected root folder does not contain any FS2 Open executables.\n\nOn the Basic Settings page, either select a different root folder, or add FS2 Open executables to the selected root folder and press the Refresh button."),
			wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
		invalidTC->SetFont(SkinSystem::GetSkinSystem()->GetMessageFont());
		invalidTC->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		invalidTC->Wrap(TEXT_WRAP_WIDTH);
		
		wxStaticBitmap* warningImage =
			new wxStaticBitmap(this, ID_MODS_PAGE_WARNING_IMAGE, SkinSystem::GetSkinSystem()->GetBigWarningIcon());
		
		wxBoxSizer* invalidTCSizer = new wxBoxSizer(wxVERTICAL);
		invalidTCSizer->AddStretchSpacer(1);
		invalidTCSizer->Add(warningImage,0, wxALL | wxCENTER);
		invalidTCSizer->AddSpacer(10);
		invalidTCSizer->Add(invalidTC, 0, wxALL | wxCENTER);
		invalidTCSizer->AddStretchSpacer(1);

#if IS_LINUX
		this->SetSizerAndFit(invalidTCSizer);
#else
		this->SetSizer(invalidTCSizer);
#endif
	} else {
#if 0
		wxStaticText* header = new wxStaticText(this, wxID_ANY,
			_("Installed mods.  Click on Install/Update in the left to search, download, and install additional mods and updates."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
		header->Wrap(TAB_AREA_WIDTH);
#endif
		wxSize modGridSize(TAB_AREA_WIDTH - 20, TAB_AREA_HEIGHT); // FIXME for left and right borders of 5 pixels each -- but why does it have to be 20?
		ModList* modGrid = new ModList(this, modGridSize, tcPath);
		modGrid->SetMinSize(modGridSize);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
#if 0
		sizer->Add(header);
#endif
		sizer->Add(modGrid, wxSizerFlags().Center().Border(wxALL,5));

		this->SetMaxSize(wxSize(TAB_AREA_WIDTH, TAB_AREA_HEIGHT));
		this->SetSizer(sizer);
	}
	this->Layout();
}

void ModsPage::OnTCSkinChanged(wxCommandEvent &WXUNUSED(event)) {
	wxStaticBitmap* infoImage = dynamic_cast<wxStaticBitmap*>(
		wxWindow::FindWindowById(ID_MODS_PAGE_INFO_IMAGE, this));
	
	if (infoImage != NULL) {
		infoImage->SetBitmap(SkinSystem::GetSkinSystem()->GetBigInfoIcon());
	}
	
	wxStaticBitmap* warningImage = dynamic_cast<wxStaticBitmap*>(
		wxWindow::FindWindowById(ID_MODS_PAGE_WARNING_IMAGE, this));
	
	if (warningImage != NULL) {
		warningImage->SetBitmap(SkinSystem::GetSkinSystem()->GetBigWarningIcon());
	}
}
