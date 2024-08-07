/**
 * @file    Pipeline.cpp
 *
 * @brief   Implementation of the Pipeline class
 *
 * @note    This example code is free software: you can redistribute it and/or modify it.
 *
 *          This program is provided by EDI on an "AS IS" basis without
 *          any warranties in the hope that it will be useful.
 * 
 * Gatis Gaigals, 2024
 */


#include "Pipeline.h"


//Constructor with default buffer size
Pipeline::Pipeline( uint16_t buffersize ) :
    _defaultBufferSize( buffersize ) {
    //create the first buffer
    ByteArray* initialBuffer = new ByteArray( _defaultBufferSize );
    buffers.push_back( initialBuffer );
}

//Constructor with a provided ByteArray input buffer
Pipeline::Pipeline( ByteArray* pBAin ) :
    _defaultBufferSize( pBAin->size() ) {
    //do not create the first buffer
    buffers.push_back( pBAin );
}

//Destructor to clean up dynamically allocated objects
Pipeline::~Pipeline() {
    for ( auto buffer : buffers ) {
        delete[] buffer->data();
        delete buffer;
    }
    for ( auto pipe : pipes ) {
        delete pipe;
    }
}

//Add a processor to the pipeline
StatusCode Pipeline::AddProcessor( Pipe::ProcessorFunc processor, int outputBufferSize ) {
    //Check input buffer - if it is reason to continue
    ByteArray* inputBuffer  = buffers.back();
    //if ( ( nullptr == inputBuffer ) || ( nullptr == inputBuffer->data() ) ) {
    if ( !inputBuffer || !inputBuffer->data() ) {
        return StatusCode::ERROR;
    }

    //Create new buffer
    ByteArray* outputBuffer = new ByteArray( ( outputBufferSize < 1 ) ? _defaultBufferSize : outputBufferSize );

    //if ( ( nullptr == outputBuffer ) || ( nullptr == outputBuffer->data() ) ) {
    if ( !outputBuffer || !outputBuffer->data() ) {
        return StatusCode::ERROR;
    }

    //Add the output buffer to the buffers list
    buffers.push_back( outputBuffer );

    //Create a new Pipe with these buffers
    Pipe* newPipe           = new Pipe( inputBuffer, outputBuffer, processor );
    //if ( nullptr == newPipe ) {
    if ( !newPipe ) {
        return StatusCode::ERROR;
    }

    pipes.push_back( newPipe );

    return StatusCode::OK;
}

//Sink data in te buffer
StatusCode Pipeline::Sink( uint8_t bufferIndex, char c ) {

    _skip = bufferIndex;

    //Check if the buffer index is within the valid range
    if ( bufferIndex >= buffers.size() ) {
        return StatusCode::ERROR;  //Invalid buffer index
    }

    //Get the specified buffer
    ByteArray* targetBuffer = buffers[bufferIndex];
    if ( !targetBuffer || !targetBuffer->data() ) {
        return StatusCode::ERROR;  //Invalid or uninitialized buffer
    }

    //Check if there is space to append the character
    if ( targetBuffer->count() >= targetBuffer->size() ) {
        return StatusCode::ERROR;  //Buffer overflow
    }

    //Append the character to the buffer
    targetBuffer->append( c );

    return StatusCode::OK;

}

StatusCode Pipeline::Sink( uint8_t bufferIndex, const char* cstring ) {

    _skip = bufferIndex;

    //Check if the buffer index is within the valid range
    if ( bufferIndex >= buffers.size() ) {
        return StatusCode::ERROR;       //Invalid buffer index
    }

    //Get the target buffer
    ByteArray* targetBuffer = buffers[bufferIndex];

    //Check if the target buffer and its data are valid
    if ( !targetBuffer || !targetBuffer->data() ) {
        return StatusCode::ERROR;       //Invalid buffer or uninitialized data
    }

    //Append each character of the C-string to the buffer
    uint16_t offset = 0;
    uint16_t OffsetLimit = targetBuffer->size() - targetBuffer->count();
    char ch;
    while ( '\0' != ( ch = *( cstring + offset++ ) ) ) {
        if ( offset > OffsetLimit ) {
            return StatusCode::ERROR;   //Failed to append character
        }
        targetBuffer->append( ch );
    }

    return StatusCode::OK;
}

StatusCode Pipeline::Sink( uint8_t bufferIndex, ByteArray* pByteArray ) {

    _skip = bufferIndex;

    //Check if the buffer index is within the valid range
    if ( bufferIndex >= buffers.size() ) {
        return StatusCode::ERROR;       //Invalid buffer index
    }

    //Get the target buffer
    ByteArray* targetBuffer = buffers[bufferIndex];

    //Check if the target buffer and its data are valid
    if ( !targetBuffer || !targetBuffer->data() ) {
        return StatusCode::ERROR;       //Invalid buffer or uninitialized data
    }

    //Calculate how much space is available in the target buffer
    uint16_t space = targetBuffer->size() - targetBuffer->count();

    //Calculate how much data can be appended from the source buffer
    space = pByteArray->count() <= space ? pByteArray->count() : space;

    //Append as much data as possible from pByteArray to targetBuffer
    for ( uint16_t i = 0; i < space; ) {
        targetBuffer->append( pByteArray->at( i++ ) );
    }

    //Determine the status based on whether all data was appended
    if ( space < pByteArray->count() ) {
        return StatusCode::PARTIAL;     //Only part of the data was appended
    }
    return StatusCode::OK;
}

/* Getters for buffers */

ByteArray* Pipeline::getInputBuffer() const {
    return buffers.front();
}

ByteArray* Pipeline::getFrontEnd() const {
    //Return the first buffer, which is the input buffer
    return buffers.front();
}

ByteArray* Pipeline::getOutputBuffer() const {
    return buffers.back();
}

ByteArray* Pipeline::getBackEnd() const {
    //Return the last buffer, which is the output buffer
    return buffers.back();
}

uint8_t Pipeline::getBufferCount() const {
    return buffers.size();
}

ByteArray* Pipeline::getBuffer( uint8_t index ) const {
    if ( index < buffers.size() ) {
        return buffers[index];
    }
    return nullptr;
}

/* Processing functions */

//Count pipes from 1
StatusCode Pipeline::processStep( uint8_t i ) {
    if ( i > 0 && i <= pipes.size() ) {
        _faultyPipe = 0;            //Reset the faulty pipe indicator
        StatusCode status = pipes[i-1]->process();
        if ( status != StatusCode::OK ) {
            _faultyPipe = i;        //Count pipes from 1
        }
        if ( _ErrorHandler ) {
            _ErrorHandler( this, status );
        }
        return status;
    }
    _faultyPipe = i;                //Count pipes from 1
    return StatusCode::ERROR;       //no such a pipe
}

StatusCode Pipeline::processAll() {
    StatusCode  status;
    uint8_t i   = _skip;
    _skip       = 0;
    _faultyPipe = 0;                //Reset the faulty pipe indicator
    for ( ; i < pipes.size(); ++i ) {
        //printf("Processing pipe %d\r\n", i + 1 );
        status = pipes[i]->process();
        //printf("Pipe result: %d\r\n", status );
        if ( status != StatusCode::OK ) {
            _faultyPipe = i + 1;    //Count pipes from 1
            if ( _ErrorHandler ) {
                _ErrorHandler( this, status );
            }
            return status;
        }
    }
    return status;
}

/* Other utility functions */

uint8_t Pipeline::getPipeCount() const {
    return pipes.size();
}

uint8_t Pipeline::getFaultyPipe() const {
    return _faultyPipe;
}

//Takes two buffer indices x and y as arguments.
//Swaps the buffers at positions x and y if both indices are within the bounds of the buffers vector.
void Pipeline::swapBuffers( uint16_t x, uint16_t y ) {
    if ( x < buffers.size() && y < buffers.size() ) {
        std::swap( buffers[x], buffers[y] );
    }
}

//Swaps the first and last buffers in the buffers vector if there are at least two buffers.
void Pipeline::swapIO() {
    if ( buffers.size() >= 2 ) {
        std::swap( buffers.front(), buffers.back() );
    }
}

//Set the error handler
void Pipeline::setErrorHandler( void (*ErrorHandler)( Pipeline* pPipeline, StatusCode ErrorCode ) ) {
    //Handle the error code here
    if ( ErrorHandler ) {
        _ErrorHandler = ErrorHandler;
    }
}
