#ifndef __PARALLEL16_H__
#define __PARALLEL16_H__

#include "../../helper/general.h"

#include "parallel16Def.h"
#include "prallel16Helper.h"

def p16CreateDevice(parallel16Dev_t ** dev){
    __p16Entry("p16CreateDevice(%p)", dev);
    __p16NULLCheck(dev, "dev", "p16CreateDevice", goto returnERR_NULL;);

    (*dev) = malloc(sizeof(parallel16Dev_t));

    memset((*dev), 0, sizeof(*dev));

    return OKE;
    __p16Exit("p16CreateDevice() : OKE");
    returnERR_NULL:
        __p16Exit("p16CreateDevice() : ERR_NULL");
        return ERR_NULL;
        
}

def p16ConfigDevice(parallel16Dev_t * dev, p16DataPin_t* dataPin, p16ControlPin_t * controlPin){
    __p16Entry("p16ConfigDevice(%p, %p, %p)", dev, dataPin, controlPin);
    __p16NULLCheck(dev, "dev", "p16ConfigDevice", goto returnERR_NULL;);
    __p16NULLCheck(dataPin, "dataPin", "p16ConfigDevice", goto returnERR_NULL;);
    __p16NULLCheck(controlPin, "controlPin", "p16ConfigDevice", goto returnERR_NULL;);

    dev->dataPinMask = 0;
    dev->controlPinMask = 0;

    if (0 <= dev->dataPin.__0 && dev->dataPin.__0 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__0); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__1 && dev->dataPin.__1 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__1); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__2 && dev->dataPin.__2 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__2); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__3 && dev->dataPin.__3 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__3); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__4 && dev->dataPin.__4 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__4); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__5 && dev->dataPin.__5 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__5); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__6 && dev->dataPin.__6 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__6); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__7 && dev->dataPin.__7 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__7); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__8 && dev->dataPin.__8 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__8); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__9 && dev->dataPin.__9 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__9); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__10 && dev->dataPin.__10 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__10); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__11 && dev->dataPin.__11 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__11); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__12 && dev->dataPin.__12 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__12); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__13 && dev->dataPin.__13 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__13); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__14 && dev->dataPin.__14 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__14); else goto returnERR_INVALID_ARG;
    if (0 <= dev->dataPin.__15 && dev->dataPin.__15 <= 31) dev->dataPinMask |= (1U << dev->dataPin.__15); else goto returnERR_INVALID_ARG;


    return OKE;
    __p16Exit("p16ConfigDevice() : OKE");
    returnERR_NULL:
        __p16Exit("p16ConfigDevice() : ERR_NULL");
        return ERR_NULL;
    returnERR_INVALID_ARG:
        __p16Exit("p16ConfigDevice() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
        
}

#endif
