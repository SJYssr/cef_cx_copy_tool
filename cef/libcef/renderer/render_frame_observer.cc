// Copyright 2014 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "base/compiler_specific.h"

// Enable deprecation warnings on Windows. See http://crbug.com/585142.
#if defined(OS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wdeprecated-declarations"
#else
#pragma warning(push)
#pragma warning(default : 4996)
#endif
#endif

#include "libcef/renderer/render_frame_observer.h"

#include "libcef/common/content_client.h"
#include "libcef/renderer/blink_glue.h"
#include "libcef/renderer/browser_impl.h"
#include "libcef/renderer/dom_document_impl.h"
#include "libcef/renderer/v8_impl.h"

#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_node.h"

CefRenderFrameObserver::CefRenderFrameObserver(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {}

CefRenderFrameObserver::~CefRenderFrameObserver() {}

void CefRenderFrameObserver::OnInterfaceRequestForFrame(
    const std::string& interface_name,
    mojo::ScopedMessagePipeHandle* interface_pipe) {
  registry_.TryBindInterface(interface_name, interface_pipe);
}

void CefRenderFrameObserver::DidFinishLoad() {
  if (frame_) {
    frame_->OnDidFinishLoad();
  }
}

void CefRenderFrameObserver::FrameDetached() {
  if (frame_) {
    frame_->OnDetached();
    frame_ = nullptr;
  }
}

void CefRenderFrameObserver::FrameFocused() {
  if (frame_) {
    frame_->OnFocused();
  }
}

void CefRenderFrameObserver::FocusedNodeChanged(const blink::WebNode& node) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (!browserPtr)
    return;

  CefRefPtr<CefRenderProcessHandler> handler;
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application)
    handler = application->GetRenderProcessHandler();
  if (!handler)
    return;

  CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  if (node.IsNull()) {
    handler->OnFocusedNodeChanged(browserPtr.get(), framePtr.get(), nullptr);
    return;
  }

  if (node.GetDocument().IsNull())
    return;

  CefRefPtr<CefDOMDocumentImpl> documentImpl =
      new CefDOMDocumentImpl(browserPtr.get(), frame);
  handler->OnFocusedNodeChanged(browserPtr.get(), framePtr.get(),
                                documentImpl->GetOrCreateNode(node));
  documentImpl->Detach();
}

void CefRenderFrameObserver::DraggableRegionsChanged() {
  if (frame_) {
    frame_->OnDraggableRegionsChanged();
  }
}

void CefRenderFrameObserver::DidCreateScriptContext(
    v8::Handle<v8::Context> context,
    int world_id) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (!browserPtr)
    return;

  CefRefPtr<CefRenderProcessHandler> handler;
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application)
    handler = application->GetRenderProcessHandler();
  if (!handler)
    return;

  CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  v8::Isolate* isolate = blink::MainThreadIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope scope(context);
  v8::MicrotasksScope microtasks_scope(isolate,
                                       v8::MicrotasksScope::kRunMicrotasks);

  CefRefPtr<CefV8Context> contextPtr(new CefV8ContextImpl(isolate, context));

  handler->OnContextCreated(browserPtr.get(), framePtr.get(), contextPtr);
}

void CefRenderFrameObserver::WillReleaseScriptContext(
    v8::Handle<v8::Context> context,
    int world_id) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(frame->Top());
  if (browserPtr) {
    CefRefPtr<CefApp> application = CefContentClient::Get()->application();
    if (application) {
      CefRefPtr<CefRenderProcessHandler> handler =
          application->GetRenderProcessHandler();
      if (handler) {
        CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

        v8::Isolate* isolate = blink::MainThreadIsolate();
        v8::HandleScope handle_scope(isolate);

        // The released context should not be used for script execution.
        // Depending on how the context is released this may or may not already
        // be set.
        blink_glue::CefScriptForbiddenScope forbidScript;

        CefRefPtr<CefV8Context> contextPtr(
            new CefV8ContextImpl(isolate, context));

        handler->OnContextReleased(browserPtr.get(), framePtr.get(),
                                   contextPtr);
      }
    }
  }

  CefV8ReleaseContext(context);
}

void CefRenderFrameObserver::OnDestruct() {
  delete this;
}

bool CefRenderFrameObserver::OnMessageReceived(const IPC::Message& message) {
  if (frame_) {
    return frame_->OnMessageReceived(message);
  }
  return false;
}

void CefRenderFrameObserver::AttachFrame(CefFrameImpl* frame) {
  DCHECK(frame);
  DCHECK(!frame_);
  frame_ = frame;
  frame_->OnAttached();
}

// Enable deprecation warnings on Windows. See http://crbug.com/585142.
#if defined(OS_WIN)
#if defined(__clang__)
#pragma GCC diagnostic pop
#else
#pragma warning(pop)
#endif
#endif
