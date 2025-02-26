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
// $hash=1a7718d07067ca673cee1afa55fba8687d91e2f7$
//

#include "libcef_dll/ctocpp/completion_callback_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") void CefCompletionCallbackCToCpp::OnComplete() {
  shutdown_checker::AssertNotShutdown();

  cef_completion_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_complete))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->on_complete(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

CefCompletionCallbackCToCpp::CefCompletionCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefCompletionCallbackCToCpp::~CefCompletionCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_completion_callback_t* CefCToCppRefCounted<
    CefCompletionCallbackCToCpp,
    CefCompletionCallback,
    cef_completion_callback_t>::UnwrapDerived(CefWrapperType type,
                                              CefCompletionCallback* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

template <>
CefWrapperType CefCToCppRefCounted<CefCompletionCallbackCToCpp,
                                   CefCompletionCallback,
                                   cef_completion_callback_t>::kWrapperType =
    WT_COMPLETION_CALLBACK;
