/**
 * @file    Pipeline.h
 *
 * @brief   Declaration of Pipeline
 *
 * @note    This example code is free software: you can redistribute it and/or modify it.
 *
 *          This program is provided on an "AS IS" basis without
 *          any warranties in the hope that it will be useful.
 * 
 * Gatis Gaigals, 2024
 */

#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "Pipe.h"

#include <vector>


class Pipeline {

public:
    Pipeline( uint16_t buffersize = 128 );
    Pipeline( ByteArray* pBAin );
    ~Pipeline();

    uint8_t    AddBuffer( int        BufferSize = -1 ); //every buffer can be added only once
    uint8_t    AddBuffer( ByteArray* pByteArray );      //they will get unique uint8_t ID

    StatusCode AddProcessor(    Pipe::ProcessorFunc processor,
                                uint16_t            outputBufferSize = 0 );
    StatusCode AddProcessor(    uint8_t             inputBufferID,
                                Pipe::ProcessorFunc processor,
                                uint8_t             outputBufferID );
    StatusCode AddProcessor(    Pipe::ProcessorFunc processor,
                                ByteArray*          outputBuffer );
    StatusCode AddProcessor(    ByteArray*          inputBuffer,
                                Pipe::ProcessorFunc processor );
    StatusCode AddProcessor(    ByteArray*          inputBuffer,
                                Pipe::ProcessorFunc processor,
                                ByteArray*          outputBuffer );

    ByteArray* getFrontEnd() const;                         //first pipe input
    ByteArray* getBackEnd() const;                          //last pipe output
    ByteArray* getBuffer( uint8_t index ) const;            //from _buffers

    ByteArray* getInputBuffer(  uint8_t PipeIndex ) const;   //input buffer of a pipe
    ByteArray* getOutputBuffer( uint8_t PipeIndex ) const;

    ByteArray* setInputBuffer(  uint8_t PipeIndex, uint8_t BufferIndex );
    ByteArray* setOutputBuffer( uint8_t PipeIndex, uint8_t BufferIndex );
    ByteArray* setInputBuffer(  uint8_t PipeIndex, ByteArray* pByteArray );
    ByteArray* setOutputBuffer( uint8_t PipeIndex, ByteArray* pByteArray );

    StatusCode Sink( uint8_t PipeIndex, char c );
    StatusCode Sink( uint8_t PipeIndex, const char* cstring );
    StatusCode Sink( uint8_t PipeIndex, ByteArray* pByteArray );

    StatusCode processStep( uint8_t i );
    StatusCode processAll( void );

    uint8_t getFaultyPipe( void ) const;
    uint8_t getPipeOffset( void ) const;
    uint8_t getBufferCount( void ) const;
    uint8_t getPipeCount( void ) const;

    void swapIO( void );                                    //swap F-end and B-end (_buffers)
    void swapIO( uint8_t PipeIndex );                       //swap buffers of pipe
    void swapBuffers( uint16_t x, uint16_t y );             //swap buffers in _buffers

    void setErrorHandler( void (*ErrorHandler)( Pipeline* pPipeline, StatusCode ErrorCode ) );

private:

    uint8_t     _faultyPipe         = 0;
    uint8_t     _pipeOffset         = 0;
    uint16_t    _defaultBufferSize  = 128;
    void      (*_ErrorHandler)( Pipeline* pPipeline, StatusCode ErrorCode ) = nullptr;

    std::vector<ByteArray*>         _buffers;
    std::vector<Pipe*>              _pipes;
};

#endif // _PIPELINE_H_
