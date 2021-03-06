#ifndef __INCLUDED_USELESS_CRYPTO_O_FILE_H__
#define __INCLUDED_USELESS_CRYPTO_O_FILE_H__

#include "Useless/System/Assert.h"
#include "Useless/File/OFile.h"
#include "Useless/Util/SPointer.h"

#include <algorithm> // - permutation
#include "Useless/ErrorConfig.h"   // - error config logging

namespace Useless {

template< class Cipher, bool PermuteKey=false >
class USELESS_API CryptoOFile : public OFile
{
public:
    enum { BLOCKSIZE = Cipher::BLOCKSIZE };

    CryptoOFile( const SPointer<OFile> &file, const std::string &key )
        :_file(file), _key(key), _transformation( key )
    {
        //_file->Seek( 0, SEEKSET ); <- nobody says we want to start encryption from the begining of the file
        _cache = _key;
        _block = 0;
        _offset= 0;
        _in = _inBlock.get();
        _out = _outBlock.get();
    }

    ~CryptoOFile() throw()
    {
    }

    int WriteUpto( const void *buffer, int size )
    {
        char *buf = (char*)buffer;
        for ( int n=size; n>0; )
        {
            int bytes_in_buffer = BLOCKSIZE-_offset;
            if ( n < bytes_in_buffer )
            {
                memcpy( _in+((int)_offset), buf, n );
                _offset += n;
                buf += n;
                break;
            }
            else
            {
                if ( bytes_in_buffer > 0 )
                {
                    memcpy( _in+((int)_offset), buf, bytes_in_buffer );
                    _offset += bytes_in_buffer;
                    buf += bytes_in_buffer;
                    n -= bytes_in_buffer;
                    ProcessNext();
                }
            }
        }
        return size;
    }

    void Close()
    {
        if (_offset > 0)
        {
            memset( _in+((int)_offset), 0, BLOCKSIZE-_offset );
            ProcessNext();
        }
        if ( _file.Unique() ) { _file->Close(); }
    }

    void Seek( Types::QUAD offset, SeekMode mode )
    {
        int result_offset;
        switch( mode )
        {
            case SEEKSET:
            {
                assert( offset>0 );
                _cache = _key;
                result_offset = (int)(offset % BLOCKSIZE); 
                //_block=0;
                _block = (int)(offset / BLOCKSIZE);
                if (PermuteKey)
                {
                    while( (offset-=BLOCKSIZE) >= 0 )
                    {
                            std::next_permutation( _cache.begin(), _cache.end() );
                        //++_block;
                    }
                }
                _file->Seek( _block*BLOCKSIZE, SEEKSET ); --_block;
                ProcessNext();
                break;
            }

            case SEEKCUR:
            {
                int blocks = 0;
                if ( offset > 0 )
                {
                    result_offset = (int)((((Types::QUAD)_offset) + offset) % BLOCKSIZE);
                    while( (offset-=BLOCKSIZE) >= 0 )
                    {
                        if (PermuteKey)
                            std::next_permutation( _cache.begin(), _cache.end() );
                        ++_block; ++blocks;
                    }
                    _file->Seek( blocks*BLOCKSIZE, SEEKCUR );
                }
                else
                {
                    result_offset = (int)((((Types::QUAD)_offset) - offset) % BLOCKSIZE);
                    while( (offset+=BLOCKSIZE) < BLOCKSIZE )
                    {
                        if (PermuteKey)
                            std::prev_permutation( _cache.begin(), _cache.end() );
                        --_block; ++blocks;
                    }
                    _file->Seek( -blocks*BLOCKSIZE, SEEKCUR );
                }
                if ( blocks>0 )
                {
                    --_block;
                    ProcessNext();
                }
                break;
            }

            case SEEKEND:
            {
                assert( offset<0 );
                offset = -offset;
                Types::QUAD abs_offset = _file->GetLen() - offset;
                Types::QUAD total_blocks = _file->GetLen() / BLOCKSIZE;
                _cache = _key;
                result_offset = (int)(abs_offset % BLOCKSIZE);
                _block=0;
                while( (abs_offset-=BLOCKSIZE) >= 0 )
                {
                    if (PermuteKey)
                        std::next_permutation( _cache.begin(), _cache.end() );
                    ++_block;
                }
                _file->Seek( -(total_blocks-_block)*BLOCKSIZE, SEEKEND ); --_block;
                ProcessNext();
            }
        }

        if ( _block < 0 )
        {
            ProcessNext();
        }
        _offset = result_offset;
    }


    Types::QUAD GetPos()
    {
        return (_block*BLOCKSIZE + ((int)_offset));
    }

    Types::QUAD GetLen()
    {
        return _file->GetLen();
    }

    const std::string &GetName()
    {
        return _file->GetName();
    }

private:
    void ProcessNext()
    {
        if ( PermuteKey )
        {
            // generate next key
            std::next_permutation( _cache.begin(), _cache.end() );
            _transformation = Cipher( _cache );
        }
        // process block
        _transformation.ProcessBlock( _in, _out );
        _offset = 0;
        ++_block;

        // write block into file
        int wrlen = _file->WriteUpto( _out, BLOCKSIZE );
        if ( wrlen != BLOCKSIZE )
        {
            APP_ERROR( Error("Couldn't write block of %i bytes", (int)BLOCKSIZE), "CryptoOFile" );
        }
    }

    SPointer<OFile> _file;
    std::string     _key;
    std::string     _cache;
    int             _block;
    char            _offset;
    Cipher          _transformation;

    struct Block
    {
        long _data[ BLOCKSIZE/ sizeof(long) + 1 ];
        char *get() const { return ((char *)_data); }
    };

    char *_in;
    char *_out;

    Block _inBlock;
    Block _outBlock;
};

};//namespace Useless
#endif//__INCLUDED_USELESS_CRYPTO_O_FILE_H__
