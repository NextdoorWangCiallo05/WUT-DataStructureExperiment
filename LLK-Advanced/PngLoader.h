#pragma once
#include <memory>
#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>

class CPngLoader
{
public:
    // 从 RC 资源加载 PNG（type 建议 L"PNG"）
    static std::unique_ptr<Gdiplus::Bitmap> LoadFromResource(HINSTANCE hInst, UINT resId, const wchar_t* resType)
    {
        if (!hInst || !resType) return nullptr;

        HRSRC hRes = ::FindResourceW(hInst, MAKEINTRESOURCEW(resId), resType);
        if (!hRes) return nullptr;

        DWORD size = ::SizeofResource(hInst, hRes);
        if (size == 0) return nullptr;

        HGLOBAL hResData = ::LoadResource(hInst, hRes);
        if (!hResData) return nullptr;

        void* pResData = ::LockResource(hResData);
        if (!pResData) return nullptr;

        // 拷贝到可写全局内存（CreateStreamOnHGlobal 需要）
        HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, size);
        if (!hBuffer) return nullptr;

        void* pBuffer = ::GlobalLock(hBuffer);
        if (!pBuffer)
        {
            ::GlobalFree(hBuffer);
            return nullptr;
        }

        memcpy(pBuffer, pResData, size);
        ::GlobalUnlock(hBuffer);

        IStream* pStream = nullptr;
        if (FAILED(::CreateStreamOnHGlobal(hBuffer, TRUE, &pStream)))
        {
            ::GlobalFree(hBuffer);
            return nullptr;
        }

        std::unique_ptr<Gdiplus::Bitmap> bmp(Gdiplus::Bitmap::FromStream(pStream));
        pStream->Release();

        if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok)
            return nullptr;

        return bmp;
    }
};