/*
 * SIM7020.h
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

#ifndef __SIM7020_H__
#define __SIM7020_H__

#include <Arduino.h>
#include <SoftwareSerial.h>

#define TRUE 1
#define FALSE 0
#define DEFAULT_TIMEOUT 5000

/** SIM7020 class.
 *  Used for SIM7020 communication. attention that SIM7020 module communicate with MCU in serial protocol
 */

class SIM7020
{

public:
    /** Create SIM7020 Instance
     *  @param baudRate baud rate of uart communication
     *  @param rxPin uart receive pin to communicate with SIM7020
     *  @param txPin uart transmit pin to communicate with SIM7020
     *  @param rstPin reset pin of SIM7020
     *  @param verbose controls if the Serial is used for debugging purposes
     */
    SIM7020(unsigned int baudRate,
            unsigned int rxPin,
            unsigned int txPin,
            unsigned int rstPin,
            int verbose = TRUE) : serialSIM7020(rxPin, txPin)
    {
        serialSIM7020.begin(baudRate);
        resetPin = rstPin;
        verboseEnabled = verbose;
    };

    /** Check if SIM7020 readable
     */
    int checkReadable(void);

    /** read from SIM7020 module and save to buffer array
     *  @param  buffer  buffer array to save what read from SIM7020 module
     *  @param  count   the maximal bytes number read from SIM7020 module
     *  @param  timeOut time to wait for reading from SIM7020 module
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int readBuffer(char *buffer, int count, unsigned int timeOut = DEFAULT_TIMEOUT);

    /** clean Buffer
     *  @param buffer   buffer to clean
     *  @param count    number of bytes to clean
     */
    void cleanBuffer(char *buffer, int count);

    /** send AT command to SIM7020 module
     *  @param cmd  command array which will be send to GPRS module
     *  @param delayBeforeSend  integer indicating the sime to wait before sending a command
     */
    void sendCmd(const char *cmd, unsigned int delayBeforeSend = 0);

    /**send "AT" to SIM7020 module
     */
    int sendATTest(void);

    /**send '0x1A' to SIM7020 Module
     */
    void sendEndMark(void);

    /** check SIM7020 module response before time out
     *  @param  *resp   correct response which SIM7020 module will return. A 'X' char
     *  can be used to allow any char matching character, example:
     *  '200' and '201' matches with '20X'
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int waitForResp(const char *resp, unsigned timeout);

    /** send AT command to GPRS module and wait for correct response
     *  @param  *cmd    AT command which will be send to GPRS module
     *  @param  *resp   correct response which GPRS module will return
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int sendCmdAndWaitForResp(const char *cmd, const char *resp, unsigned timeout);

    /**
     *  The sendCmdAndWaitForResp_P() function is similar to sendCmdAndWaitForResp()
     *  except that cmd and resp are pointers to a string in program space.
     */
    int sendCmdAndWaitForResp_P(const char *cmd, const char *resp, unsigned timeout);

    /** used for serial debug, you can specify tx and rx pin and then communicate with GPRS module with common AT commands
     */
    void serialDebug(void);

    /*
     * Sets the module in software sleep mode
     */
    int sleep();

    /*
     * Prepares SIM7020 to start working in full mode removing the sleep mode if set.
     */
    int wakeUp();

    void purgeSerial();
    void write(const char *data);
    void write(const char *data, unsigned int size);

protected:
    SoftwareSerial serialSIM7020;
    unsigned int resetPin;
    int verboseEnabled;
};

#endif
