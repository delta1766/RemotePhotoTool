//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AppSettings.cpp Settings
//

// includes
#include "stdafx.h"
#include "AppSettings.hpp"
#include "HuginInterface.hpp"
#include "PhotomatixInterface.hpp"

LPCTSTR g_pszProjectsFolder =       _T("ProjectsFolder");
LPCTSTR g_pszCurrentDateSubfolder = _T("CurrentDateSubfolder");
LPCTSTR g_pszImageTypeSubfolder =   _T("ImageTypeSubfolder");
LPCTSTR g_pszPhotomatixPath =       _T("PhotomatixPath");
LPCTSTR g_pszHuginPath =            _T("HuginPath");
LPCTSTR g_pszLogging =              _T("Logging");
LPCTSTR g_pszLogfilePath =          _T("LogfilePath");

/// reads string value from registry
void ReadStringValue(CRegKey& regKey, LPCTSTR pszName, UINT uiMaxLength, CString& cszValue)
{
   ULONG ulLen = uiMaxLength;
   CString cszTemp;
   LONG lRet = regKey.QueryStringValue(pszName, cszTemp.GetBuffer(ulLen), &ulLen);
   cszTemp.ReleaseBuffer();

   if (lRet == ERROR_SUCCESS)
      cszValue = cszTemp;
}

/// reads boolean value from registry
void ReadBooleanValue(CRegKey& regKey, LPCTSTR pszName, bool& bValue)
{
   DWORD value;
   if (ERROR_SUCCESS == regKey.QueryDWORDValue(pszName, value))
      bValue = value==1;
}

AppSettings::AppSettings(LPCTSTR pszSettingsRegkey)
:m_pszSettingsRegkey(pszSettingsRegkey),
 m_bCurrentDateSubfolder(true),
 m_bImageTypeSubfolder(true),
 m_bLogging(false)
{
   // get "my pictures" special folder
   SHGetFolderPath(NULL,
      CSIDL_MYPICTURES,
      NULL,
      SHGFP_TYPE_CURRENT,
      m_cszProjectsFolder.GetBuffer(MAX_PATH));
   m_cszProjectsFolder.ReleaseBuffer();

   {
      PhotomatixInterface pi(_T(""));
      if (pi.IsInstalled())
         m_cszPhotomatixPath = pi.InstallPath();
   }

   {
      HuginInterface hi(_T(""));
      if (hi.IsInstalled())
         m_cszHuginPath = hi.InstallPath();
   }
}

void AppSettings::Store()
{
   // open root key
   CRegKey regRoot;
   if (ERROR_SUCCESS != regRoot.Open(HKEY_CURRENT_USER, m_pszSettingsRegkey))
   {
      // try to create key
      if (ERROR_SUCCESS != regRoot.Create(HKEY_CURRENT_USER, m_pszSettingsRegkey))
         return;
   }

   regRoot.SetStringValue(g_pszProjectsFolder, m_cszProjectsFolder);
   regRoot.SetDWORDValue(g_pszCurrentDateSubfolder, m_bCurrentDateSubfolder ? 1 : 0);
   regRoot.SetDWORDValue(g_pszImageTypeSubfolder, m_bImageTypeSubfolder ? 1 : 0);
   regRoot.SetStringValue(g_pszPhotomatixPath, m_cszPhotomatixPath);
   regRoot.SetStringValue(g_pszHuginPath, m_cszHuginPath);
   regRoot.SetDWORDValue(g_pszLogging, m_bLogging ? 1 : 0);
   regRoot.SetStringValue(g_pszLogfilePath, m_cszLogfilePath);
}

void AppSettings::Load()
{
   // open root key
   CRegKey regRoot;
   if (ERROR_SUCCESS != regRoot.Open(HKEY_CURRENT_USER, m_pszSettingsRegkey, KEY_READ))
      return;

   ReadStringValue(regRoot, g_pszProjectsFolder, MAX_PATH, m_cszProjectsFolder);
   ReadBooleanValue(regRoot, g_pszCurrentDateSubfolder, m_bCurrentDateSubfolder);
   ReadBooleanValue(regRoot, g_pszImageTypeSubfolder, m_bImageTypeSubfolder);
   ReadStringValue(regRoot, g_pszPhotomatixPath, MAX_PATH, m_cszPhotomatixPath);
   ReadStringValue(regRoot, g_pszHuginPath, MAX_PATH, m_cszHuginPath);
   ReadBooleanValue(regRoot, g_pszLogging, m_bLogging);
   ReadStringValue(regRoot, g_pszLogfilePath, MAX_PATH, m_cszLogfilePath);
}
