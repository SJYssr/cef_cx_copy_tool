// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=7ed195825a3d893f05a848e9d87ff86637908f90$
//

#ifndef CEF_LIBCEF_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_
#define CEF_LIBCEF_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_
#pragma once

#if !defined(BUILDING_CEF_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/views/cef_button_capi.h"
#include "include/capi/views/cef_label_button_capi.h"
#include "include/views/cef_button.h"
#include "include/views/cef_label_button.h"
#include "libcef_dll/cpptoc/cpptoc_ref_counted.h"

// Wrap a C++ class with a C structure.
// This class may be instantiated and accessed DLL-side only.
class CefButtonCppToC
    : public CefCppToCRefCounted<CefButtonCppToC, CefButton, cef_button_t> {
 public:
  CefButtonCppToC();
  virtual ~CefButtonCppToC();
};

#endif  // CEF_LIBCEF_DLL_CPPTOC_VIEWS_BUTTON_CPPTOC_H_
