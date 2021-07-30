#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX 100

int charType(char equation[INPUT_MAX], int index);
void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, 
             int *numAr1, int *numAr2, char *cAr1, char *cAr2);
void operate(char oType, int *numArr, char *charArr, int inputSize, int numSize);
void simplify(int inputSize1, int inputSize2, int numSize1, int numSize2, int *leftNumArr, int *rightNumArr,
              char *leftCharArr, char *rightCharArr);
int findNextNum(int numCount, int numSize, int *numArr);
void move(int *numArr, int *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite);
int solve(char var, int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, int *leftNumArr, int *rightNumArr,
           char *leftCharArr, char *rightCharArr);
void displayResult(int inputSize, int numSize, int *numArr, char *charArr, char var);

const int ALPHA = 1;
const int NUM = 2;
const int OPERATION = 3;
const int OPEN_BRACKET = 4;
const int CLOSED_BRACKET = 5;
const int EQUAL_SIGN = 6;

char operations[4][2] = {
    {'/', '*'},
    {'*', '/'},
    {'+', '-'},
    {'-', '+'}
};

int findNextNum(int numCount, int numSize, int *numArr) {
    for(int i = (numCount+1); i < numSize; i++) {
        if(numArr[i] != 0) {
            return i;
        }
    }
    return 0;
}
void operate(char oType, int *numArr, char *charArr, int inputSize, int numSize) {
    int numCount = -1, result;
    for(int i = 0; i < inputSize; i++) {
        if(charArr[i] == '$') numCount++;
        if(numCount == numSize) break;
        if(charArr[i] == oType) {
            int index = findNextNum(numCount, numSize, numArr);
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

void updateVars(int numCount, int *inputSize, int *numSize, int signs) {
    (*inputSize) = (*inputSize) - ((*numSize) - numCount) - signs;
    (*numSize) = numCount;
}

void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, int *leftNumArr, int *rightNumArr,
             char *leftCharArr, char *rightCharArr) {
    int currNum = -1, numCount = 0, charCount = 0, currState, brackNum = 0, negNum = 0, signs = 0,
        size = ((*inputSize1) + (*inputSize2) + 1);
    int *currNumArr = leftNumArr;
    char *currCharArr = leftCharArr;
    if(input[0] == '-') {
        negNum = 1;
        signs++;
    }
    for(int i = 0; i < size; i++) {
        if(input[i] == '=') {
            currNumArr = rightNumArr;
            currCharArr = rightCharArr;
            updateVars(numCount, inputSize1, numSize1, signs);
            numCount = 0;
            charCount = 0;
            signs = 0;
            if(input[i+1
            ] == '-') {
                negNum = 1;
                signs++;
            }
            continue;
        }
        currState = charType(input, i);
        if(currState == ALPHA) {
            currCharArr[charCount] = input[i];
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
                currCharArr[charCount] = input[i];
                charCount++;
            }
        } else if(currState == OPEN_BRACKET) {
            if(i != 0 && charType(input, i-1) != OPERATION && charType(input, i+1) != OPERATION) {
                currCharArr[charCount] = '*';
                charCount++;
            }
        } else if(currState == EQUAL_SIGN) {
            currCharArr[charCount] = '=';
            charCount++;
        }
        if(currState == NUM && (i == (size - 1) || charType(input, i+1) != NUM)) {
            currCharArr[charCount] = '$';
            charCount++;
            if(negNum == 1) currNum *= -1;
            currNumArr[numCount] = currNum;
            numCount++;
            currNum = -1;
            negNum = 0;
            brackNum = 0;
        }
    }
    updateVars(numCount, inputSize2, numSize2, signs);
}

void printArr(int numSize1, int numSize2, int inputSize1, int inputSize2, int *leftNumArr, int *rightNumArr,
              char *leftCharArr, char *rightCharArr) {
    printf("left num start\n");
    for(int i = 0; i < numSize1; i++) {
        printf("%d\n", leftNumArr[i]);
    }
    printf("left num ends\n");
    printf("right num start\n");
    for(int i = 0; i < numSize2; i++) {
        printf("%d\n", rightNumArr[i]);
    }
    printf("right num ends\n");
    printf("left char start\n");
    for(int i = 0; i < inputSize1; i++) {
        printf("%c\n", leftCharArr[i]);
    }
    printf("left char ends\n");
    printf("right char start\n");
    for(int i = 0; i < inputSize2; i++) {
        printf("%c\n", rightCharArr[i]);
    }
    printf("right char ends\n");
}
void simplify(int inputSize1, int inputSize2, int numSize1, int numSize2, int *leftNumArr, int *rightNumArr,
              char *leftCharArr, char *rightCharArr) {
    for(int i = 0; i < 4; i++) {
        operate(operations[i][0], leftNumArr, leftCharArr, inputSize1, numSize1);
        operate(operations[i][0], rightNumArr, rightCharArr, inputSize2, numSize2); 
    }
    /*
    operate('/', leftNumArr, leftCharArr inputSize1, numSize1);
    operate('/', rightNumArr, rightCharArr inputSize2, numSize2);
    operate('*', leftNumArr, leftCharArr inputSize1, numSize1);
    operate('*', rightNumArr, rightCharArr inputSize2, numSize2);
    operate('+', leftNumArr, leftCharArr inputSize1, numSize1);
    operate('+', rightNumArr, rightCharArr inputSize2, numSize2);
    operate('-', leftNumArr, leftCharArr inputSize1, numSize1);
    operate('-', rightNumArr, rightCharArr inputSize2, numSize2);
     */
}

void addElement(int *numArr, char *charArr, int *size, int nElement, char cElement) {
    (*size)++;
    if(nElement == 0) {
        char *temp = charArr;
        for(int i = 0; i < ((*size)-1); i++) {
            charArr[i] = temp[i];
        }
        charArr[(*size)-1] = cElement;
    } else {
        int *temp = numArr;
        for(int i = 0; i < ((*size)-1); i++) {
            numArr[i] = temp[i];
        }
        numArr[(*size)-1] = nElement;
    }
}

void move(int *numArr, int *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite) {
    int numCount = -1, index, moveNum;
    char moveChar;
    for(int i = 0; i < (*inputSize); i++) {
        if(charArr[i] == '$') numCount++;
        if(charArr[i] == oType) {
            if(charArr[i+1] == '$' || (charArr[i-1] == '$' && (oType == '+' || oType == '*'))) {
                if(charArr[i+1] == '$') {
                    index = findNextNum(numCount, (*numSize), numArr);
                    moveNum = numArr[index];
                    numArr[index] = 0;
                } else {
                    index = findNextNum(numCount-1, (*numSize), numArr);
                    moveNum = numArr[index];
                    numArr[index] = 0;
                }
                charArr[i] = '@';
                /*for(int i = 0; i < 10; i++) {
                    printf("char: %c  oChar: %c\n", charArr[i], oCharArr[i]);
                }*/
                addElement(oNumArr, NULL, oNumSize, moveNum, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, '$');
            } else if(charType(charArr, i) == ALPHA && charArr[i] != var) {
                moveChar = charArr[i];
                charArr[i] = '@';
                addElement(NULL, charArr, inputSize, 0, opposite);
                addElement(NULL, charArr, inputSize, 0, moveChar);
            }
        }
    }
}

/*
 * what if var doesn't exist?
 */
int solve(char var, int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, int *leftNumArr, int *rightNumArr,
           char *leftCharArr, char *rightCharArr) {
    int *numArr = rightNumArr, *oNumArr = leftNumArr, *numSize = numSize2, *oNumSize = numSize1,
        *inputSize = inputSize2, *oInputSize = inputSize1, leftRight = 1;
    char *charArr = rightCharArr, *oCharArr = leftCharArr;
    for(int i = 0; i < (*inputSize1); i++) {
        if(leftCharArr[i] == var) {
            numArr = leftNumArr;
            charArr = leftCharArr;
            oNumArr = rightNumArr;
            oCharArr = rightCharArr;
            inputSize = inputSize1;
            oInputSize = inputSize2;
            numSize = numSize1;
            oNumSize = numSize2;
            leftRight = 0;
        }
    }
    for(int i = 0; i < 4; i++) {
        move(numArr, oNumArr, charArr, oCharArr, numSize, oNumSize, inputSize, oInputSize, var, operations[i][0], operations[i][1]);
    }
    return leftRight;
}

void displayResult(int inputSize, int numSize, int *numArr, char *charArr, char var) {
    int numCount = -1;
    printf("%c = ", var);
    for(int i = 0; i < inputSize; i++) {
        if(charArr[i] == '$') {
            numCount++;
            if(numArr[numCount] != 0) {
                printf("%d ", numArr[numCount]);
            }
        } else if (charArr[i] != '@') {
            printf("%c ", charArr[i]);
        }
    }
}

int main(void) {
    char input[INPUT_MAX], leftCharArr[INPUT_MAX], rightCharArr[INPUT_MAX], var;
    int inputSize1 = 0, inputSize2 = 0, numSize1 = 0, numSize2 = 0, done = 0, cType, leftNumArr[INPUT_MAX], rightNumArr[INPUT_MAX],
        leftRight; 
    printf("Please input your equation (only use +, -, *, /) and do not put spaces or multiple letters in between equation: ");
    fgets(input, INPUT_MAX, stdin);
    printf("What variable would you like to isolate? (case-sensitive): ");
    scanf("\n%c", &var);
    printf("\nvar: %c\n", var);
    inputSize1 = strlen(input) - 1;
    printf("\n%d\n", inputSize1);
    for(int i = 0; i < inputSize1; i++) {
        cType = charType(input, i);
        if(input[i] == '=') {
            done = 1;
        } else if(done == 0) {
            if(cType == NUM) numSize1++;
        } else {
            inputSize2++;
            if(cType == NUM) numSize2++;
        }
        
    }
    inputSize1 = inputSize1 - inputSize2 - 1;
    printf("\n\nsize1 = %d, size2 = %d:\n", inputSize1, inputSize2);
    //printf("size: %d, num: %d\n", inputSize, numSize);
    //printf("nEqualPlace: %d\n", nEqualPlace);
    convert(input, &numSize1, &numSize2, &inputSize1, &inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
     //debugging---------------------------------------------
    /*for(int i = 0; i < (inputSize1+inputSize2); i++) {
        printf("%c\n", input[i]);
    }*/
    //--------------------------------------------------------
    printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    simplify(inputSize1, inputSize2, numSize1, numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    printf("\nafter simplifying:\n");
    printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    leftRight = solve(var, &inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    printf("\nafter solving:\n");
    printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    printf("\n\nsize1 = %d, size2 = %d:\n", inputSize1, inputSize2);
    simplify(inputSize1, inputSize2, numSize1, numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    printf("\nafter again:\n");
    printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
    int *numArr = leftNumArr, inputSize = inputSize1, numSize = numSize1;
    char *charArr = leftCharArr;
    if(leftRight == 0) {
        numArr = rightNumArr;
        charArr = rightCharArr;
        numSize = numSize2;
        inputSize = inputSize2;
    }
    displayResult(inputSize, numSize, numArr, charArr, var);
    return 0;
}

