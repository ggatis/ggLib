/**
 * @file    CircularBuffer.h
 *
 * @brief   Declaration of class CircularBuffer
 *
 * @note    This example code is free software: you can redistribute it and/or modify it.
 *
 *          This program is provided by EDI on an "AS IS" basis without
 *          any warranties in the hope that it will be useful.
 * 
 * Gatis Gaigals, 2024
 */

#ifndef _CircularBuffer_H_
#define _CircularBuffer_H_

#include <string>   //cpp string
#include <stdint.h>
//#include <stdio.h>  //c printf

#include "ByteArray.h"

/**
 * @brief   The CircularBuffer class provides methods for CircularBuffer.
 */

class CircularBuffer : public ByteArray {
    public:

                        CircularBuffer( uint16_t size );
//                      CircularBuffer( uint16_t repeats, char c );
//                      CircularBuffer( uint16_t size, uint8_t* dataptr );
//                      CircularBuffer( uint16_t size, uint16_t filled, uint8_t* dataptr );
//                      CircularBuffer( const std::string& aString );
//                      CircularBuffer( std::string&& aString );
//                      CircularBuffer( const CircularBuffer& other );
//                      CircularBuffer( CircularBuffer&& other ) noexcept;

//                     ~CircularBuffer( void );       //no need

//      CircularBuffer& operator = ( const CircularBuffer& other );
//      CircularBuffer& operator = ( CircularBuffer&& other ) noexcept;

        void            clear(  void );

//      void            extendo( void );
//      uint16_t        update_count( uint16_t newcount );

        bool            isEmpty( void ) const;
        bool            isFull( void ) const;

        CircularBuffer  put(    uint8_t abyte );
        CircularBuffer  putU16( uint16_t aword );
        CircularBuffer  putU32( uint32_t aqword );
        CircularBuffer  append( uint8_t abyte );
        CircularBuffer  append( const char* cstring );

        uint8_t         at(     uint16_t index ) const;
        uint8_t         at(     int index ) const;
        uint8_t         get(    void );
        uint16_t        getU16( void );
        uint32_t        getU32( void );

//        CircularBuffer   mid( uint16_t index, int size ) const;

        CircularBuffer  chop(   int n );

        void            print(  void ) const;

    private:

        uint16_t _head = 0;
        uint16_t _tail = 0;

        inline void add( uint8_t item ) {
            ByteArray::data()[_head] = item;
            if ( ByteArray::size() == ByteArray::count() ) {
                //Advance tail if buffer is full
                //_tail = ( _tail + 1 ) % ByteArray::size();  
                ++_tail;
                if ( _tail >= ByteArray::size() ) {
                    _tail = _tail - ByteArray::size();
                }
            } else {
                update_count( ByteArray::count() + 1 );
            }
            //Move head to next position
            //_head = ( _head + 1 ) % ByteArray::size();
            ++_head;
            if ( _head >= ByteArray::size() ) {
                _head = _head - ByteArray::size();
            }
        }

};

#endif // _CircularBuffer_H_
