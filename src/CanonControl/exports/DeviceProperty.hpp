//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file DeviceProperty.hpp Canon control - Device property
//
#pragma once

// includes
#include "Variant.hpp"
#include "ImplFwd.hpp"

/// device property
class DeviceProperty
{
public:
   /// device property id
   unsigned int Id() const throw() { return m_uiPropertyId; }

   /// property name
   CString Name() const throw();

   /// property value as string
   CString AsString() const throw();

   /// value as variant
   Variant Value() const { return m_value; }

   /// returns if property is read-only
   bool IsReadOnly() const throw() { return m_bReadOnly; }

   /// returns valid values as list of variants
   const std::vector<Variant>& ValidValues() const { return m_vecValidValues; }

   /// converts a given value (e.g. from ValidValues()) to string
   CString ValueAsString(Variant value) const throw();

private:
   friend EDSDK::SourceDeviceImpl;
   friend CDSDK::SourceDeviceImpl;
   friend PSREC::SourceDeviceImpl;

   /// ctor
   DeviceProperty(T_enSDKVariant enSDKVariant, unsigned int uiPropertyId, Variant value, bool bReadOnly)
      :m_enSDKVariant(enSDKVariant),
       m_uiPropertyId(uiPropertyId),
       m_value(value),
       m_bReadOnly(bReadOnly)
   {
   }

   /// returns valid values as list of variants; non-const version
   std::vector<Variant>& ValidValues() { return m_vecValidValues; }

private:
   /// SDK variant
   T_enSDKVariant m_enSDKVariant;

   /// device property id
   unsigned int m_uiPropertyId;

   /// value
   Variant m_value;

   /// read only property?
   bool m_bReadOnly;

   /// valid values
   std::vector<Variant> m_vecValidValues;
};
