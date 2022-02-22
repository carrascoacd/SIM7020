/*
 * Http.h
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

#ifndef __HTTP_H__
#define __HTTP_H__

#include "Sim7020.h"
#include "Result.h"
#include "Parser.h"
class HTTP : public SIM7020
{

public:
  HTTP(unsigned int baudRate,
       unsigned int rxPin,
       unsigned int txPin,
       unsigned int rstPin) : SIM7020(baudRate, rxPin, txPin, rstPin){
    parser = new Parser();
  };
  Result connect(const char *apn);
  Result disconnect();
  Result get(const char *host, const char *path, char *response);
  Result post(const char *host, const char *path, const char *body, char *response);

private: 
  Result prepare(const char *host);

  Parser *parser;
};

#endif
