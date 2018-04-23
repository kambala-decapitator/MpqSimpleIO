#include <iostream>
#include <string>
#include <cstring>
#include <StormLib/StormLib.h>

using std::cerr;
using std::endl;

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        cerr << "min 2 params" << endl;
        return 1;
    }

    auto mode = argv[1];
    auto isListMode = !strcmp(mode, "-l");
    DWORD mpqFlag;
    if (isListMode || !strcmp(mode, "-r"))
        mpqFlag = STREAM_FLAG_READ_ONLY;
    else if (!strcmp(mode, "-w"))
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

    if (mpqFlag == STREAM_FLAG_READ_ONLY)
    {
        auto mpqInternalPathMask = (!isListMode || argc > 3) ? argv[3] : "*";
        SFILE_FIND_DATA findData;
        if (auto findHandle = SFileFindFirstFile(mpqHandle, mpqInternalPathMask, &findData, nullptr))
        {
            std::string extractPath;
            if (!isListMode)
            {
                extractPath = std::string{argv[4]};
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
                    std::cout << fileInternalPath << endl;
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

    SFileCloseArchive(mpqHandle);
    return 0;
}
