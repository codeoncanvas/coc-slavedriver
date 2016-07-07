#include "cocKeyValByteManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace coc {


void KeyValByteManager::addPair( char key, uint32_t value )
{
	byteArray.push_back( (Byte) key );

	byteArray.push_back( value );
	byteArray.push_back( value >> 8 );
	byteArray.push_back( value >> 16 );
	byteArray.push_back( value >> 24 );
}

void KeyValByteManager::addPair( char key, double value )
{

	byteArray.push_back( (Byte) key );

	Byte bArray[sizeof( double )] = { 0 };
	memcpy( bArray, &value, sizeof( double ) );

	for ( int i = 0; i < sizeof( double ); i ++ ) {
		byteArray.push_back( bArray[i] );
	}

}

map< char, KeyValByteType > & KeyValByteManager::getMap()
{
	return kvTypeMap;
}

vector<KeyValByteBase *> & KeyValByteManager::getPairs()
{
	return kvPairs;
}

ci::Buffer KeyValByteManager::getBuffer()
{
	ci::Buffer buf( byteArray.size() );
	memcpy( buf.getData(), &byteArray[0], byteArray.size() );
	return buf;
}

void KeyValByteManager::processBuffer( ci::Buffer buffer )
{
	UInt8 result[4];
	Byte dArray[sizeof( double )] = { 0 };

	size_t i = 0, j = 0;
	size_t numElements = buffer.getSize() / sizeof( uint8_t );
	Byte *pBuffer = reinterpret_cast<Byte *>( buffer.getData() );

	for ( i = 0; i < numElements; i ++ ) {
		Byte element = pBuffer[i];

		switch ( kvTypeMap[(char)element]) {
			case KV_DOUBLE:
			{
				KeyValByte<double> *doubleKV = new KeyValByte<double>();
				doubleKV->setKey( element );
				i ++;
				for ( j = 0; j < sizeof( double ); j ++ ) {
					dArray[j] = pBuffer[i + j];
				}
				i += j - 1;
				double doubleVal;
				memcpy( &doubleVal, dArray, sizeof( double ) );
				doubleKV->setValue( doubleVal );
				kvPairs.push_back( doubleKV );
			}

				break;
			case KV_INT32:
			{
				KeyValByte<uint32_t> *intKV = new KeyValByte<uint32_t>();
				intKV->setKey( element );
				i ++;
				for ( j = 0; j < sizeof( uint32_t ); j ++ ) {
					result[j] = pBuffer[i + j];
				}
				i += j - 1;
				uint32_t value = result[0] + (result[1] << 8) + (result[2] << 16) + (result[3] << 24);
				intKV->setValue( (uint32_t) value );
				kvPairs.push_back( intKV );
			}

				break;
		}

	}
}

void KeyValByteManager::clear()
{
	byteArray.clear();

	for (int i=kvPairs.size()-1; i>=0; i--) delete kvPairs[i];//todo: check if not casting creates leak
	kvPairs.clear();
}


}//namespace coc