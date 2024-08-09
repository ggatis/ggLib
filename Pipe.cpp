/**
 * @file    Pipe.cpp
 *
 * @brief   Implementation of the Pipe class
 *
 * @note    This example code is free software: you can redistribute it and/or modify it.
 *
 *          This program is provided by EDI on an "AS IS" basis without
 *          any warranties in the hope that it will be useful.
 * 
 * Gatis Gaigals, 2024
 */


#include "Pipe.h"
//#include <algorithm>
//in ByteArray.h

//Constructor implementation
Pipe::Pipe( ByteArray* pInput_data, ProcessorFunc processor, ByteArray* pOutput_data )
    : _pInput_data( pInput_data ), _processor( processor ), _pOutput_data( pOutput_data ) {}

//Process method implementation
StatusCode Pipe::process() {
#if EXCEPTIONS_SUPPORTED
        try {
            //Clear the output buffer before processing
            //pOutput_data->clear();
            //Process the input data and store the result in the output data
            return _processor( _pInput_data, _pOutput_data );
        } catch (...) {
            return StatusCode::ERROR;
        }
#else
        //Clear the output buffer before processing
        //pOutput_data->clear();
        //Process the input data and store the result in the output data
        return _processor( _pInput_data, _pOutput_data );
#endif
    }

//Get the input buffer
ByteArray* Pipe::getInputBuffer( void ) const {
    return _pInput_data;
}

//Get the output buffer
ByteArray* Pipe::getOutputBuffer( void ) const {
    return _pOutput_data;
}

//Set the input buffer, even nullptr is allowed
void setInputBuffer( ByteArray* pInput_data ) {
    _pInput_data = pInput_data;
}

//Set the output buffer, even nullptr is allowed
void setOutputBuffer( ByteArray* pOutput_data ) {
    _pOutput_data = pOutput_data;
}


//Swap input and output buffers aka swapBuffers
void Pipe::swapIO( void ) {
    std::swap( _pInput_data, _pOutput_data );
}
