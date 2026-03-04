#include "FileExplorer.h"
#include <shobjidl.h>
#include <codecvt>

namespace sLink::utility
{
    FileExplorer::FileExplorer() : m_IsOpen(false)
    {
    }

    void FileExplorer::open()
    { {
            std::scoped_lock lock(m_IsOpenMutex);

            if (m_IsOpen)
                return;

            m_IsOpen = true;
        }

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        if (SUCCEEDED(hr))
        {
            IFileOpenDialog *pFileOpen;

            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void **>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
                hr = pFileOpen->Show(NULL);

                m_IsOpen = true;

                if (SUCCEEDED(hr))
                {
                    IShellItem *pItem;
                    hr = pFileOpen->GetResult(&pItem);

                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        if (SUCCEEDED(hr))
                        {
                            int size_needed = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);

                            std::string path(size_needed - 1, 0);

                            WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &path[0], size_needed, NULL, NULL);

                            m_Paths.push(std::move(path));

                            CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }

        m_IsOpen = false;
    }

    SafeQueue<std::string> &FileExplorer::getPaths()
    {
        return m_Paths;
    }
}
