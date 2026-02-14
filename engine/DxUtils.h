//
// Created by Misha on 2/3/2026.
//

#pragma once

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <third_party/d3d12ma/D3D12MemAlloc.h>
#include <third_party/directx/d3d12.h>
#include <third_party/directx/d3dx12.h>

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include "engine/Types.h"

#ifdef _DEBUG
inline dx::ComPtr<ID3D12InfoQueue> _DebugInfoQueue = nullptr;


// Call this once right after device creation
inline void DX12DebugSetup(ID3D12Device* device)
{
    if (!device) return;

    if (!SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&_DebugInfoQueue)))) {
        std::cerr << "[DX12 ERROR] Failed to query ID3D12InfoQueue" << std::endl;
    }

    if (_DebugInfoQueue)
    {
        // if (!SUCCEEDED(_DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE))) {
        //     std::cerr << "[DX12 ERROR] SetBreakOnSeverity failed" << std::endl;
        // }
        // if (!SUCCEEDED(_DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE))) {
        //     std::cerr << "[DX12 ERROR] SetBreakOnSeverity failed" << std::endl;
        // }
        // if (!SUCCEEDED(_DebugInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE))) {
        //     std::cerr << "[DX12 ERROR] SetBreakOnSeverity failed" << std::endl;
        // }
    }
}

inline void DX12DebugPoll()
{
    if (!_DebugInfoQueue) return;

    const UINT64 numMessages = _DebugInfoQueue->GetNumStoredMessages();
    for (UINT64 i = 0; i < numMessages; ++i)
    {
        SIZE_T msgLen = 0;
        if (!SUCCEEDED(_DebugInfoQueue->GetMessage(i, nullptr, &msgLen))) {
            std::cerr << "[DX12 ERROR] GetMessage failed" << std::endl;
        };

        D3D12_MESSAGE* msg = static_cast<D3D12_MESSAGE *>(malloc(msgLen));
        if (!SUCCEEDED(_DebugInfoQueue->GetMessage(i, msg, &msgLen))) {
            std::cerr << "[DX12 ERROR] GetMessage failed" << std::endl;
        };

        std::cerr << "[DX12 DEBUG] " << msg->pDescription << std::endl;

        free(msg);
    }

    _DebugInfoQueue->ClearStoredMessages();
}
#endif

#ifndef _DEBUG
inline void DX12DebugPoll() {};
#endif




#define DX_CHECK(hr) do { \
HRESULT _hrTmp = (hr); \
if (FAILED(_hrTmp)) { \
DX12DebugPoll(); \
char* errMsg = nullptr; \
FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
nullptr, _hrTmp, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
(LPSTR)&errMsg, 0, nullptr); \
std::ostringstream oss; \
oss << "[DX12 ERROR] HRESULT: 0x" << std::hex << _hrTmp; \
if (errMsg) { \
oss << " (" << errMsg << ")"; \
LocalFree(errMsg); \
} \
std::cerr << oss.str() << std::endl; \
throw std::runtime_error(oss.str()); \
} \
} while(0)
