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
#include "Page2.h"

#include <comdef.h>
#include <WbemIdl.h>
#pragma comment(lib, "wbemuuid.lib")

IMPLEMENT_DYNAMIC(CPage2, CPropertyPage)

CPage2::CPage2()
	: CPropertyPage(CPage2::IDD),
	  nojoy(false)
{
}

CPage2::~CPage2()
{
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFOTEXT1, m_Text);
	DDX_Control(pDX, IDC_RESULTICON, m_ResultIcon);
}

BEGIN_MESSAGE_MAP(CPage2, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_INFOTEXT1, &CPage2::OnNMClickInfotext1)
END_MESSAGE_MAP()

const wchar_t* GetMonthName(int x)
{
	switch (x) {
	case 1:
		return L"January";
	case 2:
		return L"February";
	case 3:
		return L"March";
	case 4:
		return L"April";
	case 5:
		return L"May";
	case 6:
		return L"June";
	case 7:
		return L"July";
	case 8:
		return L"August";
	case 9:
		return L"September";
	case 10:
		return L"October";
	case 11:
		return L"November";
	case 12:
		return L"December";
	default:
		return L"";
	}
}

const wchar_t* plural(int x)
{
	return x != 1 ? L"s" : L"";
}

std::wstring GetOSName()
{
	std::wstring os_name;
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);

	int vercode = osv.dwMajorVersion * 10 + osv.dwMinorVersion;

	switch (vercode) {
	case 50:	os_name = L"Windows 2000";		break;
	case 51:	os_name = L"Windows XP";		break;
	case 52:	os_name = L"Windows XP";		break;
	case 60:	os_name = L"Windows Vista";		break;
	case 61:	os_name = L"Windows 7";			break;
	case 62:	os_name = L"Windows 8";			break;
	}

	if (!os_name.empty())
	{
		SYSTEM_INFO si;
		GetNativeSystemInfo(&si);

		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
			os_name += L" (32-bit)";
		else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			os_name += L" (64-bit)";
	}

	return os_name;
}

BOOL CPage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_FINISH);

	// Obtain the initial locator to WMI
	IWbemLocator *pLoc = NULL;

	HRESULT hres = CoCreateInstance(
		CLSID_WbemLocator,
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		NoJoy();
		return TRUE;
	}

	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres))
	{
		pLoc->Release();
		NoJoy();
		return TRUE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		NoJoy();
		return TRUE;
	}

	// Use the IWbemServices pointer to make requests of WMI
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), 
		bstr_t("SELECT * FROM Win32_VideoController"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		NoJoy();
		return TRUE;
	}

	// Get the data from the query
	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;
	std::wstring driverdate;

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
			break;

		VARIANT vtProp;
		pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		adaptername = vtProp.bstrVal;
		VariantClear(&vtProp);

		pclsObj->Get(L"DriverDate", 0, &vtProp, 0, 0);
		driverdate = vtProp.bstrVal;
		VariantClear(&vtProp);

		pclsObj->Release();

		// Keep going until we find something that looks valid
		if (!adaptername.empty() && !driverdate.empty())
			break;
	}

	if (adaptername.empty() || driverdate.length() < 6)
	{
		pSvc->Release();
		pLoc->Release();
		pEnumerator->Release();
		NoJoy();
		return TRUE;
	}

	// WMI datetime format is yyyymmddhhmmss... etc. just extract year and month to get the age in months
	int driveryear = FromString<int>(driverdate.substr(0, 4));
	int drivermonth = FromString<int>(driverdate.substr(4, 2));

	SYSTEMTIME systime;
	GetSystemTime(&systime);
	int curyear = systime.wYear;
	int curmonth = systime.wMonth;

	// Work out months since 1970 for both
	int drivermonths1970 = (12 * (driveryear - 1970)) + drivermonth;
	int curmonths1970 = (12 * (curyear - 1970)) + curmonth;
	int age_in_months = curmonths1970 - drivermonths1970;

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();

	// Don't trust the driver date if it comes out negative or over 20 years old
	if (age_in_months < 0 || age_in_months > 240)
	{
		NoJoy();
		return TRUE;
	}

	std::wostringstream ss;
	ss << L"Your graphics card was successfully detected";

	if (age_in_months <= 12)
		ss << L" and the driver appears to be up to date.";
	else if (age_in_months <= 24)
		ss << L" and the driver appears to be reasonably up to date.";
	else
		ss << L" but the driver appears to be over two years old.";

	ss << L"\n\nName: " << adaptername << L" (<a>copy</a>)\nDriver date: " << GetMonthName(drivermonth) << L" " << driveryear << L" (";

	if (age_in_months <= 12)
	{
		ss << age_in_months << L" month" << plural(age_in_months) << L" old)";
	}
	else
	{
		ss << (age_in_months / 12) << L" year" << plural(age_in_months / 12) << L" and "
		<< (age_in_months % 12) << L" month" << plural(age_in_months % 12) << L" old)";
	}

	ss << L"\n\n";

	if (age_in_months <= 12)
	{
		ss << L"Your graphics driver is less than a year old, so it is likely it's up to date.  However, if you'd like to check for "
		L"an update anyway, click the link below.";
	}
	else if (age_in_months <= 24)
	{
		ss << L"Your graphics driver is less than two years old, so it is reasonably likely it's up to date.  However, it's worth checking "
			L"for an update anyway, in case a newer one is available.  Click the link below to check for an update.";
	}
	else
	{
		ss << L"It is highly recommended to check for an update.  Updating drivers can make your "
			L"computer run faster and crash less often.  Click the link below to check for an update.";

		m_ResultIcon.SetIcon(theApp.LoadIcon(IDI_WARN));
		m_ResultIcon.ModifyStyle(0, SS_REALSIZEIMAGE);
		m_ResultIcon.SetWindowPos(NULL, 0, 0, 16, 16, SWP_NOMOVE | SWP_NOZORDER);
	}

	ss << L"\n\n";

	std::wstring lower = ToLower(adaptername);

	if (lower.find(L"nvidia") != std::wstring::npos)
	{
		ss << L"<a>nVidia graphics driver update</a>";
	}
	else if (lower.find(L"intel") != std::wstring::npos)
	{
		ss << L"<a>Intel graphics driver update</a>";
	}
	// Note case sensitive check for ATI since it's a common character sequence
	else if (lower.find(L"amd") != std::wstring::npos || adaptername.find(L"ATI") != std::wstring::npos)
	{
		ss << L"<a>AMD or ATI graphics driver update</a>";
	}
	else
	{
		NoJoy();
		return TRUE;
	}

	std::wstring os_name = GetOSName();

	if (!os_name.empty())
		ss << L"\n\nYou may be asked to select your operating system.  You are using " << os_name << L".";

	m_Text.SetWindowText(ss.str().c_str());
	m_Text.SetItemState(0, 0, LIS_FOCUSED);
	m_Text.SetItemState(1, LIS_FOCUSED, LIS_FOCUSED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPage2::NoJoy()
{
	nojoy = true;

	m_ResultIcon.ShowWindow(SW_HIDE);

	std::wostringstream ss;
	ss << L"Sorry!  The Graphics Driver Updater was unable to automatically detect your graphics card.  "
		L"The following links may still help you:\n\n"
		L"Try checking for updates on <a>Windows Update</a>\n\n"
		L"Alternatively open <a>Device Manager</a> and look under 'Display Adapters'.  Click one of the following links "
		L"depending on the manufacturer of the graphics card:\n\n"
		L"<a>nVidia graphics driver update</a>\n\n"
		L"<a>AMD or ATI graphics driver update</a>\n\n"
		L"<a>Intel graphics driver update</a>\n\n"
		L"On the appropriate website, locate your graphics card as it appears in Device Manager.  There may also be an 'auto detect' option you can try.";

	std::wstring os_name = GetOSName();

	if (!os_name.empty())
		ss << L"  If you are asked to select your operating system, you are using " << os_name << L".";

	m_Text.SetWindowText(ss.str().c_str());
}

void CPage2::OnNMClickInfotext1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NMLINK* nmlink = (NMLINK*)pNMHDR;

	if (nojoy)
	{
		switch (nmlink->item.iLink) {
		case 0:		// Windows Update
			LaunchBrowserWithURL(L"http://windowsupdate.microsoft.com");
			break;
		case 1:		// Device Manager
			ShellExecute(NULL, L"open", L"devmgmt.msc", NULL, NULL, SW_SHOW);
			break;
		case 2:
			LaunchBrowserWithURL(NVIDIA_DRIVER_URL);
			break;
		case 3:
			LaunchBrowserWithURL(AMD_DRIVER_URL);
			break;
		case 4:
			LaunchBrowserWithURL(INTEL_DRIVER_URL);
			break;
		}
	}
	else
	{
		if (nmlink->item.iLink == 0)	// copy
		{
			HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, (adaptername.length() + 1) * sizeof(wchar_t));
			wcscpy_s(reinterpret_cast<wchar_t*>(GlobalLock(hMem)), adaptername.length() + 1, adaptername.c_str());
			GlobalUnlock(hMem);
			OpenClipboard();
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();

			std::wostringstream ss;
			ss << L"The name of your graphics card, \"" << adaptername << L"\", has been copied to the clipboard.  "
				L"You can now paste this in to other applications, e.g. a web search.";
			MessageBox(ss.str().c_str(), L"Graphics driver updater", MB_ICONINFORMATION | MB_OK);
		}
		else							// driver link
		{
			std::wstring lower = ToLower(adaptername);

			if (lower.find(L"nvidia") != std::wstring::npos)
			{
				LaunchBrowserWithURL(NVIDIA_DRIVER_URL);
			}
			else if (lower.find(L"intel") != std::wstring::npos)
			{
				LaunchBrowserWithURL(INTEL_DRIVER_URL);
			}
			// Note case sensitive check for ATI since it's a common character sequence
			else if (lower.find(L"amd") != std::wstring::npos || adaptername.find(L"ATI") != std::wstring::npos)
			{
				LaunchBrowserWithURL(AMD_DRIVER_URL);
			}
		}
	}
}
