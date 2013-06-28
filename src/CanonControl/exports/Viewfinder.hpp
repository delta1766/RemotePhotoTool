//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file Viewfinder.hpp Canon control - Viewfinder
//
#pragma once

// includes
#include <vector>
#include <boost/function.hpp>

/// viewfinder class
class Viewfinder
{
public:
   /// dtor
   virtual ~Viewfinder() throw() {}

   /// callback function type to call when viewfinder image is available
   typedef boost::function<void (const std::vector<BYTE>& vecImage)> T_fnOnAvailViewfinderImage;

   /// sets (or resets) viewfinder callback
   virtual void SetAvailImageHandler(T_fnOnAvailViewfinderImage fnOnAvailViewfinderImage = T_fnOnAvailViewfinderImage()) = 0;
};