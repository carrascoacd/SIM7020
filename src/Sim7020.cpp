/*
 * SIM7020.cpp
 * A library for SeeedStudio seeeduino GPRS shield
 *
 * Original work Copyright (c) 2013 seeed technology inc. [lawliet zou]
 * Modified work Copyright 2022 Antonio Carrasco
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

#include "SIM7020.h"

const char SOFTWARE_SLEEP_MODE[] PROGMEM = "AT+CSCLK=2\r\n";
const char NORMAL_MODE[] PROGMEM = "AT+CSCLK=0\r\n";
const char AT_OK[] PROGMEM = "OK";
const char AT[] PROGMEM = "AT\r\n";

int SIM7020::checkReadable(void)
{
    return serialSIM7020.available();
}

int SIM7020::readBuffer(char *buffer, int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart = millis();
    while (1)
    {
        while (serialSIM7020.available())
        {
            char c = serialSIM7020.read();
            buffer[i] = c;
            buffer[i + 1] = '\0';
            ++i;
            if (i > count - 1)
                break;
        }
        if (i > count - 1)
            break;

        unsigned long timerEnd = millis();
        if (timerEnd - timerStart > timeOut)
        {
            break;
        }
    }

    while (serialSIM7020.available())
    {
        serialSIM7020.read();
    }
    return TRUE;
}

void SIM7020::cleanBuffer(char *buffer, int count)
{
    for (int i = 0; i < count; i++)
    {
        buffer[i] = '\0';
    }
}

void SIM7020::sendCmd(const char *cmd, unsigned int delayBeforeSend)
{
    serialSIM7020.listen();
    delay(delayBeforeSend);
    write(cmd);
    serialSIM7020.flush();
}

int SIM7020::sendATTest(void)
{
    // We use low timeout since it is needed to activate the UART from sleep mode.
    // Otherwise the module will go to sleep automatically and the AT cmd won´t
    // have any effect.
    return sendCmdAndWaitForResp_P(AT, AT_OK, 500);
}

int SIM7020::waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum = 0;
    unsigned long timerStart = millis();

    while (1)
    {
        if (serialSIM7020.available())
        {
            char c = serialSIM7020.read();

            if (TRUE == verboseEnabled)
                Serial.print(c);

            sum = (c == resp[sum] || resp[sum] == 'X') ? sum + 1 : 0;
            if (sum == len)
                break;
        }
        unsigned long timerEnd = millis();
        if (timerEnd - timerStart > timeout)
        {
            return FALSE;
        }
    }

    while (serialSIM7020.available())
    {
        serialSIM7020.read();
    }

    return TRUE;
}

void SIM7020::sendEndMark(void)
{
    serialSIM7020.println((char)26);
}

int SIM7020::sendCmdAndWaitForResp(const char *cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp, timeout);
}

int SIM7020::sendCmdAndWaitForResp_P(const char *cmd, const char *resp, unsigned timeout)
{
    char cmdBuff[128];
    char respBuff[32];
    strcpy_P(cmdBuff, cmd);
    strcpy_P(respBuff, resp);

    return sendCmdAndWaitForResp(cmdBuff, respBuff, timeout);
}

void SIM7020::serialDebug(void)
{
    while (1)
    {
        if (serialSIM7020.available())
        {
            Serial.write(serialSIM7020.read());
        }
        if (Serial.available())
        {
            serialSIM7020.write(Serial.read());
        }
    }
}

void SIM7020::purgeSerial()
{
    while (serialSIM7020.available())
        serialSIM7020.read();
}

void SIM7020::write(const char *data)
{
    serialSIM7020.listen();
    serialSIM7020.write(data);
}

void SIM7020::write(const char *data, unsigned int size)
{
    serialSIM7020.listen();
    serialSIM7020.write(data, size);
}

int SIM7020::sleep()
{
    return sendCmdAndWaitForResp_P(SOFTWARE_SLEEP_MODE, AT_OK, DEFAULT_TIMEOUT);
}

int SIM7020::wakeUp()
{
    /*
    pinMode(resetPin, OUTPUT);

    digitalWrite(resetPin, HIGH);
    delay(200);
    digitalWrite(resetPin, LOW);
    delay(2000);
    digitalWrite(resetPin, HIGH);
    delay(3000);
    */

    // Ensure no data is left to transmit.
    purgeSerial();
    serialSIM7020.flush();

    // Sending any AT cmd through the UART will wake up the module from sleep mode.
    for (int i = 0; i < 3; ++i)
        sendATTest();

    return sendCmdAndWaitForResp_P(NORMAL_MODE, AT, DEFAULT_TIMEOUT);
}
