// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/browser_impl.h"

#include <string>
#include <vector>

#include "libcef/common/cef_messages.h"
#include "libcef/common/content_client.h"
#include "libcef/renderer/blink_glue.h"
#include "libcef/renderer/content_renderer_client.h"
#include "libcef/renderer/render_frame_util.h"
#include "libcef/renderer/thread_util.h"

#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/renderer/document_state.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/navigation_state.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_frame_content_dumper.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_security_policy.h"
#include "third_party/blink/public/web/web_view.h"

// CefBrowserImpl static methods.
// -----------------------------------------------------------------------------

// static
CefRefPtr<CefBrowserImpl> CefBrowserImpl::GetBrowserForView(
    content::RenderView* view) {
  return CefContentRendererClient::Get()->GetBrowserForView(view);
}

// static
CefRefPtr<CefBrowserImpl> CefBrowserImpl::GetBrowserForMainFrame(
    blink::WebFrame* frame) {
  return CefContentRendererClient::Get()->GetBrowserForMainFrame(frame);
}

// CefBrowser methods.
// -----------------------------------------------------------------------------

CefRefPtr<CefBrowserHost> CefBrowserImpl::GetHost() {
  NOTREACHED() << "GetHost cannot be called from the render process";
  return nullptr;
}

bool CefBrowserImpl::CanGoBack() {
  CEF_REQUIRE_RT_RETURN(false);

  return blink_glue::CanGoBack(render_view()->GetWebView());
}

void CefBrowserImpl::GoBack() {
  CEF_REQUIRE_RT_RETURN_VOID();

  blink_glue::GoBack(render_view()->GetWebView());
}

bool CefBrowserImpl::CanGoForward() {
  CEF_REQUIRE_RT_RETURN(false);

  return blink_glue::CanGoForward(render_view()->GetWebView());
}

void CefBrowserImpl::GoForward() {
  CEF_REQUIRE_RT_RETURN_VOID();

  blink_glue::GoForward(render_view()->GetWebView());
}

bool CefBrowserImpl::IsLoading() {
  CEF_REQUIRE_RT_RETURN(false);

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame)
      return main_frame->ToWebLocalFrame()->IsLoading();
  }
  return false;
}

void CefBrowserImpl::Reload() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StartReload(
          blink::WebFrameLoadType::kReload);
    }
  }
}

void CefBrowserImpl::ReloadIgnoreCache() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StartReload(
          blink::WebFrameLoadType::kReloadBypassingCache);
    }
  }
}

void CefBrowserImpl::StopLoad() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      main_frame->ToWebLocalFrame()->StopLoading();
    }
  }
}

int CefBrowserImpl::GetIdentifier() {
  CEF_REQUIRE_RT_RETURN(0);

  return browser_id();
}

bool CefBrowserImpl::IsSame(CefRefPtr<CefBrowser> that) {
  CEF_REQUIRE_RT_RETURN(false);

  CefBrowserImpl* impl = static_cast<CefBrowserImpl*>(that.get());
  return (impl == this);
}

bool CefBrowserImpl::IsPopup() {
  CEF_REQUIRE_RT_RETURN(false);

  return is_popup();
}

bool CefBrowserImpl::HasDocument() {
  CEF_REQUIRE_RT_RETURN(false);

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      return !main_frame->ToWebLocalFrame()->GetDocument().IsNull();
    }
  }
  return false;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetMainFrame() {
  CEF_REQUIRE_RT_RETURN(nullptr);

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
    if (main_frame && main_frame->IsWebLocalFrame()) {
      return GetWebFrameImpl(main_frame->ToWebLocalFrame()).get();
    }
  }
  return nullptr;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFocusedFrame() {
  CEF_REQUIRE_RT_RETURN(nullptr);

  if (render_view()->GetWebView() &&
      render_view()->GetWebView()->FocusedFrame()) {
    return GetWebFrameImpl(render_view()->GetWebView()->FocusedFrame()).get();
  }
  return nullptr;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFrame(int64 identifier) {
  CEF_REQUIRE_RT_RETURN(nullptr);

  return GetWebFrameImpl(identifier).get();
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFrame(const CefString& name) {
  CEF_REQUIRE_RT_RETURN(nullptr);

  blink::WebView* web_view = render_view()->GetWebView();
  if (web_view) {
    const blink::WebString& frame_name =
        blink::WebString::FromUTF16(name.ToString16());
    // Search by assigned frame name (Frame::name).
    blink::WebFrame* frame = web_view->MainFrame();
    if (frame && frame->IsWebLocalFrame())
      frame = frame->ToWebLocalFrame()->FindFrameByName(frame_name);
    if (!frame) {
      // Search by unique frame name (Frame::uniqueName).
      const std::string& searchname = name;
      for (blink::WebFrame* cur_frame = web_view->MainFrame(); cur_frame;
           cur_frame = cur_frame->TraverseNext()) {
        if (cur_frame->IsWebLocalFrame() &&
            render_frame_util::GetName(cur_frame->ToWebLocalFrame()) ==
                searchname) {
          frame = cur_frame;
          break;
        }
      }
    }
    if (frame && frame->IsWebLocalFrame())
      return GetWebFrameImpl(frame->ToWebLocalFrame()).get();
  }

  return nullptr;
}

size_t CefBrowserImpl::GetFrameCount() {
  CEF_REQUIRE_RT_RETURN(0);

  int count = 0;

  if (render_view()->GetWebView()) {
    for (blink::WebFrame* frame = render_view()->GetWebView()->MainFrame();
         frame; frame = frame->TraverseNext()) {
      count++;
    }
  }

  return count;
}

void CefBrowserImpl::GetFrameIdentifiers(std::vector<int64>& identifiers) {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (identifiers.size() > 0)
    identifiers.clear();

  if (render_view()->GetWebView()) {
    for (blink::WebFrame* frame = render_view()->GetWebView()->MainFrame();
         frame; frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame())
        identifiers.push_back(
            render_frame_util::GetIdentifier(frame->ToWebLocalFrame()));
    }
  }
}

void CefBrowserImpl::GetFrameNames(std::vector<CefString>& names) {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (names.size() > 0)
    names.clear();

  if (render_view()->GetWebView()) {
    for (blink::WebFrame* frame = render_view()->GetWebView()->MainFrame();
         frame; frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame())
        names.push_back(render_frame_util::GetName(frame->ToWebLocalFrame()));
    }
  }
}

// CefBrowserImpl public methods.
// -----------------------------------------------------------------------------

CefBrowserImpl::CefBrowserImpl(content::RenderView* render_view,
                               int browser_id,
                               bool is_popup,
                               bool is_windowless)
    : content::RenderViewObserver(render_view),
      browser_id_(browser_id),
      is_popup_(is_popup),
      is_windowless_(is_windowless) {}

CefBrowserImpl::~CefBrowserImpl() {}

CefRefPtr<CefFrameImpl> CefBrowserImpl::GetWebFrameImpl(
    blink::WebLocalFrame* frame) {
  DCHECK(frame);
  int64_t frame_id = render_frame_util::GetIdentifier(frame);

  // Frames are re-used between page loads. Only add the frame to the map once.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end())
    return it->second;

  CefRefPtr<CefFrameImpl> framePtr(new CefFrameImpl(this, frame, frame_id));
  frames_.insert(std::make_pair(frame_id, framePtr));

  return framePtr;
}

CefRefPtr<CefFrameImpl> CefBrowserImpl::GetWebFrameImpl(int64_t frame_id) {
  if (frame_id == blink_glue::kInvalidFrameId) {
    if (render_view()->GetWebView()) {
      blink::WebFrame* main_frame = render_view()->GetWebView()->MainFrame();
      if (main_frame && main_frame->IsWebLocalFrame()) {
        return GetWebFrameImpl(main_frame->ToWebLocalFrame());
      }
    }
    return nullptr;
  }

  // Check if we already know about the frame.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end())
    return it->second;

  if (render_view()->GetWebView()) {
    // Check if the frame exists but we don't know about it yet.
    for (blink::WebFrame* frame = render_view()->GetWebView()->MainFrame();
         frame; frame = frame->TraverseNext()) {
      if (frame->IsWebLocalFrame() &&
          render_frame_util::GetIdentifier(frame->ToWebLocalFrame()) ==
              frame_id) {
        return GetWebFrameImpl(frame->ToWebLocalFrame());
      }
    }
  }

  return nullptr;
}

void CefBrowserImpl::AddFrameObject(int64_t frame_id,
                                    CefTrackNode* tracked_object) {
  CefRefPtr<CefTrackManager> manager;

  if (!frame_objects_.empty()) {
    FrameObjectMap::const_iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end())
      manager = it->second;
  }

  if (!manager.get()) {
    manager = new CefTrackManager();
    frame_objects_.insert(std::make_pair(frame_id, manager));
  }

  manager->Add(tracked_object);
}

// RenderViewObserver methods.
// -----------------------------------------------------------------------------

void CefBrowserImpl::OnDestruct() {
  // Notify that the browser window has been destroyed.
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get())
      handler->OnBrowserDestroyed(this);
  }

  CefContentRendererClient::Get()->OnBrowserDestroyed(this);
}

void CefBrowserImpl::DidFailProvisionalLoad(blink::WebLocalFrame* frame,
                                            const blink::WebURLError& error) {
  OnLoadError(frame, error);
}

void CefBrowserImpl::DidCommitProvisionalLoad(blink::WebLocalFrame* frame,
                                              bool is_new_navigation) {
  if (frame->Parent() == nullptr) {
    OnLoadingStateChange(true);
  }
  OnLoadStart(frame);
}

void CefBrowserImpl::FrameDetached(int64_t frame_id) {
  if (!frames_.empty()) {
    // Remove the frame from the map.
    FrameMap::iterator it = frames_.find(frame_id);
    if (it != frames_.end()) {
      frames_.erase(it);
    }
  }

  if (!frame_objects_.empty()) {
    // Remove any tracked objects associated with the frame.
    FrameObjectMap::iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end())
      frame_objects_.erase(it);
  }
}

void CefBrowserImpl::OnLoadingStateChange(bool isLoading) {
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        blink::WebView* web_view = render_view()->GetWebView();
        const bool canGoBack = blink_glue::CanGoBack(web_view);
        const bool canGoForward = blink_glue::CanGoForward(web_view);

        // Don't call OnLoadingStateChange multiple times with the same status.
        // This can occur in cases where there are multiple highest-level
        // LocalFrames in-process for the same browser.
        if (last_loading_state_ &&
            last_loading_state_->IsMatch(isLoading, canGoBack, canGoForward)) {
          return;
        }

        load_handler->OnLoadingStateChange(this, isLoading, canGoBack,
                                           canGoForward);
        last_loading_state_.reset(
            new LoadingState(isLoading, canGoBack, canGoForward));
      }
    }
  }
}

void CefBrowserImpl::OnLoadStart(blink::WebLocalFrame* frame) {
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        CefRefPtr<CefFrameImpl> cef_frame = GetWebFrameImpl(frame);
        load_handler->OnLoadStart(this, cef_frame.get(), TT_EXPLICIT);
      }
    }
  }
}

void CefBrowserImpl::OnLoadError(blink::WebLocalFrame* frame,
                                 const blink::WebURLError& error) {
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler = app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        CefRefPtr<CefFrameImpl> cef_frame = GetWebFrameImpl(frame);
        const cef_errorcode_t errorCode =
            static_cast<cef_errorcode_t>(error.reason());
        const std::string& errorText = net::ErrorToString(error.reason());
        const GURL& failedUrl = error.url();
        load_handler->OnLoadError(this, cef_frame.get(), errorCode, errorText,
                                  failedUrl.spec());
      }
    }
  }
}
