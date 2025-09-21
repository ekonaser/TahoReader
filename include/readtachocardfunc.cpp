#include "readtachocardfunc.hpp"

void DFG1(TahoReader& reader)
{
    reader.SelectFile({0x00, 0xA4, 0x04, 0x0C, 0x06, 0xFF, 0x54, 0x41, 0x43, 0x48, 0x4F}); // selecting TAHO app

    // Card Certificate
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0xC1, 0x00});
    cardCertDATAptr = reader.ReadData(194); // exactly 194
    // CA Certificate
    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0xC1, 0x08});
    CACertDATAptr = reader.ReadData(194); // exactly 194

    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x05, 0x20}); // ID section
    memcpy(&idData, reader.ReadData(143), 143);
    idDataNull = IDNull(idData);

    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x05, 0x21}); // driver license section
    driverLicenseDATAptr = reader.ReadData(53);
    licenseDataNull = DriverLicenseNULL(driverLicenseDATAptr);

    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x05, 0x04});
    activitiesDATAptr = reader.ReadData(13780);

    reader.SelectFile({0x00, 0xA4, 0x02, 0x0C, 0x02, 0x05, 0x05});
    vehiclesDATAptr = reader.ReadData(6107); // G1 size of file
    vehicles.readVehicles(vehiclesDATAptr);
}

void DFG2(TahoReader& reader)
{
    reader.SelectFile({0x00, 0xA4, 0x04, 0x0C, 0x06, 0xFF, 0x53, 0x4D, 0x52, 0x44, 0x54}); // selecting SMRDT app
}