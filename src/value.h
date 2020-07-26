#ifndef SQUEAKY_VALUE_H_INCLUDED
#define SQUEAKY_VALUE_H_INCLUDED

typedef double Value;

struct value_array {
    long count;
    long capacity;
    Value* values;
};

void value_array_init(struct value_array* array);
void value_array_free(struct value_array* array);
void value_array_append(struct value_array* array, Value value);

void value_print(Value value);
void value_println(Value value);

#endif
