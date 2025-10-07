# Taho reader

## Note
Program is being developed with help of EU Regulation 2016/799 which defines how the Tachograph card is built.

It is tested on [USB 2.0 Smart Card Reader](https://www.ewent-eminent.com/en/products/68-card-reader-connectivity/usb-20-smart-card-id-reader) that supports ISO-7816 standard for basic APDU commands.

As of today, the program successfully supports reading driver cards via the smart card reader and handling
standardized .DDD files.

This project does not redistribute Microsoft binaries or SDK components. It relies on system-installed
headers and libraries available through official Windows development environments. This project statically links
several community libraries for portability and performance. All linked libraries are used in accordance
with their respective licenses.

The program serves for basic data analysis for now and later maybe for advanced.

There are few minor changes that needs to be done regarding previous microsoft compiler MSVC.

Opening and saving files should be blazingly fast. Same can't be said for reading actual tachograph
card, because we need to consider engineering of first generation card section which can't be read fast as
second therefore using stack is irrelevant here.

## AI Collaboration

Built from scratch, byte by byte, struct by struct — with guidance from Microsoft Copilot.
No extensive AI code generation.