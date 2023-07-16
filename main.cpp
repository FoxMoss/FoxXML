#include "handler.hpp"
#include <expat.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("ERROR: No file path.\n");
    return 1;
  }
  std::string filePath(argv[1]);

  XML_Parser parser = XML_ParserCreate(NULL);

  if (!parser) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    return 1;
  }

  XML_SetUserData(parser, getDepth());
  XML_SetElementHandler(parser, startElement, endElement);
  XML_SetCharacterDataHandler(parser, elementData);

  std::ostringstream xmlBuffer;
  std::ifstream xmlFile(filePath);

  xmlBuffer << xmlFile.rdbuf();
  xmlFile.close();
  std::string xmlString = xmlBuffer.str();

  if (xmlString.length() == 0) {
    printf("ERROR: null content\n");
    return 1;
  }

  if (XML_Parse(parser, xmlString.c_str(), xmlString.length(), XML_TRUE) ==
      XML_STATUS_ERROR) {
    printf("ERROR: XML parser error.");
    return 1;
  }

  XML_ParserFree(parser);

  run(argc, argv);

  return 0;
}
