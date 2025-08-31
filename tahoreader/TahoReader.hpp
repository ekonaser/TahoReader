#pragma once

#include <winscard.h>
#include <stdint.h>

class TahoReader
{
private:
    SCARDCONTEXT cardContext;
    SCARDHANDLE cardHandle;
    LPTSTR readers = NULL;
    LONG result;
    BYTE apdu[7] = {0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}, buffer[1025];
    uint16_t blockRemainder;
    DWORD readerNameBufferSize = SCARD_AUTOALLOCATE, activeProtocol, bufferResponseLen;
    const SCARD_IO_REQUEST* choosenProtocol;
public:
    TahoReader();

    template <size_t N>
    void SelectFile(const BYTE(&cmd)[N]);

    BYTE* ReadData(int length);

    ~TahoReader();
};