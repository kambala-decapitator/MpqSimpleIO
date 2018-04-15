#include <iostream>
#include <StormLib/StormLib.h>

using std::cout;
using std::endl;

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        cout << "no params passed" << endl;
        return 1;
    }

    const char *mpqInternalPath = argv[1], *mpqPath = argv[2];
    HANDLE hMPQ = 0;
    if (!SFileOpenArchive(mpqPath, 0, BASE_PROVIDER_FILE | STREAM_PROVIDER_FLAT | STREAM_FLAG_READ_ONLY, &hMPQ))
    {
        cout << "error opening MPQ '" << mpqPath << "': " << GetLastError() << "\neither invalid path or unsupported file." << endl;
        return 1;
    }

    SFILE_FIND_DATA d;
    HANDLE h = SFileFindFirstFile(hMPQ, mpqInternalPath, &d, nullptr);
    bool r;
    do {
        cout << d.szPlainName << " (" << d.cFileName << ") " << d.dwCompSize << " / " << d.dwFileSize << endl;
        cout << "extract " << SFileExtractFile(hMPQ, d.cFileName, d.szPlainName, SFILE_OPEN_FROM_MPQ) << ": " << GetLastError() << endl;
        r = SFileFindNextFile(h, &d);
    } while (r);
    SFileFindClose(h);
    SFileCloseArchive(hMPQ);
    return 0;
}
