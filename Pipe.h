/**
 * @file    Pipe.h
 *
 * @brief   Declaration of class Pipe
 *
 * @note    This example code is free software: you can redistribute it and/or modify it.
 *
 *          This program is provided by EDI on an "AS IS" basis without
 *          any warranties in the hope that it will be useful.
 * 
 * Gatis Gaigals, 2024
 */

#ifndef _PIPE_H_
#define _PIPE_H_


#ifndef EXCEPTIONS_SUPPORTED
#define EXCEPTIONS_SUPPORTED 0
#endif


#include "ByteArray.h"


enum class StatusCode {
    OK,
    PENDING,
    PARTIAL,
    ERROR,
    REPEAT,
    NEXT
};


class Pipe {

    public:

        using ProcessorFunc = StatusCode (*)( ByteArray*, ByteArray* );

        Pipe( 
            ByteArray* pInput_data,
            ProcessorFunc processor,
            ByteArray* pOutput_data = nullptr
        );

        StatusCode process();

        ByteArray* getInputBuffer() const;
        ByteArray* getOutputBuffer() const;
        void       setInputBuffer( ByteArray* pInput_data );
        void       setOutputBuffer( ByteArray* pOutput_data );

        void       swapIO( void ); //void swapBuffers();

    protected:

        ByteArray*      _pInput_data;
        ByteArray*      _pOutput_data;
        ProcessorFunc   _processor;

};


#endif // PIPE_H
