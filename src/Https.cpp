/*
 * Http.cpp
 * A HTTP library for the SIM7020 board
 *
 * Copyright 2021 Antonio Carrasco
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

#include "Https.h"

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
const char HTTP_SEND[] PROGMEM = "AT+CHTTPSEND=0,0,\"%s\"\r\n";
const char HTTP_CONTENT_[] = "+CHTTPNMIC: ";
const char HTTP_2XX_[] = "+CHTTPNMIH: 0,2XX";

Result HTTPS::connect(const char *apn)
{
  Result result;

  if (sendCmdAndWaitForResp_P(SET_MINIMUM_FUN, AT_OK, 8000) == FALSE)
    result = ERROR_HTTP_INIT;

  char httpApn[64];
  char tmp[24];
  strcpy_P(tmp, apn);
  sprintf_P(httpApn, SET_APN, tmp);
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

Result HTTPS::disconnect()
{
  Result result;

  if (sendCmdAndWaitForResp_P(HTTP_DISCONNECT, AT_OK, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (sendCmdAndWaitForResp_P(HTTP_DESTROY, AT_OK, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  return result;
}

int hexDigit(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  return 0;
}


Result HTTPS::get(const char *host, const char *path, char *response)
{
  Result result;

  char buffer[512];
  char tmp[128];
  strcpy_P(tmp, host);
  sprintf_P(buffer, HTTP_CREATE, tmp);
  if (sendCmdAndWaitForResp(buffer, AT_OK_, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (sendCmdAndWaitForResp_P(HTTP_CONNECT, AT_OK, 10000) == TRUE)
    result = SUCCESS;
  
  strcpy_P(tmp, path);
  sprintf_P(buffer, HTTP_SEND, tmp);
  if (sendCmdAndWaitForResp(buffer, HTTP_2XX_, 8000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  if (waitForResp(HTTP_CONTENT_, 5000) == FALSE)
    result = ERROR_HTTP_CLOSE;

  cleanBuffer(buffer, sizeof(buffer));
  cleanBuffer(tmp, sizeof(tmp));
  cleanBuffer(response, sizeof(response));

  if (readBuffer(buffer, sizeof(buffer)) == FALSE)
    return ERROR_HTTP_CLOSE;

  //+CHTTPNMIC: 0,0,21,21,7b2277656174686572456e7472696573223a5b5d7d

  // Fetch parameters
  const unsigned int maxParameters = 4;
  unsigned int parameters[maxParameters];
  unsigned int currentParameter = 0;
  unsigned int startContentIndex = 0;
  unsigned int tmpIndex = 0;
  for (unsigned int i = 0; i < strlen(buffer); ++i){
    tmp[tmpIndex] = buffer[i];
    tmpIndex ++;
    if (',' == buffer[i]) {
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

  // Parse content
  unsigned int responseIndex = 0;
  unsigned int pkgSizeIndex = 3;

  for (unsigned int i = startContentIndex; i < parameters[pkgSizeIndex] * 2 + startContentIndex; i += 2){
    response[responseIndex] = (char)(hexDigit(buffer[i]) * 16 + hexDigit(buffer[i+1]));
    responseIndex ++;
  }
  response[responseIndex] = '\0';

  return result;
}
