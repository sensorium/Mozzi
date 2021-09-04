
template<uint8_t BYTES> struct IntegerType {
    // at an odd value, such as 3 bytes? Add one more byte (up to at most 8 bytes)..
    typedef typename IntegerType<(BYTES < 8) ? (BYTES+1) : 8>::unsigned_type unsigned_type;
    typedef typename IntegerType<(BYTES < 8) ? (BYTES+1) : 8>::signed_type signed_type;
};

// These are the specializations for the types that we actually assume to exist:
template<> struct IntegerType<1> {
    typedef uint8_t unsigned_type;
    typedef int8_t signed_type;
};

template<> struct IntegerType<2> {
    typedef uint16_t unsigned_type;
    typedef int16_t signed_type;
};

template<> struct IntegerType<4> {
    typedef uint32_t unsigned_type;
    typedef int32_t signed_type;
};

template<> struct IntegerType<8> {
    typedef uint64_t unsigned_type;
    typedef int64_t signed_type;
};
