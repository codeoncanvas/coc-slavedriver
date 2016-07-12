#include "cinder/Buffer.h"

//! \brief Class to help manage key value pairs as buffer of bytes for sending over network.

namespace coc {

//-------------------------------------------------------

class KeyValByteBase {

public:
	void setKey( char c ) { key = c; };
	char getKey() { return key; };

private:
	char key;

};//class KeyValByteBase

template<typename T>
class KeyValByte : public KeyValByteBase {

public:

	void setValue( T v ) { value = v; };
	T getValue() { return value; };

private:
	T value;

};//class KeyValByte

enum KeyValByteType {
	KV_INT32,
	KV_DOUBLE
};//enum KeyValByteType

//-------------------------------------------------------

class KeyValByteManager {

public:

	//! Add pair with int32 value
	void addPair( char key, uint32_t value );

	//! Add pair with double value
	void addPair( char key, double value );

	//! Turn bytes into pairs
	void processBytes( const char * _start, size_t _length );

	//! Turn buffer into pairs
	void processBuffer( ci::BufferRef buffer );

	//! Get map of types for keys
	std::map< char, KeyValByteType > & getMap();

	//! Get key value pairs
	std::vector<KeyValByteBase *> & getPairs();

	//! Return pairs as buffer
	ci::BufferRef getBuffer();

	//! Clear byte array and pairs
	void clear();

private:

	std::vector<Byte> 					byteArray;
	std::vector<KeyValByteBase *> 		kvPairs;
	std::map< char, KeyValByteType >	kvTypeMap;

};//class KeyValByteManager

}//namespace coc