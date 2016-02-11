#define PI 3.14159265f

#define MAX(number1,number2) (((number1) > (number2)) ? number1 : number2)
#define MIN(number1,number2) (((number1) > (number2)) ? number2 : number1)
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr;
#define FOR_EACH(i, vector) for(i = 0; i < vector.size(); ++i)
#define FOR_EACH2(vector) for(unsigned int i = 0; i < vector.size(); ++i)
#define CYCLIC_ERASE(vector, index) vector[index] = vector.back(); vector.pop_back();

#define SWAP(number1, number2) decltype(number1) temp = (number1); (number1) = (number2); (number2) = temp;
#define PRINT(text) std::cout << #text << std::endl;

#define TO_RAD(number) (number) * PI / 180
#define TO_DEG(number) (number) * 180 / PI

#define TINYXML_CHECKQUERY(value) (value != 0) ? DL_ASSERT(#value) : DL_PRINT(#value)