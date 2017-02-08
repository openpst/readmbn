/**
*
* (c) Gassan Idriss <ghassani@gmail.com>
* 
* This file is part of readmbn.
* 
* readmbn is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* readmbn is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with readmbn. If not, see <http://www.gnu.org/licenses/>.
*
* @file main.cpp
* @package openpst/readmbn
* @brief CLI tool for reading and display information about files in Qualcomm mbn format
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "tclap/CmdLine.h"
#include "qualcomm/mbn.h"
#include "qualcomm/mbn_parser.h"

using TCLAP::CmdLine;
using TCLAP::SwitchArg;
using TCLAP::ValueArg;
using TCLAP::UnlabeledValueArg;
using TCLAP::ArgException;
using namespace OpenPST::QC;

size_t write_data(std::ofstream& file, std::vector<uint8_t>& data)
{
	if (!file.is_open()) {
		return 0;
	}

	file.write(reinterpret_cast<const char*>(&data[0]), data.size());

	return data.size();
}

int main(int argc, char* argv[])
{
	try {  
		
		CmdLine args("readmbn", ' ', "1.0");    	
    	UnlabeledValueArg<std::string> filePathArg("file", "MBN file to read", true, ""," string");
    	ValueArg<std::string> extractArg("e", "extract", "Extract a segement to out file [-o|--out]. One of code, signature, x509 (x509 is in DER format)", false, "", "string");
    	ValueArg<std::string> outArg("o", "out", "Out file, required if extract option is used", false, "", "string");
   
		args.add(filePathArg);
		args.add(extractArg);
		args.add(outArg);

		args.parse(argc, argv);

		MbnParser parser;

		int flags = 0;

		if (extractArg.getValue().compare("x509") == 0) {
			flags |= kMbnParserFlagExtractX509;
		} else if (extractArg.getValue().compare("code") == 0) {
			flags |= kMbnParserFlagExtractCode;
		} else if (extractArg.getValue().compare("signature") == 0) {
			flags |= kMbnParserFlagExtractSignature;
		}

		if (
			extractArg.getValue().size() && 
			(flags & kMbnParserFlagExtractX509 || flags & kMbnParserFlagExtractCode || flags & kMbnParserFlagExtractSignature) &&
			!outArg.getValue().size()
		) {
			std::cerr << "Specified extract option but no output file was specified" << std::endl;
			return 0;
		}

		auto fileInfo = parser.parse(filePathArg.getValue(), flags);

		printf("Image ID: %d\n", fileInfo->getImageId());
		printf("Image Name: %s\n", fileInfo->getImageName().c_str());
		printf("File Size: %lu bytes\n", fileInfo->getFileSize());
		printf("Image Src: 0x%08X\n", fileInfo->getImageSrc());
		printf("Code Size: %d\n", fileInfo->getCodeSize());
		printf("Image Size: %d bytes\n", fileInfo->getImageSize());
		printf("Image Destination (RAM): 0x%08X\n", fileInfo->getImageDestPtr());
		printf("Signature Size: %d\n", fileInfo->getSignatureSize());
		printf("Signature Pointer (RAM): 0x%08X\n", fileInfo->getSignaturePtr());
		printf("Certificate Chain Size: %d\n", fileInfo->getCertChainSize());
		printf("Certificate Chain Pointer (RAM): 0x%08X\n", fileInfo->getCertChainPtr());

		if (fileInfo->header.size() == MBN_HEADER_MAX_SIZE) {
			printf("OEM Root Cert Selection: %d\n", fileInfo->getOemRootCertSelection());
			printf("OEM Number of Root Certs: %d\n", fileInfo->getOemNumberOfRootCerts());
		}

		if (
			extractArg.getValue().size() && 
			(extractArg.getValue().compare("code") == 0 || 
				extractArg.getValue().compare("signature") == 0 || 
				extractArg.getValue().compare("x509") == 0)
		) {
			size_t written = 0;
			std::ofstream outFile(outArg.getValue().c_str(), std::ios::out | std::ios::binary);
			
			if (!outFile.is_open()) {
				std::cerr << "Error opening " << outArg.getValue() << " for writing" << std::endl;
				return 0;
			}

			if (extractArg.getValue().compare("code") == 0) {
				written = write_data(outFile, fileInfo->code);;
			} else if (extractArg.getValue().compare("signature") == 0) {
				written = write_data(outFile, fileInfo->signature);
			} else {
				written = write_data(outFile, fileInfo->x509);;
			}
			
			outFile.close();

			std::cout << "Wrote " << written << " bytes to " << outArg.getValue() << std::endl;
		}

	} catch (ArgException &e) {
		std::cerr << "Error parsing arguments: " << e.error() << " for argument " << e.argId() << std::endl;
	} catch (MbnParserException &e) {
		std::cerr << "Error parsing: " << e.what() << std::endl;
	}

	return 0;
}