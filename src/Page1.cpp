/////////////////////////////////////////////////////////
// Graphics Driver Updater
// Copyright (c) 2012, Ashley Gullen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the distribution.
// * Neither the name of the Scirra nor the names of its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
//  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include "stdafx.h"
#include "GfxDriverUpdater.h"
#include "Page1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPage1::CPage1()
	: CPropertyPage(CPage1::IDD)
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFOTEXT1, m_InfoText1);
}

BEGIN_MESSAGE_MAP(CPage1, CPropertyPage)
END_MESSAGE_MAP()

BOOL CPage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_InfoText1.SetWindowText(L"Your graphics card is the part of your computer that displays applications, games and videos to your screen.  "
		L"The graphics driver is the program which talks to your graphics card.\n\nUpdating your graphics driver can help make your computer "
		L"run faster and crash less often.  Some applications and games also require that you have the latest graphics driver, since old drivers "
		L"can cause problems.\n\nThis program will help you identify your graphics card and update its driver.  Click 'Next' to continue.");

	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_NEXT);
	parent->SetWindowText(L"Graphics driver updater");

	return TRUE;  // return TRUE  unless you set the focus to a control
}
