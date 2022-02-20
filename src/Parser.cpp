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
#include <stdio.h>
#include <stdlib.h>

void Parser::parseResponse(const char *response, char *result) {
  // char tmp[64];

  // //+CHTTPNMIC: 0,0,21,21,7b2277656174686572456e7472696573223a5b5d7d

  // // Fetch parameters
  // const unsigned int maxParameters = 4;
  // unsigned int parameters[maxParameters];
  // unsigned int currentParameter = 0;
  // unsigned int startContentIndex = 0;
  // unsigned int tmpIndex = 0;
  // for (unsigned int i = 0; i < strlen(response); ++i){
  //   tmp[tmpIndex] = response[i];
  //   tmpIndex ++;
  //   if (',' == response[i]) {
  //     tmp[tmpIndex - 1] = '\0';
  //     tmpIndex = 0;

  //     parameters[currentParameter] = atoi(tmp);
  //     currentParameter ++;
  //   }
  //   if (maxParameters == currentParameter) {
  //     startContentIndex = i + 1;
  //     break;
  //   }
  // }

  // // Decode base16 content
  // unsigned int resultIndex = 0;
  // unsigned int pkgSizeIndex = 3;

  // for (unsigned int i = startContentIndex; i < parameters[pkgSizeIndex] * 2 + startContentIndex; i += 2){
  //   result[resultIndex] = (char)(hexDigit(response[i]) * 16 + hexDigit(response[i+1]));
  //   resultIndex ++;
  // }
  // result[resultIndex] = '\0';
}
