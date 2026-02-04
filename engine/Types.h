//
// Created by Misha on 2/2/2026.
//

#ifndef DXGRAPHICSENGINE_TYPES_H
#define DXGRAPHICSENGINE_TYPES_H

#include <wrl/client.h>

namespace dx
{
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
}

#endif //DXGRAPHICSENGINE_TYPES_H