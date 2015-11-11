#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

// enumeration 'enumParameterType' is corresponding parameter_type member UniDbParameter
// 0 - boolean, 1 - integer, 2 - double, 3 - string, 4 - int+int array, 5 - int array, 6 - double array
enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType, IntArrayType, DoubleArrayType, ErrorType = 999};

struct IIStructure
{
    int int_1;
    int int_2;
};//__attribute__((packed));

#endif // DB_STRUCTURES_H
