/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *  @file       CameraCC3200DMA.h
 *
 *  @brief      Camera driver implementation for a CC3200 Camera controller
 *
 *  The Camera header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/Camera.h>
 *  #include <ti/drivers/Camera/CameraCC3200DMA.h>
 *  @endcode
 *
 *  Refer to @ref Camera.h for a complete description of APIs & example of use.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_Camera_CameraCC3200DMA__include
#define ti_drivers_Camera_CameraCC3200DMA__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/Camera.h>
#include <ti/drivers/ports/HwiP.h>
#include <ti/drivers/ports/SemaphoreP.h>

/**
 *  @addtogroup Camera_STATUS
 *  CameraCC3200DMA_STATUS_* macros are command codes only defined in the
 *  CameraCC3200DMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/camera/CameraCC3200DMA.h>
 *  @endcode
 *  @{
 */

/* Add CameraCC3200DMA_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup Camera_CMD
 *  CameraCC3200DMA_CMD_* macros are command codes only defined in the
 *  CameraCC3200DMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/camera/CameraCC3200DMA.h>
 *  @endcode
 *  @{
 */

/* Add CameraCC3200DMA_CMD_* macros here */

/** @}*/

/* CC3200 camera DMA transfer size */
#define CameraCC3200DMA_DMA_TRANSFER_SIZE  64

/* Camera function table pointer */
extern const Camera_FxnTable CameraCC3200DMA_fxnTable;

/*!
 *  @brief      CameraCC3200DMA Hardware attributes
 *
 *  These fields, with the exception of intPriority,
 *  are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For CC3200Ware these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *
 *  intPriority is the Camera peripheral's interrupt priority, as defined by the
 *  underlying OS.  It is passed unmodified to the underlying OS's interrupt
 *  handler creation code, so you need to refer to the OS documentation
 *  for usage.  For example, for SYS/BIOS applications, refer to the
 *  ti.sysbios.family.arm.m3.Hwi documentation for SYS/BIOS usage of
 *  interrupt priorities.  If the driver uses the ti.drivers.ports interface
 *  instead of making OS calls directly, then the HwiP port handles the
 *  interrupt priority in an OS specific way.  In the case of the SYS/BIOS
 *  port, intPriority is passed unmodified to Hwi_create().
 *
 *  A sample structure is shown below:
 *  @code
 *  const CameraCC3200DMA_HWAttrs CameraCC3200DMAHWAttrs[] = {
 *      {
 *          .baseAddr = CAMERA_BASE,
 *          .intNum = INT_CAMERA,
 *          .intPriority = (~0),
 *          .channelIndex = UDMA_CH22_CAMERA
 *      }
 *  };
 *  @endcode
 */
typedef struct CameraCC3200DMA_HWAttrs {
    /*! Camera Peripheral's base address */
    uint32_t     baseAddr;
    /*! Camera Peripheral's interrupt vector */
    uint32_t    intNum;
    /*! Camera Peripheral's interrupt priority */
    uint32_t    intPriority;
    /*! uDMA controlTable channel index */
    unsigned long channelIndex;
} CameraCC3200DMA_HWAttrs;

/*!
 *  @brief      CameraCC3200DMA Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct CameraCC3200DMA_Object {
    /* Camera control variables */
    bool                opened;            /* Has the obj been opened */
    Camera_CaptureMode  operationMode;     /* Mode of operation of Camera */

    /* Camera capture variables */
    Camera_Callback     captureCallback;   /* Pointer to capture callback */
    uint32_t            captureTimeout;    /* Timeout for capture semaphore */
    void                *captureBuf;       /* Buffer data pointer */
    size_t              bufferlength;      /* Input Buffer length*/
    size_t              frameLength;       /* Captured frame length */

    bool                cameraDMA_PingPongMode; /* DMA ping pong mode */
    size_t              cameraDMAxIntrRcvd;     /* Number of DMA interrupts*/
    bool                inUse;                  /* Camera in Use */

    /* Camera OS objects */
    SemaphoreP_Handle   captureSem;
    HwiP_Handle         hwiHandle;
} CameraCC3200DMA_Object, *CameraCC3200DMA_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Camera_CameraCC3200DMA__include */
