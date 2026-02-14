//
// Created by Misha on 2/2/2026.
//

#pragma once

#include <wrl/client.h>

namespace dx
{
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
}
