#include <iostream>
#include <string>
#include <cstring>
#include <StormLib.h>

#ifdef STORMLIB_LINUX
#define GetLastError SErrGetLastError
#endif

using std::cerr;
using std::endl;
using std::string;

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        cerr << "min 2 params" << endl;
        return 1;
    }

    auto mode = argv[1];
    auto isListMode = !strcmp(mode, "-l") || !strcmp(mode, "--list");
    DWORD mpqFlag;
    if (isListMode || !strcmp(mode, "-r") || !strcmp(mode, "--read"))
        mpqFlag = STREAM_FLAG_READ_ONLY;
    else if (!strcmp(mode, "-w") || !strcmp(mode, "--write"))
        mpqFlag = STREAM_FLAG_WRITE_SHARE;
    else
    {
        cerr << "unknown mode: " << mode << endl;
        return 2;
    }

    if (!isListMode && argc < 5)
    {
        cerr << "not enough params for non-list mode" << endl;
        return 3;
    }

    HANDLE mpqHandle = nullptr;
    if (!SFileOpenArchive(argv[2], 0, BASE_PROVIDER_FILE | STREAM_PROVIDER_FLAT | mpqFlag, &mpqHandle))
    {
        cerr << "failed to open MPQ, error: " << GetLastError() << endl;
        return 4;
    }
    auto closeMpq = [&mpqHandle]{ SFileCloseArchive(mpqHandle); };

    if (mpqFlag == STREAM_FLAG_READ_ONLY)
    {
        // for list mode path mask is required
        const auto extraListFileArgIndex = isListMode ? 4 : 5;
        if (extraListFileArgIndex < argc)
            SFileAddListFile(mpqHandle, argv[extraListFileArgIndex]);

        auto mpqInternalPathMask = (!isListMode || argc > 3) ? argv[3] : "*";
        SFILE_FIND_DATA findData;
        if (auto findHandle = SFileFindFirstFile(mpqHandle, mpqInternalPathMask, &findData, nullptr))
        {
            string extractPath;
            if (!isListMode)
            {
                extractPath = string{argv[4]};
                if (extractPath.back() != '/')
#ifdef PLATFORM_WINDOWS
                    if (extractPath.back() != '\\')
#endif
                        extractPath += '/';
            }

            bool nextFileFound;
            do
            {
                auto fileInternalPath = findData.cFileName;
                if (isListMode)
                    std::cout << findData.dwFileSize << " " << fileInternalPath << endl;
                else
                {
                    auto fileExtractPath = extractPath + findData.szPlainName;
                    if (!SFileExtractFile(mpqHandle, fileInternalPath, fileExtractPath.c_str(), SFILE_OPEN_FROM_MPQ))
                        cerr << "failed to extract file '" << fileInternalPath << "', error: " << GetLastError() << endl;
                }
                nextFileFound = SFileFindNextFile(findHandle, &findData);
            } while (nextFileFound);
            SFileFindClose(findHandle);
        }
        else
        {
            auto err = GetLastError();
            cerr << "no files found with mask '" << mpqInternalPathMask << "'";
            if (err != ERROR_NO_MORE_FILES)
                cerr << ", error: " << err;
            cerr << endl;
        }
    }
    else
    {
        string thirdParam{argv[3]}, prefixParam{"--prefix="}, internalPathPrefix;
        int filesStartIndex;
        bool isSinglePrefix;
        if ((isSinglePrefix = thirdParam.find(prefixParam) == 0))
        {
            internalPathPrefix = thirdParam.substr(prefixParam.length());
            filesStartIndex = 4;
        }
        else
        {
            filesStartIndex = 3;
            if ((argc - filesStartIndex) % 2)
            {
                cerr << "each input file must be balanced with an output one" << endl;
                closeMpq();
                return 5;
            }
        }

        for (int i = filesStartIndex; i < argc; ++i)
        {
            string filePath{argv[i]}, internalPath;
            if (isSinglePrefix)
            {
                string fileName;
                auto lastSlashIndex = filePath.rfind('/');
#ifdef PLATFORM_WINDOWS
                if (lastSlashIndex == string::npos)
                    lastSlashIndex = filePath.rfind('\\');
#endif
                internalPath = internalPathPrefix + filePath.substr(lastSlashIndex != string::npos ? lastSlashIndex + 1 : 0);
            }
            else
                internalPath = argv[++i];

            auto s = "'" + filePath + "' => '" + internalPath + "'";
            if (SFileAddFileEx(mpqHandle, filePath.c_str(), internalPath.c_str(), MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING, MPQ_COMPRESSION_PKWARE, MPQ_COMPRESSION_NEXT_SAME))
                std::cout << s << endl;
            else
                cerr << s << " error: " << GetLastError() << endl;
        }

        SFileCompactArchive(mpqHandle, nullptr, false);
    }

    closeMpq();
    return 0;
}
