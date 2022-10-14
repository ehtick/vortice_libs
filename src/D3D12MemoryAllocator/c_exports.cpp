#include "D3D12MemAlloc.h"


extern "C" __declspec(dllexport) HRESULT CreateAllocator(const D3D12MA::ALLOCATOR_DESC * pDesc, D3D12MA::Allocator * *ppAllocator)
{
    return D3D12MA::CreateAllocator(pDesc, ppAllocator);
}
