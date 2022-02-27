/*
 * Http.cpp
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

#include "Http.h"

const char AT_OK[] PROGMEM = "OK";
const char AT_OK_[] = "OK";

const char SET_MINIMUM_FUN[] PROGMEM = "AT+CFUN=0\r\n";
const char SET_FULL_FUN[] PROGMEM = "AT+CFUN=1\r\n";
const char FULL_FUN_READY[] PROGMEM = "+CPIN:READY\r\n";
const char SET_APN[] PROGMEM = "AT*MCGDEFCONT=\"IP\",\"%s\"\r\n";
const char GPRS_ATTACH[] PROGMEM = "AT+CGATT?\r\n";
const char GPRS_ATTACHED[] PROGMEM = "+CGATT: 1";
const char READ_PDP_PARAMS[] PROGMEM = "AT+CGCONTRDP\r\n";
const char HTTP_DISCONNECT[] PROGMEM = "AT+CHTTPDISCON=0\r\n";
const char HTTP_DESTROY[] PROGMEM = "AT+CHTTPDESTROY=0\r\n";
const char HTTP_CREATE[] PROGMEM = "AT+CHTTPCREATE=\"%s\"\r\n";
const char HTTP_CONNECT[] PROGMEM = "AT+CHTTPCON=0\r\n";
const char HTTP_SEND_GET[] PROGMEM = "AT+CHTTPSEND=0,0,\"%s\"\r\n";
const char HTTP_SEND_POST[] PROGMEM = "AT+CHTTPSEND=0,1,\"%s\",,\"application/json\",%s\r\n";
const char HTTP_CONTENT_[] = "+CHTTPNMIC: ";
const char HTTP_2XX_[] = "+CHTTPNMIH: 0,2XX";

Result HTTP::connect(const char *apn)
{
  Result result;

  if (sendCmdAndWaitForResp_P(SET_MINIMUM_FUN, AT_OK, 8000) == FALSE)
    result = ERROR_HTTP_INIT;

  char httpApn[64];
  char buffer2[24];
  strcpy_P(buffer2, apn);
  sprintf_P(httpApn, SET_APN, buffer2);
  if (sendCmdAndWaitForResp(httpApn, AT_OK_, 10000) == FALSE)
    result = ERROR_HTTP_INIT;

  if (sendCmdAndWaitForResp_P(SET_FULL_FUN, FULL_FUN_READY, 8000) == FALSE)
    result = ERROR_HTTP_INIT;

  if (sendCmdAndWaitForResp_P(GPRS_ATTACH, GPRS_ATTACHED, 8000) == FALSE)
    result = ERROR_HTTP_INIT;

  if (sendCmdAndWaitForResp_P(READ_PDP_PARAMS, AT_OK, 8000) == FALSE)
    result = ERROR_HTTP_INIT;

  return result;
}

Result HTTP::disconnect()
{
  Result result;

  if (sendCmdAndWaitForResp_P(HTTP_DISCONNECT, AT_OK, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (sendCmdAndWaitForResp_P(HTTP_DESTROY, AT_OK, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  return result;
}

Result HTTP::prepare(const char *host)
{
  Result result;

  char buffer1[64];
  char buffer2[64];

  strcpy_P(buffer2, host);
  sprintf_P(buffer1, HTTP_CREATE, buffer2);
  if (sendCmdAndWaitForResp(buffer1, AT_OK_, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (sendCmdAndWaitForResp_P(HTTP_CONNECT, AT_OK, 10000) == TRUE)
    result = SUCCESS;

  return result;
}

Result HTTP::post(const char *host, const char *path, const char *body, char *response) 
{
  Result result = prepare(host);

  char buffer1[400];
  char buffer2[64];
  char encodedBody[256];

  parser->encodeBody(body, encodedBody);

  cleanBuffer(buffer1, sizeof(buffer1));
  cleanBuffer(buffer2, sizeof(buffer2));
  cleanBuffer(response, sizeof(response));

  strcpy_P(buffer2, path);
  sprintf_P(buffer1, HTTP_SEND_POST, buffer2, encodedBody);

  if (sendCmdAndWaitForResp(buffer1, HTTP_2XX_, 8000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (waitForResp(HTTP_CONTENT_, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  cleanBuffer(buffer1, sizeof(buffer1));
  cleanBuffer(buffer2, sizeof(buffer2));
  cleanBuffer(response, sizeof(response));

  if (readBuffer(buffer1, sizeof(buffer1)) == FALSE)
    return ERROR_HTTP_CLOSE;

  parser->parseResponse(buffer1, response);

  return result;
}

Result HTTP::get(const char *host, const char *path, char *response)
{
  Result result = prepare(host);
  
  char buffer1[400];
  char buffer2[64];

  strcpy_P(buffer2, path);
  sprintf_P(buffer1, HTTP_SEND_GET, buffer2);
  if (sendCmdAndWaitForResp(buffer1, HTTP_2XX_, 8000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (waitForResp(HTTP_CONTENT_, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  cleanBuffer(buffer1, sizeof(buffer1));
  cleanBuffer(buffer2, sizeof(buffer2));
  cleanBuffer(response, sizeof(response));

  if (readBuffer(buffer1, sizeof(buffer1)) == FALSE)
    return ERROR_HTTP_CLOSE;

  parser->parseResponse(buffer1, response);

  return result;
}
