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


//Constructor implementation
Pipe::Pipe( ByteArray* pInput_data, ByteArray* pOutput_data, ProcessorFunc processor )
    : pInput_data( pInput_data ), pOutput_data( pOutput_data ), processor( processor ) {}

//Process method implementation
StatusCode Pipe::process() {
#if EXCEPTIONS_SUPPORTED
        try {
            //Clear the output buffer before processing
            //pOutput_data->clear();
            //Process the input data and store the result in the output data
            return processor( pInput_data, pOutput_data );
        } catch (...) {
            return StatusCode::ERROR;
        }
#else
        //Clear the output buffer before processing
        //pOutput_data->clear();
        //Process the input data and store the result in the output data
        return processor( pInput_data, pOutput_data );
#endif
    }

//Get the input buffer
ByteArray* Pipe::getInputBuffer( void ) const {
    return pInput_data;
}


//Get the output buffer
ByteArray* Pipe::getOutputBuffer( void ) const {
    return pOutput_data;
}

//Swap input and output buffers
void Pipe::swapBuffers( void ) {
    std::swap( pInput_data, pOutput_data );
}
