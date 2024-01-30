// Simple dynamic array impl

struct _array
{
    int size;
    char** data;
};

/**
 * @brief A array container
 * 
 */
typedef struct _array array;

/**
 * @brief Create new array
 * 
 * @param arr A array container
 * @return int 0 - susses, 1 - faild
 */
int array_create(array* arr);

/**
 * @brief Return the size of the array
 * 
 * @param arr self
 * @return int size of the array
 */
int array_size(array* arr);

/**
 * @brief Return the address to the real location of the data
 * 
 * @param arr self
 * @return char** array data address
 */
char** array_data(array* arr);

/**
 * @brief Pull the last element
 * 
 * @param arr self
 * @return char* the elment value, NULL - if has faild
 */
char* array_pull(array* arr);

/**
 * @brief Push element into the last of the array
 * 
 * @param arr self
 * @param value the element value
 */
void array_push(array* arr, char* value);

/**
 * @brief Get element value from the array by index
 * 
 * @param arr self
 * @param index index of the element
 * @return char* the element value, NULL - if has faild
 */
char* array_get(array* arr, int index);

/**
 * @brief Set value into index in the array
 * 
 * @param arr self
 * @param index index of the new value
 * @param value the value
 */
void array_set(array* arr, int index, char* value);

/**
 * @brief Free the array
 * 
 * @param arr self
 * @note Can create again after array_free
 */
void array_free(array* arr);
