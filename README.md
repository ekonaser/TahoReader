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

## Microsoft WebView2 SDK License
Copyright (C) Microsoft Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
   * The name of Microsoft Corporation, or the names of its contributors 
may not be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.