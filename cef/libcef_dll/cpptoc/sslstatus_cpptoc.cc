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
// $hash=c50dcf877d17e1228579f9e23fdc5b153924ce37$
//

#include "libcef_dll/cpptoc/sslstatus_cpptoc.h"
#include "libcef_dll/cpptoc/x509certificate_cpptoc.h"
#include "libcef_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int CEF_CALLBACK sslstatus_is_secure_connection(struct _cef_sslstatus_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval = CefSSLStatusCppToC::Get(self)->IsSecureConnection();

  // Return type: bool
  return _retval;
}

cef_cert_status_t CEF_CALLBACK
sslstatus_get_cert_status(struct _cef_sslstatus_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CERT_STATUS_NONE;

  // Execute
  cef_cert_status_t _retval = CefSSLStatusCppToC::Get(self)->GetCertStatus();

  // Return type: simple
  return _retval;
}

cef_ssl_version_t CEF_CALLBACK
sslstatus_get_sslversion(struct _cef_sslstatus_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return SSL_CONNECTION_VERSION_UNKNOWN;

  // Execute
  cef_ssl_version_t _retval = CefSSLStatusCppToC::Get(self)->GetSSLVersion();

  // Return type: simple
  return _retval;
}

cef_ssl_content_status_t CEF_CALLBACK
sslstatus_get_content_status(struct _cef_sslstatus_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return SSL_CONTENT_NORMAL_CONTENT;

  // Execute
  cef_ssl_content_status_t _retval =
      CefSSLStatusCppToC::Get(self)->GetContentStatus();

  // Return type: simple
  return _retval;
}

struct _cef_x509certificate_t* CEF_CALLBACK
sslstatus_get_x509certificate(struct _cef_sslstatus_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return NULL;

  // Execute
  CefRefPtr<CefX509Certificate> _retval =
      CefSSLStatusCppToC::Get(self)->GetX509Certificate();

  // Return type: refptr_same
  return CefX509CertificateCppToC::Wrap(_retval);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefSSLStatusCppToC::CefSSLStatusCppToC() {
  GetStruct()->is_secure_connection = sslstatus_is_secure_connection;
  GetStruct()->get_cert_status = sslstatus_get_cert_status;
  GetStruct()->get_sslversion = sslstatus_get_sslversion;
  GetStruct()->get_content_status = sslstatus_get_content_status;
  GetStruct()->get_x509certificate = sslstatus_get_x509certificate;
}

// DESTRUCTOR - Do not edit by hand.

CefSSLStatusCppToC::~CefSSLStatusCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefSSLStatus>
CefCppToCRefCounted<CefSSLStatusCppToC, CefSSLStatus, cef_sslstatus_t>::
    UnwrapDerived(CefWrapperType type, cef_sslstatus_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

template <>
CefWrapperType CefCppToCRefCounted<CefSSLStatusCppToC,
                                   CefSSLStatus,
                                   cef_sslstatus_t>::kWrapperType =
    WT_SSLSTATUS;
