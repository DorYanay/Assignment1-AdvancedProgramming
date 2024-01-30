// encode and decode strings

#ifdef CODEC_BUILD_STATIC
void encode(char* str);
void decode(char* str);
#else
typedef void (*encode_function)(char*);
typedef void (*decode_function)(char*);
#endif
