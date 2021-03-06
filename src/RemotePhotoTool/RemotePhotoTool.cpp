//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoTool.cpp RemotePhotoTool main function
//

// includes
#include "stdafx.h"
#include "App.hpp"

/// main function
#pragma warning(suppress: 28251)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   try
   {
      App::InitCrashReporter();

      App app(hInstance);
      return app.Run(lpstrCmdLine, nCmdShow);
   }
   catch (...)
   {
      // exception while running
      ATLTRACE(_T("Exception while running RemotePhotoTool\n"));
      return -1;
   }
}
