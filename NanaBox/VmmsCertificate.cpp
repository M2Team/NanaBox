/*
 * PROJECT:    NanaBox
 * FILE:       VmmsCertificate.cpp
 * PURPOSE:    Implementation for the VMMS self-signed certificate management
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "VmmsCertificate.h"

#include <Mile.Helpers.CppBase.h>

#include <AclAPI.h>
#include <sddl.h>
#include <ShlObj.h>
#include <wincrypt.h>

#pragma comment(lib, "Crypt32.lib")

namespace
{
    static std::string VmmsOid = "1.3.6.1.4.1.311.62.1.1.1";
    static std::wstring VirtualMachineSidString = L"S-1-5-83-0";
    static BYTE VmmsExtensionValue[] = { 0x02, 0x01, 0x04 };

    static std::wstring GetComputerNameAsString()
    {
        static std::wstring const CachedResult = ([]() -> std::wstring
        {
            std::wstring Result(MAX_COMPUTERNAME_LENGTH, L'\0');

            DWORD Length = static_cast<DWORD>(Result.size() + 1);
            if (::GetComputerNameW(Result.data(), &Length))
            {
                Result.resize(Length);
            }
            else
            {
                Result.clear();
            }

            return Result;
        }());

        return CachedResult;
    }

    static std::wstring SHGetKnownFolderPathAsString(
        _In_ REFKNOWNFOLDERID rfid,
        _In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
        _In_opt_ HANDLE hToken)
    {
        std::wstring Path;

        LPWSTR RawPath = nullptr;
        if (SUCCEEDED(::SHGetKnownFolderPath(rfid, dwFlags, hToken, &RawPath)))
        {
            Path = std::wstring(RawPath);
        }
        ::CoTaskMemFree(RawPath);

        return Path;
    }

    static std::wstring GetProgramDataFolderPath()
    {
        static std::wstring const CachedResult = ::SHGetKnownFolderPathAsString(
            FOLDERID_ProgramData,
            KF_FLAG_DEFAULT,
            nullptr);
        return CachedResult;
    }

    static bool IsVmmsCertificateAvailable()
    {
        bool Result = false;

        std::wstring ComputerName = ::GetComputerNameAsString();
        if (!ComputerName.empty())
        {
            HCERTSTORE StoreHandle = ::CertOpenStore(
                CERT_STORE_PROV_SYSTEM_W,
                0,
                0,
                CERT_SYSTEM_STORE_LOCAL_MACHINE |
                CERT_STORE_OPEN_EXISTING_FLAG |
                CERT_STORE_READONLY_FLAG,
                L"MY");
            if (StoreHandle)
            {
                PCCERT_CONTEXT Context = nullptr;
                do
                {
                    Context = ::CertFindCertificateInStore(
                        StoreHandle,
                        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                        0,
                        CERT_FIND_SUBJECT_STR_W,
                        ComputerName.c_str(),
                        Context);
                    if (Context)
                    {
                        if (::CertFindExtension(
                            VmmsOid.c_str(),
                            Context->pCertInfo->cExtension,
                            Context->pCertInfo->rgExtension))
                        {
                            if (!::CertVerifyTimeValidity(
                                nullptr,
                                Context->pCertInfo))
                            {
                                Result = true;
                            }
                        }
                    }
                } while (Context && !Result);

                if (Context)
                {
                    ::CertFreeCertificateContext(Context);
                }

                ::CertCloseStore(StoreHandle, 0);
            }
        }

        return Result;
    }

    static std::wstring GetUniqueContainerName(
        _In_ HCRYPTPROV ProviderHandle)
    {
        std::wstring Result;

        DWORD Length = 0;
        if (::CryptGetProvParam(
            ProviderHandle,
            PP_UNIQUE_CONTAINER,
            nullptr,
            &Length,
            0))
        {
            if (Length)
            {
                std::string RawResult(Length, '\0');
                if (::CryptGetProvParam(
                    ProviderHandle,
                    PP_UNIQUE_CONTAINER,
                    reinterpret_cast<BYTE*>(RawResult.data()),
                    &Length,
                    0))
                {
                    RawResult.resize(Length);
                    if (!RawResult.empty() && '\0' == RawResult.back())
                    {
                        RawResult.pop_back();
                    }
                    Result = Mile::ToWideString(CP_ACP, RawResult);
                }
            }
        }

        return Result;
    }

    static bool GrantVmmsPrivateKeyReadAccess(
        _In_ HCRYPTPROV ProviderHandle)
    {
        std::wstring PrivateKeyPath;
        {
            std::wstring BasePath = ::GetProgramDataFolderPath();
            if (BasePath.empty())
            {
                return false;
            }
            PrivateKeyPath.append(BasePath);
            PrivateKeyPath.append(L"\\Microsoft\\Crypto\\RSA\\MachineKeys\\");

            std::wstring KeyName = ::GetUniqueContainerName(ProviderHandle);
            if (KeyName.empty())
            {
                return false;
            }
            PrivateKeyPath.append(KeyName);
        }

        bool Result = false;

        PSID VirtualMachinesSid = nullptr;
        if (::ConvertStringSidToSidW(
            VirtualMachineSidString.c_str(),
            &VirtualMachinesSid))
        {
            PACL ExistingAccessControlList = nullptr;
            PSECURITY_DESCRIPTOR SecurityDescriptor = nullptr;
            DWORD ErrorCode = ::GetNamedSecurityInfoW(
                PrivateKeyPath.data(),
                SE_FILE_OBJECT,
                DACL_SECURITY_INFORMATION,
                nullptr,
                nullptr,
                &ExistingAccessControlList,
                nullptr,
                &SecurityDescriptor);
            if (ERROR_SUCCESS == ErrorCode)
            {
                if (ExistingAccessControlList)
                {
                    EXPLICIT_ACCESSW ExplicitAccess = {};
                    ExplicitAccess.grfAccessPermissions = FILE_GENERIC_READ;
                    ExplicitAccess.grfAccessMode = GRANT_ACCESS;
                    ExplicitAccess.grfInheritance = NO_INHERITANCE;
                    ::BuildTrusteeWithSidW(
                        &ExplicitAccess.Trustee,
                        VirtualMachinesSid);

                    PACL UpdatedAccessControlList = nullptr;
                    ErrorCode = ::SetEntriesInAclW(
                        1,
                        &ExplicitAccess,
                        ExistingAccessControlList,
                        &UpdatedAccessControlList);
                    if (ERROR_SUCCESS == ErrorCode)
                    {
                        ErrorCode = ::SetNamedSecurityInfoW(
                            PrivateKeyPath.data(),
                            SE_FILE_OBJECT,
                            DACL_SECURITY_INFORMATION,
                            nullptr,
                            nullptr,
                            UpdatedAccessControlList,
                            nullptr);
                        if (ERROR_SUCCESS == ErrorCode)
                        {
                            Result = true;
                        }

                        ::LocalFree(UpdatedAccessControlList);
                    }
                }
                else
                {
                    Result = true;
                }

                ::LocalFree(SecurityDescriptor);
            }

            ::LocalFree(VirtualMachinesSid);
        }

        return Result;
    }

    static std::vector<BYTE> CryptEncodeObjectAsVector(
        _In_ DWORD dwCertEncodingType,
        _In_ LPCSTR lpszStructType,
        _In_ void const* pvStructInfo)
    {
        std::vector<BYTE> Result;

        DWORD Length = 0;
        if (::CryptEncodeObject(
            dwCertEncodingType,
            lpszStructType,
            pvStructInfo,
            nullptr,
            &Length))
        {
            if (Length)
            {
                Result.resize(Length);
                if (!::CryptEncodeObject(
                    dwCertEncodingType,
                    lpszStructType,
                    pvStructInfo,
                    Result.data(),
                    &Length))
                {
                    Result.clear();
                }
                else
                {
                    Result.resize(Length);
                }
            }
        }

        return Result;
    }

    static bool CreateVmmsCertificate()
    {
        std::wstring ComputerName = ::GetComputerNameAsString();
        if (ComputerName.empty())
        {
            return false;
        }

        std::wstring SubjectName = L"CN=" + ComputerName;

        std::vector<BYTE> EncodedSubject;
        {
            DWORD Length = 0;
            if (::CertStrToNameW(
                X509_ASN_ENCODING,
                SubjectName.c_str(),
                CERT_X500_NAME_STR,
                nullptr,
                nullptr,
                &Length,
                nullptr))
            {
                if (Length)
                {
                    EncodedSubject.resize(Length);
                    if (!::CertStrToNameW(
                        X509_ASN_ENCODING,
                        SubjectName.c_str(),
                        CERT_X500_NAME_STR,
                        nullptr,
                        EncodedSubject.data(),
                        &Length,
                        nullptr))
                    {
                        EncodedSubject.clear();
                    }
                    else
                    {
                        EncodedSubject.resize(Length);
                    }
                }
            }
        }
        if (EncodedSubject.empty())
        {
            return false;
        }

        CERT_NAME_BLOB Subject = {};
        Subject.cbData = static_cast<DWORD>(EncodedSubject.size());
        Subject.pbData = EncodedSubject.data();

        LPSTR EnhancedKeyUsageIdentifiers[] =
        {
            const_cast<LPSTR>(szOID_PKIX_KP_SERVER_AUTH)
        };

        CERT_ENHKEY_USAGE EnhancedKeyUsage = {};
        EnhancedKeyUsage.cUsageIdentifier =
            ARRAYSIZE(EnhancedKeyUsageIdentifiers);
        EnhancedKeyUsage.rgpszUsageIdentifier =
            EnhancedKeyUsageIdentifiers;

        std::vector<BYTE> EncodedEnhancedKeyUsage = ::CryptEncodeObjectAsVector(
            X509_ASN_ENCODING,
            X509_ENHANCED_KEY_USAGE,
            &EnhancedKeyUsage);
        if (EncodedEnhancedKeyUsage.empty())
        {
            return false;
        }

        BYTE KeyUsageBits = CERT_KEY_ENCIPHERMENT_KEY_USAGE;
        KeyUsageBits |= CERT_DATA_ENCIPHERMENT_KEY_USAGE;

        CRYPT_BIT_BLOB KeyUsage = {};
        KeyUsage.cbData = sizeof(KeyUsageBits);
        KeyUsage.pbData = &KeyUsageBits;
        KeyUsage.cUnusedBits = 4;

        std::vector<BYTE> EncodedKeyUsage = ::CryptEncodeObjectAsVector(
            X509_ASN_ENCODING,
            X509_KEY_USAGE,
            &KeyUsage);
        if (EncodedKeyUsage.empty())
        {
            return false;
        }

        CERT_EXTENSION Extensions[3] = {};
        Extensions[0].pszObjId = const_cast<LPSTR>(szOID_ENHANCED_KEY_USAGE);
        Extensions[0].fCritical = FALSE;
        Extensions[0].Value.cbData = static_cast<DWORD>(
            EncodedEnhancedKeyUsage.size());
        Extensions[0].Value.pbData = EncodedEnhancedKeyUsage.data();
        Extensions[1].pszObjId = const_cast<LPSTR>(szOID_KEY_USAGE);
        Extensions[1].fCritical = FALSE;
        Extensions[1].Value.cbData = static_cast<DWORD>(EncodedKeyUsage.size());
        Extensions[1].Value.pbData = EncodedKeyUsage.data();
        Extensions[2].pszObjId = VmmsOid.data();
        Extensions[2].fCritical = FALSE;
        Extensions[2].Value.cbData = sizeof(VmmsExtensionValue);
        Extensions[2].Value.pbData = VmmsExtensionValue;

        CERT_EXTENSIONS CertificateExtensions = {};
        CertificateExtensions.cExtension = ARRAYSIZE(Extensions);
        CertificateExtensions.rgExtension = Extensions;

        FILETIME CurrentFileTime = {};
        ::GetSystemTimeAsFileTime(&CurrentFileTime);

        const LONGLONG DayInterval = 24LL * 60LL * 60LL * 10000000LL;

        const LONGLONG NotBeforeDays = -1LL;
        const LONGLONG NotAfterDays = 365 * 1000LL;

        SYSTEMTIME StartSystemTime = {};
        {
            LARGE_INTEGER StartTimeValue = {};
            StartTimeValue.LowPart = CurrentFileTime.dwLowDateTime;
            StartTimeValue.HighPart = CurrentFileTime.dwHighDateTime;
            StartTimeValue.QuadPart += NotBeforeDays * DayInterval;
            FILETIME StartFileTime = {};
            StartFileTime.dwLowDateTime = StartTimeValue.LowPart;
            StartFileTime.dwHighDateTime = StartTimeValue.HighPart;
            if (!::FileTimeToSystemTime(&StartFileTime, &StartSystemTime))
            {
                return false;
            }
        }

        SYSTEMTIME EndSystemTime = {};
        {
            LARGE_INTEGER EndTimeValue = {};
            EndTimeValue.LowPart = CurrentFileTime.dwLowDateTime;
            EndTimeValue.HighPart = CurrentFileTime.dwHighDateTime;
            EndTimeValue.QuadPart += NotAfterDays * DayInterval;
            FILETIME EndFileTime = {};
            EndFileTime.dwLowDateTime = EndTimeValue.LowPart;
            EndFileTime.dwHighDateTime = EndTimeValue.HighPart;
            if (!::FileTimeToSystemTime(&EndFileTime, &EndSystemTime))
            {
                return false;
            }
        }

        std::wstring ContainerName;
        {
            GUID Value = {};
            if (SUCCEEDED(::CoCreateGuid(&Value)))
            {
                wchar_t Buffer[64] = {};
                if (::StringFromGUID2(Value, Buffer, ARRAYSIZE(Buffer)))
                {
                    ContainerName = Buffer;
                }
            }
        }
        if (ContainerName.empty())
        {
            return false;
        }

        bool Result = false;

        HCRYPTPROV ProviderHandle = 0;
        if (::CryptAcquireContextW(
            &ProviderHandle,
            ContainerName.c_str(),
            MS_DEF_RSA_SCHANNEL_PROV_W,
            PROV_RSA_SCHANNEL,
            CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
        {
            HCRYPTKEY KeyHandle = 0;
            if (::CryptGenKey(
                ProviderHandle,
                AT_KEYEXCHANGE,
                CRYPT_EXPORTABLE | (2048U << 16),
                &KeyHandle))
            {
                CRYPT_KEY_PROV_INFO KeyProviderInfo = {};
                KeyProviderInfo.pwszContainerName = ContainerName.data();
                KeyProviderInfo.pwszProvName = const_cast<LPWSTR>(
                    MS_DEF_RSA_SCHANNEL_PROV_W);
                KeyProviderInfo.dwProvType = PROV_RSA_SCHANNEL;
                KeyProviderInfo.dwFlags = CRYPT_MACHINE_KEYSET;
                KeyProviderInfo.dwKeySpec = AT_KEYEXCHANGE;

                PCCERT_CONTEXT Context = ::CertCreateSelfSignCertificate(
                    ProviderHandle,
                    &Subject,
                    0,
                    &KeyProviderInfo,
                    nullptr,
                    &StartSystemTime,
                    &EndSystemTime,
                    &CertificateExtensions);
                if (Context)
                {
                    HCERTSTORE StoreHandle = ::CertOpenStore(
                        CERT_STORE_PROV_SYSTEM_W,
                        0,
                        0,
                        CERT_SYSTEM_STORE_LOCAL_MACHINE |
                        CERT_STORE_OPEN_EXISTING_FLAG,
                        L"MY");
                    if (StoreHandle)
                    {
                        if (::CertAddCertificateContextToStore(
                            StoreHandle,
                            Context,
                            CERT_STORE_ADD_ALWAYS,
                            nullptr))
                        {
                            Result = ::GrantVmmsPrivateKeyReadAccess(
                                ProviderHandle);
                        }

                        ::CertCloseStore(StoreHandle, 0);
                    }

                    ::CertFreeCertificateContext(Context);
                }

                ::CryptDestroyKey(KeyHandle);
            }

            ::CryptReleaseContext(ProviderHandle, 0);
        }

        return Result;
    }
}

EXTERN_C VOID WINAPI EnsureVmmsCertificateAvailable()
{
    if (!::IsVmmsCertificateAvailable())
    {
        ::CreateVmmsCertificate();
    }
}
