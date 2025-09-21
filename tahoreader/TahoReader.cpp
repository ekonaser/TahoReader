#include "TahoReader.hpp"
#include "structs.hpp"
#include "functions.hpp"

TahoReader::TahoReader() {
    result = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &cardContext);
    if (result != SCARD_S_SUCCESS) status = TRUE;
    result = SCardListReaders(cardContext, NULL, (LPTSTR)&readers, &readerNameBufferSize);
    if (result != SCARD_S_SUCCESS) status = TRUE;
    result = SCardConnect(cardContext, readers, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &cardHandle, &activeProtocol);
    if (result != SCARD_S_SUCCESS) status = TRUE;
}

bool TahoReader::CheckStatus()
{
    return status;
}

TahoReader::~TahoReader() {
    SCardDisconnect(cardHandle, SCARD_UNPOWER_CARD); // u cant do leave card
    if (readers != NULL) {
        SCardFreeMemory(cardContext, readers);
    }
    SCardReleaseContext(cardContext);
}

template <size_t N>
void TahoReader::SelectFile(const BYTE(&cmd)[N]) {
    bufferResponseLen = sizeof(buffer);
    SCardTransmit(cardHandle, choosenProtocol, cmd, N, NULL, buffer, &bufferResponseLen);
}

template void TahoReader::SelectFile<11>(const BYTE(&cmd)[11]);

BYTE* TahoReader::ReadData(int length) {
    
    BYTE* ptr = new BYTE[length];
    int currentlyRead = 0;
    Iterator1023 it(length);

    while (blockRemainder = it.next()) {
        apdu[2] = (currentlyRead >> 8) & 0xFF;
        apdu[3] = currentlyRead & 0xFF;
        apdu[5] = (blockRemainder >> 8) & 0xFF;
        apdu[6] = blockRemainder & 0xFF;

        SelectFile(apdu);
        memcpy(ptr + currentlyRead, buffer, blockRemainder);
        currentlyRead += blockRemainder;
    }

    apdu[2] = 0x00;
    apdu[3] = 0x00;

    return ptr;
}
