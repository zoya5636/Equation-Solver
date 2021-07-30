#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX 100

int charType(char equation[INPUT_MAX], int index);
void convert(char input[INPUT_MAX], int *size, int *numSize);
void operate(char oType, int startP, int endP, int numStart, int numEnd);
void solve(int nEqualPlace, int cEqualPlace, int inputSize, int numSize);

int *numArr;
char *charArr;

const int ALPHA = 1;
const int NUM = 2;
const int OPERATION = 3;
const int OPEN_BRACKET = 4;
const int CLOSED_BRACKET = 5;
const int EQUAL_SIGN = 6;

int findNextNum(int numCount, int numSize) {
    for(int i = (numCount+1); i < numSize; i++) {
        if(numArr[i] != 0) {
            return i;
        }
    }
    return 0;
}

void operate(char oType, int startP, int endP, int numStart, int numEnd) {
    int numCount = numStart-1, result;
    for(int i = startP; i < endP; i++) {
        if(charArr[i] == '$') numCount++;
        if(numCount == numEnd) break;
        if(charArr[i] == oType) {
            int index = findNextNum(numCount, numEnd);
            if(index == 0) return;
            if(oType == '+') {
                result = numArr[numCount] + numArr[index];
            } else if(oType == '-') {
                result = numArr[numCount] - numArr[index];
            } else if(oType == '*') {
                result = numArr[numCount] * numArr[index];
            } else if(oType == '/') {
                result = numArr[numCount] / numArr[index];
            }
            charArr[i] = '@';
            numArr[index] = result;
            numArr[numCount] = 0;
            numCount = index;
        }
    }
}

// returns 0 if any other type
int charType(char equation[INPUT_MAX], int index) {
    if( (equation[index] <= 90 && equation[index] >= 65) ||
        (equation[index] <= 122 && equation[index] >= 96) ) {
        return ALPHA;   
    } else if(equation[index] <= 57 && equation[index] >= 48) {
        return NUM;
    } else if(equation[index] == 43 || equation[index] == 42 || equation[index] == 45 || equation[index] == 47) {
        return OPERATION;
    } else if(equation[index] == 40 || equation[index] == 91 || equation[index] == 123) {
        return OPEN_BRACKET;
    } else if(equation[index] == 41 || equation[index] == 93 || equation[index] == 125) {
        return CLOSED_BRACKET;
    } else if(equation[index] == 61) {
        return EQUAL_SIGN;
    }
    return 0;
}

void convert(char input[INPUT_MAX], int *size, int *numSize) {
    int currNum = -1, numCount = 0, charCount = 0, currState, brackNum = 0, negNum = 0, signs = 0;
    if(input[0] == '-') {
        negNum = 1;
        signs++;
    }
    for(int i = 0; i < (*size); i++) {
        currState = charType(input, i);
        if(currState == ALPHA) {
            charArr[charCount] = input[i];
            charCount++;
        } else if(currState == NUM) {
            if(currNum>(-1)) {
                currNum *= 10;
                currNum += input[i] - 48;
            } else {
                currNum = input[i] - 48;
            }
        } else if(currState == OPERATION) {
            if(i != 0 && charType(input, i-1) == OPEN_BRACKET && charType(input, i+1) == NUM) {
                brackNum = 1;
                if(input[i] == '-') {
                    negNum = 1;
                    signs++;
                }
            } else if(brackNum == 0 && negNum == 0) {
                charArr[charCount] = input[i];
                charCount++;
            }
        } else if(currState == OPEN_BRACKET) {
            if(i != 0 && charType(input, i-1) != OPERATION && charType(input, i+1) != OPERATION) {
                charArr[charCount] = '*';
                charCount++;
            }
        } else if(currState == EQUAL_SIGN) {
            charArr[charCount] = '=';
            charCount++;
        }
        if(currState == NUM && (i == ((*size) - 1) || charType(input, i+1) != NUM)) {
            charArr[charCount] = '$';
            charCount++;
            if(negNum == 1) currNum *= -1;
            numArr[numCount] = currNum;
            numCount++;
            currNum = 1;
            negNum = 0;
            brackNum = 0;
        }
    }
    int *temp;
    temp = numArr;
    numArr = malloc(sizeof(int)*numCount);
    printf("numCount: %d\n", numCount);
    for(int i = 0; i < numCount; i++) {
        numArr[i] = temp[i];
    }
    (*size) = (*size) - ((*numSize) - numCount) - signs;
    (*numSize) = numCount;
}

void printArr(int size, int numSize) {
    printf("start char\n");
    for(int i = 0; i < size; i++) {
        printf("%c\n", charArr[i]);
    }
    printf("end char\n");
    printf("start num\n");
    for(int i = 0; i < numSize; i++) {
        printf("%d\n", numArr[i]);
    }
    printf("end num\n");
}
void solve(int nEqualPlace, int cEqualPlace, int inputSize, int numSize) {
   operate('/', 0, cEqualPlace, 0, nEqualPlace);
   operate('/', cEqualPlace, inputSize, nEqualPlace, numSize); 
   operate('*', 0, cEqualPlace, 0, nEqualPlace);
   operate('*', cEqualPlace, inputSize, nEqualPlace, numSize);
   operate('+', 0, cEqualPlace, 0, nEqualPlace);
   operate('+', cEqualPlace, inputSize, nEqualPlace, numSize);
   operate('-', 0, cEqualPlace, 0, nEqualPlace);
   operate('-', cEqualPlace, inputSize, nEqualPlace, numSize);
}

void isolate(char var) {
    //for(int i = 0; i < inputSize, i++)
}

int main(void) {
    char input[INPUT_MAX];
    int inputSize, numSize = 0, nEqualPlace = 0, cEqualPlace = 0, done = 0;
    printf("Please input your equation (only use +, -, *, /) and do not put spaces or multiple letters in between equation: ");
    fgets(input, INPUT_MAX, stdin);
    inputSize = strlen(input) - 1;
    printf("\n%d\n", inputSize);
    for(int i = 0; i < inputSize; i++) {
        if(input[i] == '=') {
            done = 1;
        }
        if(input[i] != '=' && done == 0) {
            cEqualPlace++;
            if(charType(input, i) == NUM) {
                nEqualPlace++;
            }
        }
        if(charType(input, i) == NUM) {
            numSize++;
        }
    }
    //printf("size: %d, num: %d\n", inputSize, numSize);
    //printf("nEqualPlace: %d\n", nEqualPlace);
    numArr = malloc(sizeof(int)*numSize);
    charArr = malloc(sizeof(char)*inputSize);
    convert(input, &inputSize, &numSize);
    printf("size: %d, num: %d\n", inputSize, numSize);
     //debugging---------------------------------------------
    for(int i = 0; i < inputSize; i++) {
        printf("%c\n", input[i]);
    }
    printArr(inputSize, numSize);
    //--------------------------------------------------------
    solve(nEqualPlace, cEqualPlace, inputSize, numSize);
    printf("\nafter:\n");
    printArr(inputSize, numSize);
    return 0;
}

