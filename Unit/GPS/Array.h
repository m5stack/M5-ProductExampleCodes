
#ifndef ARRAY_H
#define ARRAY_H

#include <Arduino.h>

template<typename type>
class Array {
	public:
		Array( type* newArray, int newSize ): array(newArray), arraySize(newSize) {/**/}
		int size() const{
			return arraySize;
		}
		type getMax(){
			type max = array[0];
			for (int i=1; i<arraySize; i++){
				if (max < array[i]){
					max = array[i];
				}
			}
			return max;
		}

		type getMaxIndex(){
			type max = array[0];
			type maxIndex = array[0];
			for (int i=0; i<arraySize; i++){
				if (max <= array[i]){
					max = array[i];
					maxIndex = i;
				}
			}
			return maxIndex;
		}
		
		type getMin(){
			type min = array[0];
			for (int i=1; i<arraySize; i++){
				if (min > array[i]){
					min = array[i];
				}
			}
			return min;
		}

		type getMinIndex(){
			type min = array[0];
			type minIndex = array[0];
			for (int i=0; i<arraySize; i++){
				if (min >= array[i]){
					min = array[i];
					minIndex = i;
				}
			}
			return minIndex;
		}
		
		type getAverage(){
			type sum = 0;
			for (int i=0; i<arraySize; i++){
				sum += array[i];
			}
			return (sum/arraySize);
		}
		
		// subscript operator for non-const objects returns modifiable lvalue
		type &operator[]( int index ) {
			// simple error handling
			if (index>=arraySize){
				return array[arraySize-1];
			}else if (index<0){
				return array[0];
			}else{
				return array[index];
			}
		}		
		// subscript operator for const objects returns rvalue
		type operator[]( int index ) const {
			// simple error handling
			if (index>=arraySize || index<0){
				return 0;
			}else{
				return array[index];
			}
		}
		
	private:
		type* array;
		int arraySize;
};

#endif
