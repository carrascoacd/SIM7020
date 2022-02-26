/*
 * Parser.cpp
 * HTTP library for the SIM7020 board
 *
 * Copyright 2022 Antonio Carrasco
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Parser.h"
#include <Arduino.h>

void Parser::encodeBody(const char *body, char *result){
  ASCIItoHex(body, result);
}

void Parser::parseResponse(const char *response, char *result) {
  char tmp[64];

  // Fetch parameters
  const unsigned int maxParameters = 4;
  unsigned int parameters[maxParameters];
  unsigned int currentParameter = 0;
  unsigned int startContentIndex = 0;
  unsigned int tmpIndex = 0;
  for (unsigned int i = 0; i < strlen(response); ++i){
    tmp[tmpIndex] = response[i];
    tmpIndex ++;
    if (',' == response[i]) {
      tmp[tmpIndex - 1] = '\0';
      tmpIndex = 0;

      parameters[currentParameter] = atoi(tmp);
      currentParameter ++;
    }
    if (maxParameters == currentParameter) {
      startContentIndex = i + 1;
      break;
    }
  }

  // Decode base16 content
  unsigned int resultIndex = 0;
  unsigned int pkgSizeIndex = 3;

  for (unsigned int i = startContentIndex; i < parameters[pkgSizeIndex] * 2 + startContentIndex; i += 2){
    result[resultIndex] = (char)(hexDigit(response[i]) * 16 + hexDigit(response[i+1]));
    resultIndex ++;
  }
  result[resultIndex] = '\0';
}

int Parser::hexDigit(char c){
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  return 0;
}

void Parser::ASCIItoHex(const char *input, char *output){
    int inputIndex = 0;
    int outputIndex = 0;

    while(input[inputIndex] != '\0')
    {
        sprintf((char*)(output+outputIndex),"%02X", input[inputIndex]);
        input++;
        outputIndex+=2;
    }

    output[outputIndex++] = '\0';
}

// 30 = 3×16^1+0×16^0 = 48 = '0' character
void Parser::hexToASCII(const char *input, char *output){
  int outputIndex = 0;
  
  for (unsigned int inputIndex = 0; inputIndex < strlen(input); inputIndex += 2){
    output[outputIndex] = (char)(hexDigit(input[inputIndex]) * 16 + hexDigit(input[inputIndex+1]));
    outputIndex++;
  }
  
   output[outputIndex++] = '\0';
}
