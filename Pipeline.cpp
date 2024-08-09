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
    _buffers.push_back( initialBuffer );
}

//Constructor with a provided ByteArray input buffer
Pipeline::Pipeline( ByteArray* pBAin ) :
    _defaultBufferSize( pBAin->size() ) {
    //do not create the first buffer
    _buffers.push_back( pBAin );
}

//Destructor to clean up dynamically allocated objects
Pipeline::~Pipeline() {
    for ( auto buffer : _buffers ) {
        delete[] buffer->data();
        delete buffer;
    }
    for ( auto pipe : _pipes ) {
        delete pipe;
    }
}

/* Add bufers */

//every buffer will get unique uint8_t ID
//they are not checked on uniquity
//the 0 size buffer is ok but better use nullptr buffers
//in order to avoid generation of the 0 size ByteArray

//Add a buffer using the default size or the specified size
uint8_t    AddBuffer( int BufferSize ) {
    
    _faultyPipe = 0;

    uint16_t outputBufferSize = ( BufferSize < 0 ) ? _defaultBufferSize : (uint16_t)BufferSize;

    ByteArray* outputBuffer;
    if ( outputBufferSize ) {
        outputBuffer = new ByteArray( outputBufferSize );
        if ( !outputBuffer || !outputBuffer->data() ) {
            //if AddBuffer returns 0, check _faultyPipe: if it returns _pipes.size() + 1,
            //there was an error
            _faultyPipe = _pipes.size() + 1;
            return 0;
        }
    } else {
        outputBuffer = nullptr;
    }

    _buffers.push_back( outputBuffer );
    return _buffers.size() - 1;

}

uint8_t    AddBuffer( ByteArray* pByteArray ) {
    _faultyPipe = 0;
    _buffers.push_back( pByteArray );
    return _buffers.size() - 1;
}


/* Add processors */

//Add a processor to the pipeline specifying only the output buffer size
StatusCode Pipeline::AddProcessor( Pipe::ProcessorFunc processor, int outputBufferSize ) {

    //generate nullptr input buffer on demand
    if ( !_buffers.size() ) {
        _buffers.push_back( nullptr );
    }

    //input buffer can be nullptr
    ByteArray* inputBuffer  = _buffers.back();

    uint8_t    outputBufferIndex = AddBuffer( outputBufferSize );
    if ( !outputBufferIndex && _faultyPipe == ( _pipes.size() + 1 ) ) {
        return StatusCode::ERROR;
    }
    
    //Get the buffer
    ByteArray* outputBuffer = _buffers[outputBufferIndex];

    //Create a new Pipe with these buffers
    Pipe* newPipe           = new Pipe( inputBuffer, processor, outputBuffer );
    if ( !newPipe ) {
        return StatusCode::ERROR;
    }

    _pipes.push_back( newPipe );

    return StatusCode::OK;

}

StatusCode AddProcessor(
    uint8_t inputBufferID, Pipe::ProcessorFunc processor, uint8_t outputBufferID ) {

    if ( inputBufferID  >= _buffers.size() ) {
        return StatusCode::ERROR;
    }
    if ( outputBufferID >= _buffers.size() ) {
        return StatusCode::ERROR;
    }

    ByteArray* inputBuffer  = _buffers[inputBufferID];
    ByteArray* outputBuffer = _buffers[outputBufferID];

    //Create a new Pipe with these buffers
    Pipe* newPipe           = new Pipe( inputBuffer, processor, outputBuffer );
    if ( !newPipe ) {
        return StatusCode::ERROR;
    }

    _pipes.push_back( newPipe );

    return StatusCode::OK;

}

StatusCode AddProcessor(
    ByteArray* inputBuffer, Pipe::ProcessorFunc processor, ByteArray* outputBuffer ) {

    //Create a new Pipe with these buffers
    Pipe* newPipe           = new Pipe( inputBuffer, processor, outputBuffer );
    if ( !newPipe ) {
        return StatusCode::ERROR;
    }

    _pipes.push_back( newPipe );

    return StatusCode::OK;

}


/* Getters for _buffers */

ByteArray* Pipeline::getFrontEnd() const {
    //Return the first buffer, which can be the input buffer
    return _buffers.front();
}

ByteArray* Pipeline::getBackEnd() const {
    //Return the last buffer, which can be the output buffer
    return _buffers.back();
}

ByteArray* Pipeline::getBuffer( uint8_t index ) const {
    if ( index < _buffers.size() ) {
        return _buffers[index];
    }
    return nullptr;
}


ByteArray* Pipeline::getInputBuffer( uint8_t PipeIndex ) const {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        return _pipes[PipeIndex-1]->getInputBuffer();
    }
    return nullptr;
}

ByteArray* Pipeline::getOutputBuffer( uint8_t PipeIndex ) const {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        return _pipes[PipeIndex-1]->getOutputBuffer();
    }
    return nullptr;
}


ByteArray* Pipeline::setInputBuffer( uint8_t PipeIndex, uint8_t BufferIndex ) {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        if ( BufferIndex < _buffers.size() ) {
            ByteArray* pByteArray = _buffers[BufferIndex];
            _pipes[PipeIndex-1]->setInputBuffer( pByteArray );
            return pByteArray;
        }
    }
    return nullptr;
}

ByteArray* Pipeline::setOutputBuffer( uint8_t PipeIndex, uint8_t BufferIndex ) {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        if ( BufferIndex < _buffers.size() ) {
            ByteArray* pByteArray = _buffers[BufferIndex];
            _pipes[PipeIndex-1]->setOutputBuffer( pByteArray );
            return pByteArray;
        }
    }
    return nullptr;
}

uint8_t    Pipeline::setInputBuffer( uint8_t PipeIndex, ByteArray* pByteArray ) {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        _pipes[PipeIndex-1]->setInputBuffer( pByteArray );
    }
    
    for ( uint16_t i = 0; i < _buffers.size(); ++i ) {
        if ( _buffers[i] == pByteArray ) {
            return i;
        }
    }
    
    _buffers.push_back( pByteArray );
    return _buffers.size() - 1;
}

uint8_t    Pipeline::setOutputBuffer( uint8_t PipeIndex, ByteArray* pByteArray ) {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        _pipes[PipeIndex-1]->setOutputBuffer( pByteArray );
    }
    
    for ( uint16_t i = 0; i < _buffers.size(); ++i ) {
        if ( _buffers[i] == pByteArray ) {
            return i;
        }
    }
    
    _buffers.push_back( pByteArray );
    return _buffers.size() - 1;
}

/* Sink data in the Pipes */

StatusCode Pipeline::Sink( uint8_t PipeIndex, char c ) {

    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {

        _pipeOffset = PipeIndex - 1;

        //Get the specified buffer
        ByteArray* targetBuffer = _pipes[_pipeOffset]->getInputBuffer();
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

    return StatusCode::ERROR;

}

StatusCode Pipeline::Sink( uint8_t PipeIndex, const char* cstring ) {

    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {

        _pipeOffset = PipeIndex - 1;

        //Get the specified buffer
        ByteArray* targetBuffer = _pipes[_pipeOffset]->getInputBuffer();
        if ( !targetBuffer || !targetBuffer->data() ) {
            return StatusCode::ERROR;  //Invalid or uninitialized buffer
        }

        //Check if there is space to append the character
        if ( targetBuffer->count() >= targetBuffer->size() ) {
            return StatusCode::ERROR;  //Buffer overflow
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

    return StatusCode::ERROR;

}

StatusCode Pipeline::Sink( uint8_t PipeIndex, ByteArray* pByteArray ) {

    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {

        _pipeOffset = PipeIndex - 1;

        //Get the specified buffer
        ByteArray* targetBuffer = _pipes[_pipeOffset]->getInputBuffer();
        if ( !targetBuffer || !targetBuffer->data() ) {
            return StatusCode::ERROR;  //Invalid or uninitialized buffer
        }

        //Check if there is space to append the character
        if ( targetBuffer->count() >= targetBuffer->size() ) {
            return StatusCode::ERROR;  //Buffer overflow
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

    return StatusCode::ERROR;

}

/* Processing functions */

//Count pipes from 1
StatusCode Pipeline::processStep( uint8_t i ) {
    if ( i > 0 && i <= _pipes.size() ) {
        _faultyPipe = 0;            //Reset the faulty pipe indicator
        StatusCode status = _pipes[i-1]->process();
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
    //take into account _pipeOffset and reset it
    uint8_t i   = _pipeOffset;
    _pipeOffset       = 0;
    //Reset the faulty pipe indicator
    _faultyPipe = 0;
    for ( ; i < _pipes.size(); ++i ) {
        //printf("Processing pipe %d\r\n", i + 1 );
        status = _pipes[i]->process();
        //printf("Pipe result: %d\r\n", status );
        switch ( status ) {
        case StatusCode::OK:
            break;
    
        case StatusCode::PENDING:
        case StatusCode::PARTIAL:
        case StatusCode::ERROR:
            _faultyPipe = i + 1;    //Count pipes from 1
            if ( _ErrorHandler ) {
                _ErrorHandler( this, status );
            }
            return status;

        case StatusCode::REPEAT:    //finish time quant, repeat later again
            _pipeOffset = i;        //first pipe offset
            return status;

        case StatusCode::NEXT:      //finish time quant, continue later with next
            _pipeOffset = i + 1;    //first pipe offset
            return status;
    
        default:
            // Handle unexpected status
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

uint8_t Pipeline::getFaultyPipe() const {
    return _faultyPipe;
}

uint8_t Pipeline::getBufferCount() const {
    return _buffers.size();
}

uint8_t Pipeline::getPipeCount() const {
    return _pipes.size();
}


//Takes two buffer indices x and y as arguments.
//Swaps the buffers at positions x and y if both indices are within the bounds of the _buffers vector.
void Pipeline::swapBuffers( uint16_t x, uint16_t y ) {
    if ( x < _buffers.size() && y < _buffers.size() ) {
        std::swap( _buffers[x], _buffers[y] );
    }
}

//Swaps the first and last buffers in the _buffers vector if there are at least two buffers.
void Pipeline::swapIO( void ) {
    if ( _buffers.size() >= 2 ) {
        std::swap( _buffers.front(), _buffers.back() );
    }
}

//Count pipes from 1
void swapIO( uint8_t PipeIndex ) {
    if ( PipeIndex > 0 && PipeIndex <= _pipes.size() ) {
        _pipes[PipeIndex-1]->swapIO();
    }
}


//Set the error handler
void Pipeline::setErrorHandler( void (*ErrorHandler)( Pipeline* pPipeline, StatusCode ErrorCode ) ) {
    //Handle the error code here
    if ( ErrorHandler ) {
        _ErrorHandler = ErrorHandler;
    }
}
