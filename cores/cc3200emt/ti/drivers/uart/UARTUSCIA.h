/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       UARTUSCIA.h
 *
 *  @brief      UART driver implementation for a USCIA peripheral
 *
 *  The UART header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/UART.h>
 *  #include <ti/drivers/uart/UARTUSCIA.h>
 *  @endcode
 *
 *  Refer to @ref UART.h for a complete description of APIs & example of use.
 *
 *  This UART driver implementation is designed to operate on a UCSI controller
 *  in UART mode. It uses the APIs for a USCIA controller.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_uart_UARTUSCIA__include
#define ti_drivers_uart_UARTUSCIA__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/UART.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/utils/RingBuf.h>

/**
 *  @addtogroup UART_STATUS
 *  UARTUSCIA_STATUS_* macros are command codes only defined in the
 *  UARTUSCIA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTUSCIA.h>
 *  @endcode
 *  @{
 */

/* Add UARTUSCIA_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup UART_CMD
 *  UARTUSCIA_CMD_* macros are command codes only defined in the
 *  UARTUSCIA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTUSCIA.h>
 *  @endcode
 *  @{
 */

/* Add UARTUSCIA_CMD_* macros here */

/** @}*/

/* UARTUSCIA function table pointer */
extern const UART_FxnTable UARTUSCIA_fxnTable;

/*!
 *  @brief Complement set of read functions to be used by the UART ISR and
 *         UARTUSCIA_read(). Internal use only.
 *
 *  These functions should not be used by the user and are solely intended for
 *  the UARTUSCIA driver.
 *  The UARTUSCIA_FxnSet is a pair of functions that are designed to
 *  operate with one another in a task context and in an ISR context. The
 *  readTaskFxn is called by UARTUSCIA_read() to drain a circular buffer,
 *  whereas the readIsrFxn is used by the UARTUSCIA_hwiIntFxn to fill up the
 *  circular buffer.
 *
 *  readTaskFxn:    Function called by UART read
 *                  These variables are set and available for use to the
 *                  readTaskFxn.
 *                  object->readBuf = buffer; //Pointer to a user buffer
 *                  object->readSize = size;  //Desired no. of bytes to read
 *                  object->readCount = size; //Remaining no. of bytes to read
 *
 *  readIsrFxn:     The required ISR counterpart to readTaskFxn
 */
typedef struct UARTUSCIA_FxnSet {
    bool (*readIsrFxn)  (UART_Handle handle);
    int  (*readTaskFxn) (UART_Handle handle);
} UARTUSCIA_FxnSet;

/*!
 *  @brief      UARTUSCIA Baudrate configuration
 *
 *  This structure is used to specifiy the usci controller's clock divider
 *  settings to achieve the desired baudrate given the indicated clock input
 *  frequency.
 *  Divider values can be determined by referring to the MSP430 baudrate
 *  calculator.
 *  http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 *
 *  A sample structure is shown below:
 *  @code
 *  const UARTUSCIA_BaudrateConfig uartUSCIABaudrates[] = {
 *   // Baudrate, input clock (Hz), Prescalar, UCBRFx, UCBRSx, oversampling
 *      {
 *          .outputBaudrate = 115200,
 *          .inputClockFreq = 8192000,
 *          .prescalar = 4,
 *          .hwRegUCBRFx = 7,
 *          .hwRegUCBRSx = 0,
 *          .oversampling = 1
 *      },
 *      {9600,    8192000,          53,        5,      0,      1},
 *      {9600,    32768,            3,         0,      3,      0},
 *  };
 *  @endcode
 */
typedef struct UARTUSCIA_BaudrateConfig {
    uint32_t  outputBaudrate; /*! Search criteria: desired baudrate */
    uint32_t  inputClockFreq; /*! Search criteria: given this input clock frequency */

    uint8_t   prescalar;      /*! Clock prescalar */
    uint8_t   hwRegUCBRFx;    /*! UCBRFx lookup entry */
    uint8_t   hwRegUCBRSx;    /*! UCBRSx lookup entry */
    uint8_t   oversampling;   /*! Oversampling mode (1: True; 0: False) */
} UARTUSCIA_BaudrateConfig;

/*!
 *  @brief      UARTUSCIA Hardware attributes
 *
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For MSP430Ware these definitions are found in:
 *      - inc/hw_memmap.h
 *      - usci_a_uart.h
 *
 *  A sample structure is shown below:
 *  @code
 *  const UARTUSCIA_BaudrateConfig uartUSCIABaudrates[] = {
 *   // Baudrate, input clock (Hz), UCBRx, UCBRFx, UCBRSx, oversampling
 *      {
 *          .outputBaudrate = 115200,
 *          .inputClockFreq = 8192000,
 *          .prescalar = 4,
 *          .hwRegUCBRFx = 7,
 *          .hwRegUCBRSx = 0,
 *          .oversampling = 1
 *      },
 *      {9600,    8192000,          53,    5,      0,      1},
 *      {9600,    32768,            3,     0,      3,      0},
 *  };
 *
 *  unsigned char uartMSP432RingBuffer[2][32];
 *
 *  const UARTUSCIA_HWAttrsV1 uartUSCIAHWAttrs[] = {
 *      {
 *          .baseAddr = USCI_A0_BASE,
 *          .clockSource = USCI_A_UART_CLOCKSOURCE_SMCLK,
 *          .bitOrder = USCI_A_UART_LSB_FIRST,
 *          .numBaudrateEntries = sizeof(uartUSCIABaudrates/UARTUSCIA_BaudrateConfig),
 *          .baudrateLUT = uartUSCIABaudrates
 *          .ringBufPtr  = uartMSP432RingBuffer[0],
 *          .ringBufSize = sizeof(uartMSP432RingBuffer[0])
 *      },
 *      {
 *          .baseAddr = USCI_A1_BASE,
 *          .clockSource = USCI_A_UART_CLOCKSOURCE_SMCLK,
 *          .bitOrder = USCI_A_UART_LSB_FIRST,
 *          .numBaudrateEntries = sizeof(uartUSCIABaudrates/UARTUSCIA_BaudrateConfig),
 *          .baudrateLUT = uartUSCIABaudrates
 *          .ringBufPtr  = uartMSP432RingBuffer[1],
 *          .ringBufSize = sizeof(uartMSP432RingBuffer[1])
 *      },
 *  };
 *  @endcode
 */
typedef struct UARTUSCIA_HWAttrsV1 {
    /*! USCI_A_UART Peripheral's base address */
    unsigned int baseAddr;
    /*! USCI_A_UART Clock source */
    uint8_t    clockSource;
    /*!< USCI_A_UART Bit order */
    uint32_t   bitOrder;
    /*!< Number of UARTUSCIA_BaudrateConfig entries */
    unsigned int numBaudrateEntries;
    /*!< Pointer to a table of possible UARTUSCIA_BaudrateConfig entries */
    UARTUSCIA_BaudrateConfig const *baudrateLUT;
    /*! Pointer to a application ring buffer */
    unsigned char  *ringBufPtr;
    /*! Size of ringBufPtr */
    size_t          ringBufSize;
} UARTUSCIA_HWAttrsV1;

/*!
 *  @brief      UARTUSCIA Object
 *
 *  Not intended to be used by the user.
 */
typedef struct UARTUSCIA_Object {
    /* UARTUSCIA control variables */
    bool                 isOpen;           /* Status for open */
    UART_Mode            readMode;         /* Mode for all read calls */
    UART_Mode            writeMode;        /* Mode for all write calls */
    UART_ReturnMode      readReturnMode;   /* Receive return mode */
    UART_DataMode        readDataMode;     /* Type of data being read */
    UART_DataMode        writeDataMode;    /* Type of data being written */
    UART_Echo            readEcho;         /* Echo received data back */
    /*
     * Flag to determine if a timeout has occurred when the user called
     * UART_read(). This flag is set by the timeoutClk clock object.
     */
    bool                 bufTimeout:1;
    /*
     * Flag to determine when an ISR needs to perform a callback; in both
     * UART_MODE_BLOCKING or UART_MODE_CALLBACK
     */
    bool                 callCallback:1;
    /*
     * Flag to determine if the ISR is in control draining the ring buffer
     * when in UART_MODE_CALLBACK
     */
    bool             drainByISR:1;

    Clock_Struct         timeoutClk;       /* Clock object to for timeouts */
    /* UARTEUSCIA read variables */
    RingBuf_Object       ringBuffer;       /* local circular buffer object */
    /* A complement pair of read functions for both the ISR and UART_read() */
    UARTUSCIA_FxnSet     readFxns;
    void                *readBuf;          /* Buffer data pointer */
    size_t               readSize;         /* Chars remaining in buffer */
    size_t               readCount;        /* Number of Chars read */
    Semaphore_Struct     readSem;          /* UARTEUSCIA read semaphore */
    unsigned int         readTimeout;      /* Timeout for read semaphore */
    UART_Callback        readCallback;     /* Pointer to read callback */

    /* UARTEUSCIA write variables */
    const void          *writeBuf;         /* Buffer data pointer */
    size_t               writeCount;       /* Number of Chars sent */
    size_t               writeSize;        /* Chars remaining in buffer */
    bool                 writeCR;          /* Write a return character */
    Semaphore_Struct     writeSem;         /* UARTEUSCIA write semaphore */
    unsigned int         writeTimeout;     /* Timeout for write semaphore */
    UART_Callback        writeCallback;    /* Pointer to write callback */
} UARTUSCIA_Object, *UARTUSCIA_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_uart_UARTUSCIA__include */
