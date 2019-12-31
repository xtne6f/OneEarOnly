// 片耳モノラルにする
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <stddef.h>
#include <algorithm>
using std::min;
#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#define TVTEST_PLUGIN_VERSION TVTEST_PLUGIN_VERSION_(0,0,6)
#include "TVTestPlugin.h"

//#define IS_LEFT_ONLY

class COneEarOnly : public TVTest::CTVTestPlugin
{
public:
    COneEarOnly()
        : m_fEnabled(false)
    {
    }

    bool GetPluginInfo(TVTest::PluginInfo *pInfo)
    {
        pInfo->Type = TVTest::PLUGIN_TYPE_NORMAL;
        pInfo->Flags = 0;
        pInfo->pszCopyright = L"Public Domain";
#ifdef IS_LEFT_ONLY
        pInfo->pszPluginName = L"OneEarOnlyL";
        pInfo->pszDescription = L"左耳モノラルにする (ver.1.0)";
#else
        pInfo->pszPluginName = L"OneEarOnlyR";
        pInfo->pszDescription = L"右耳モノラルにする (ver.1.0)";
#endif
        return true;
    }

    bool Initialize()
    {
        m_pApp->SetEventCallback(EventCallback, this);
        return true;
    }

    bool Finalize()
    {
        DisablePlugin();
        return true;
    }

    bool EnablePlugin()
    {
        if (!m_fEnabled) {
            m_fEnabled = m_pApp->SetAudioCallback(AudioCallback);
        }
        return m_fEnabled;
    }

    bool DisablePlugin()
    {
        if (m_fEnabled) {
            m_pApp->SetAudioCallback(nullptr);
            m_fEnabled = false;
        }
        return true;
    }

    static LRESULT CALLBACK EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData)
    {
        static_cast<void>(lParam2);
        if (Event == TVTest::EVENT_PLUGINENABLE) {
            return lParam1 ? static_cast<COneEarOnly*>(pClientData)->EnablePlugin() :
                             static_cast<COneEarOnly*>(pClientData)->DisablePlugin();
        }
        return 0;
    }

private:
    static LRESULT CALLBACK AudioCallback(short *pData, DWORD Samples, int Channels, void *pClientData)
    {
        static_cast<void>(pClientData);
        if (Channels == 2) {
            for (DWORD i = 0; i < Samples; ++i) {
#ifdef IS_LEFT_ONLY
                pData[0] = static_cast<short>((pData[0] + pData[1]) / 2);
                pData[1] = 0;
#else
                pData[1] = static_cast<short>((pData[0] + pData[1]) / 2);
                pData[0] = 0;
#endif
                pData += 2;
            }
        }
        else if (Channels == 6) {
            for (DWORD i = 0; i < Samples; ++i) {
#ifdef IS_LEFT_ONLY
                pData[0] = static_cast<short>((pData[0] + pData[1]) / 2);
                pData[1] = 0;
                pData[2] /= 2;
                pData[3] /= 2;
                pData[4] = static_cast<short>((pData[4] + pData[5]) / 2);
                pData[5] = 0;
#else
                pData[1] = static_cast<short>((pData[0] + pData[1]) / 2);
                pData[0] = 0;
                pData[2] /= 2;
                pData[3] /= 2;
                pData[5] = static_cast<short>((pData[4] + pData[5]) / 2);
                pData[4] = 0;
#endif
                pData += 6;
            }
        }
        return 0;
    }

    bool m_fEnabled;
};

TVTest::CTVTestPlugin *CreatePluginClass()
{
    return new COneEarOnly;
}
