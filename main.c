#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INPUT_MAX 100

int charType(char equation[INPUT_MAX], int index);
void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, 
             double *numAr1, double *numAr2, char *cAr1, char *cAr2);
void operate(char oType, double *numArr, char *charArr, int *inputSize, int *numSize, int order);
void simplify(int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
              char *leftCharArr, char *rightCharArr, int start);
int findNextNum(int numCount, int numSize, double *numArr);
int move(double *numArr, double *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite);
int solve(char var, int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
           char *leftCharArr, char *rightCharArr);
void displayResult(int inputSize, int numSize, double *numArr, char *charArr, char var);
int errorCheck(char *input, int *inputSize, char var);
int numInChar(char *arr, int index, int inputSize);
void shiftArray(char *input1, double *input2, int *inputSize, int index);

const int ALPHA = 1;
const int NUM = 2;
const int OPERATION = 3;
const int OPEN_BRACKET = 4;
const int CLOSED_BRACKET = 5;
const int EQUAL_SIGN = 6;
const int EXPONENT = 7;

const int DEBUG = 1;
int OPER_NUM = 5;

char operations[5][2] = {
    {'^', '^'},
    {'/', '*'},
    {'*', '/'},
    {'+', '-'},
    {'-', '+'}
};

// -1 means nothing found
int findNextNum(int numCount, int numSize, double *numArr) {
    for(int i = (numCount+1); i < numSize; i++) {
        if(numArr[i] != 0) {
            return i;
        }
    }
    return -1;
}

int numInChar(char *arr, int index, int inputSize) {
    int count = -1;
    for(int i = 0; i < inputSize; i++) {
        if(arr[i] == '$') count++;
        if(count == index) {
            return i;
        }
    }
    return -1;
}

// returns 1 if valid
int elementInArr(char oType, char *arr, int index) {
    int end;
    /*{'^', '^'},
    {'/', '*'},
    {'*', '/'},
    {'+', '-'},
    {'-', '+'}*/
    if(oType == '^') return 1;
    else if(oType == '/' || oType == '*') end = 1;
    else if(oType == '-' || oType == '+') end = 3;
    for(int i = 0; i < end; i++) {
        if(arr[index] == operations[i][0]) return 0;
        printf("&&&&&&&&&&&&&&&&&&&&     ARR[INDEX] = %d, OPERATIONS[I] = %d\n", arr[index], operations[i][0]);
    }
    return 1;
}

// -1 is valid, 1 is not
int validOperation(char *arr, int index1, int index2, int inputSize, char oType, int order) {
   int cIndex1 = numInChar(arr, index1, inputSize), cIndex2 = numInChar(arr, index2, inputSize), found = 0;
   if(order == 1) {
       if(oType != '^' && (arr[cIndex1-1] == '^' || arr[cIndex1+1] == '^' || arr[cIndex2-1] == '^' || arr[cIndex2+1] == '^')) return -1;
   }
   if(cIndex1 == -1 || cIndex2 == -1) return -1;
   for(int i = (cIndex1+1); i < cIndex2; i++) {
       if(arr[i] != '@' && arr[i] != oType) return -1;
   }
   if(order == 1) {
       for(int i = (cIndex1+1); i > cIndex2; i--) {
           if(arr[i] == '$') found = 1;
           if((elementInArr(oType, arr, i) == 0) && found == 0) return -1;
       }
       found = 0;
       for(int i = (cIndex2+1); i < inputSize; i++) {
           if(arr[i] == '$') found = 1;
           if((elementInArr(oType, arr, i) == 0) && found == 0) return -1;
       }
   }
   return 1;
}

//$ + a + $

void operate(char oType, double *numArr, char *charArr, int *inputSize, int *numSize, int order) {
    int numCount = -1;
    double result;
    for(int i = 0; i < (*inputSize); i++) {
        if(charArr[i] == '$') numCount++;
        if(numCount == (*numSize)) break;
        if(charArr[i] == oType) {
            int index = findNextNum(numCount, (*numSize), numArr);
            if(index == -1 || index == 0) return;
            int cIndex1 = numInChar(charArr, index, *inputSize);
            int cIndex2 = numInChar(charArr, numCount, *inputSize);
            int valid = validOperation(charArr, numCount, index, (*inputSize), oType, order);
            if(valid == -1) return;
            if(oType == '+') {
                result = numArr[numCount] + numArr[index];
            } else if(oType == '-') {
                result = numArr[numCount] - numArr[index];
            } else if(oType == '*') {
                result = numArr[numCount] * numArr[index];
            } else if(oType == '/') {
                result = numArr[numCount] / numArr[index];
            } else if(oType == '^' && charArr[i-1] == '$' && charArr[i+1] == '$') {
                result = pow(numArr[numCount], numArr[index]);
            }
            /*shiftArray(charArr, NULL, inputSize, i);
            shiftArray(charArr, NULL, inputSize, cIndex1-1);
            numArr[numCount+1] = result;
            shiftArray(NULL, numArr, numSize, numCount);
            numCount++;*/
            
            charArr[i] = '@';
            charArr[cIndex1-1] = '@';
            numArr[index] = result;
            numArr[numCount] = 0;
            //numCount = index;
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
    } else if(equation[index] == 94) {
        return EXPONENT;
    }
    return 0;
}

void updateVars(int numCount, int *inputSize, int *numSize, int signs) {
    (*inputSize) = (*inputSize) - ((*numSize) - numCount) - signs;
    (*numSize) = numCount;
}

void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, double *leftNumArr,
             double *rightNumArr, char *leftCharArr, char *rightCharArr) {
    int currNum = -1, numCount = 0, charCount = 0, currState, brackNum = 0, negNum = 0, signs = 0,
        size = ((*inputSize1) + (*inputSize2) + 1), *currSize = inputSize1;
    double *currNumArr = leftNumArr;
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
            currSize = inputSize2;
            if(input[i+1] == '-') {
                negNum = 1;
                signs++;
            }
            continue;
        }
        currState = charType(input, i);
        if(currState == ALPHA) {
            if(i != 0 && charType(input, i-1) == NUM) {
                currCharArr[charCount] = '*';
                charCount++;
                (*currSize)++;
            }
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
        } else if(currState == EXPONENT) {
            currCharArr[charCount] = '^';
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

void printArr(int numSize1, int numSize2, int inputSize1, int inputSize2, double *leftNumArr, double *rightNumArr,
              char *leftCharArr, char *rightCharArr) {
    printf("left num start\n");
    for(int i = 0; i < numSize1; i++) {
        printf("%lf\n", leftNumArr[i]);
    }
    printf("left num ends\n");
    printf("right num start\n");
    for(int i = 0; i < numSize2; i++) {
        printf("%lf\n", rightNumArr[i]);
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
void simplify(int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
              char *leftCharArr, char *rightCharArr, int start) {
    if(start == 0) {
        for(int i = 0; i < OPER_NUM; i++) {
            operate(operations[i][0], leftNumArr, leftCharArr, inputSize1, numSize1, 1);
            operate(operations[i][0], rightNumArr, rightCharArr, inputSize2, numSize2, 1); 
        }  
    } else {
        for(int i = OPER_NUM-1; i >= 0; i--) {
            operate(operations[i][0], leftNumArr, leftCharArr, inputSize1, numSize1, -1);
            operate(operations[i][0], rightNumArr, rightCharArr, inputSize2, numSize2, -1); 
        }
    }
}

void addElement(double *numArr, char *charArr, int *size, double nElement, char cElement) {
    (*size)++;
    if(nElement == 0) {
        char *temp = charArr;
        for(int i = 0; i < ((*size)-1); i++) {
            charArr[i] = temp[i];
        }
        charArr[(*size)-1] = cElement;
    } else {
        double *temp = numArr;
        for(int i = 0; i < ((*size)-1); i++) {
            numArr[i] = temp[i];
        }
        numArr[(*size)-1] = nElement;
    }
}

// returns 1 if var has switched sides
int move(double *numArr, double *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite) {
    int numCount = -1, index;
    double moveNum;
    char moveChar;
    for(int i = 0; i < (*inputSize); i++) {
        if(charArr[i] == '$') numCount++;
        if(charArr[i] == oType) {
            if(oType != '^' && (charArr[i+1] == '$' || (charArr[i-1] == '$' && (oType == '+' || oType == '*')))) {
                index = findNextNum(numCount, (*numSize), numArr);
                if(charArr[i+1] == '$') {
                    index = findNextNum(numCount, (*numSize), numArr);
                    moveNum = numArr[index];
                    numArr[index] = 0;
                    charArr[i+1] = '@';
                } else {
                    index = findNextNum(numCount-1, (*numSize), numArr);
                    moveNum = numArr[index];
                    numArr[index] = 0;
                    charArr[i-1] = '@';
                }
                //shiftArray(charArr, NULL, inputSize, i);
                charArr[i] = '@';
                /*for(int i = 0; i < 10; i++) {
                    printf("char: %c  oChar: %c\n", charArr[i], oCharArr[i]);
                }*/
                addElement(oNumArr, NULL, oNumSize, moveNum, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, '$');
                printf("PRINTING/n");
                printArr(10, 10, 10, 10, numArr, oNumArr, charArr, oCharArr);
            } else if(oType != '^' && ((charType(charArr, i+1) == ALPHA && charArr[i+1] != var) ||
                       (charArr[i+1] == var && (oType == '/' || oType == '-'))) ) {
                moveChar = charArr[i+1];
                //shiftArray(charArr, NULL, inputSize, i);
                //shiftArray(charArr, NULL, inputSize, i+1);
                charArr[i] = '@';
                charArr[i+1] = '@';
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, moveChar);
                return 1;
            } else if(i != ((*inputSize)-1) && oType == '^' && charArr[i+1] == '$') {
                moveChar = '^';
                //shiftArray(charArr, NULL, inputSize, i);
                //shiftArray(charArr, NULL, inputSize, i+1);
                charArr[i] = '@';
                charArr[i+1] = '@';
                index = findNextNum(numCount, (*numSize), numArr);
                double num = numArr[index];
                if(DEBUG == 1) printf("---------------------%lf, %lf\n", 1.0 / num, num);
                moveNum = (1.0 / num);
                if(DEBUG == 1) printf("!!!!!!!!!!!!!!!!!!!!!!!!1moveNum = %lf\n", moveNum);
                numArr[index] = 0;
                addElement(oNumArr, NULL, oNumSize, moveNum, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, '$');
            }
        }
    }
    return 0;
}
// 5-a=10
/*
 * what if var doesn't exist?
 */


int solve(char var, int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
           char *leftCharArr, char *rightCharArr) {
    int leftRight = 1;
    for(int i = 0; i < (*inputSize1); i++) {
        if(leftCharArr[i] == var) {
            leftRight = 0;
        }
    }
    for(int i = (OPER_NUM-1); i >= 0; i--) {
        if(leftRight == 0) {
            if(move(leftNumArr, rightNumArr, leftCharArr, rightCharArr, numSize1, numSize2, inputSize1, inputSize2,
                 var, operations[i][0], operations[i][1]) == 1) {
               leftRight = !leftRight;
               i = (OPER_NUM-1);
            }
        } else {
            if(move(rightNumArr, leftNumArr, rightCharArr, leftCharArr, numSize2, numSize1, inputSize2, inputSize1,
                 var, operations[i][0], operations[i][1]) == 1) {
              leftRight = !leftRight;
              i = (OPER_NUM-1);
            }
        }
        //move(numArr, oNumArr, charArr, oCharArr, numSize, oNumSize, inputSize, oInputSize, var, operations[i][0], operations[i][1]);
    }
    /*int index = findNextNum(-1, (*numSize), numArr), opposite = '-';
    if(numArr[index] < 0) {
        opposite = '+';
    }
    if(index != -1) {
        addElement(oNumArr, NULL, oNumSize, numArr[index]*(-1), 0);
        numArr[index] = 0;
        addElement(NULL, oCharArr, oInputSize, 0, opposite);
        addElement(NULL, oCharArr, oInputSize, 0, '$');
    }*/
    return leftRight;
}

void displayResult(int inputSize, int numSize, double *numArr, char *charArr, char var) {
    int numCount = -1;
    printf("%c = ", var);
    for(int i = 0; i < inputSize; i++) {
        if(charArr[i] == '$') {
            numCount++;
            if(numArr[numCount] != 0) {
                printf("%.2lf ", numArr[numCount]);
            }
        } else if (charArr[i] != '@') {
            printf("%c ", charArr[i]);
        }
    }
    printf("\n");
}

void shiftArray(char *input1, double *input2, int *inputSize, int index) {
    int done = 0;
    (*inputSize)--;
    if(input1 == NULL) {
        double *temp = input2;
        for(int i = 0; i < (*inputSize); i++) {
            if(i == index) done = 1;
            if(done == 0) {
                input2[i] = temp[i];
            } else {
                input2[i] = temp[i+1];
            }
        }
    }
    else {
        char *temp = input1;
        for(int i = 0; i < (*inputSize); i++) {
            if(i == index) done = 1;
            if(done == 0) {
                input1[i] = temp[i];
            } else {
                input1[i] = temp[i+1];
            }
        }
    }
    // debugging
    printf("###################### PRINTING SHIFTED ARRAY\n");
    for(int i = 0; i < (*inputSize); i++) {
        if(input1==NULL) printf("%lf\n", input2[i]);
        else printf("%c\n", input1[i]);
    }
    printf("inputSize = %d\n", (*inputSize));
}
// returns 1 if error found in equation, 0 if not
/* error checking for:
 * - spaces in equation (equation is adjusted)
 * - if var is valid
 * - if var exists in the equation
 * - if equation has an unknown character (charType returns 0)
 * - have operations next to each other (i.e. +-)
 * - cannot end with operator
 * - cannot start with * or /
 * - cannot have empty input or input with only spaces
 */
int errorCheck(char *input, int *inputSize, char var) {
    int size = (*inputSize), varFound = 0;
    for(int i = 0; i < size; i++) {
        if(var == '=') {
            if(input[i] == ' ' && (*inputSize) != 0) {
                shiftArray(input, NULL, inputSize, i);
                i = 0;
            } else if(charType(input, i) == 0 && input[i] != 32) {
                printf("%c is not a valid character.\n", input[i]);
                return 1;
            } else if(i != ((*inputSize)-1) && charType(input, i) == OPERATION && charType(input, i+1) == OPERATION) {
                printf("You cannot place two operators next to each other.\n");
                return 1;
            }
        } else {
            if(input[i] == var) {
                varFound = 1;
            }
        }
    }
    if((*inputSize) == 0) {
        return 1;
    }
    if(input[0] == '*' || input[0] == '/') {
        printf("You cannot begin with %c.\n", input[0]);
        return 1;
    }
    if(charType(input, (*inputSize)-1) == OPERATION) {
        printf("You cannot end with %c.\n", input[(*inputSize)-1]);
        return 1;
    }
    if( (var != '=') && (((var > 90) || (var < 65)) && ((var > 122) || (var < 96))) ) {
        printf("%c is not a valid letter.\n", var);
        return 1;
    } else if(varFound == 0 && var != '=') {
        printf("%c does not exist in the input.\n", var);
        return 1;
    }
    return 0;
}

int main(void) {
    //while(1) {
        char input[INPUT_MAX], leftCharArr[INPUT_MAX], rightCharArr[INPUT_MAX], var;
        int inputSize1 = 0, inputSize2 = 0, numSize1 = 0, numSize2 = 0, done = 0, cType,
            leftRight; 
        double leftNumArr[INPUT_MAX], rightNumArr[INPUT_MAX];
        do {
            printf("Please input your equation (only use +, -, *, /) with only one letter and no decimals: ");
            fgets(input, INPUT_MAX, stdin);
            inputSize1 = strlen(input) - 1;
        }while(errorCheck(input, &inputSize1, '=') == 1);
        do {
            printf("What variable would you like to isolate? (case-sensitive): ");
            scanf("\n%c", &var);
        }while(errorCheck(input, &inputSize1, var) == 1);
        
        if(DEBUG == 1) printf("\nINPUTSIZE: %d\n", inputSize1);
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
        if(DEBUG == 1) printf("\n\nsize1 = %d, size2 = %d:\n", inputSize1, inputSize2);
        //printf("size: %d, num: %d\n", inputSize, numSize);
        //printf("nEqualPlace: %d\n", nEqualPlace);
        convert(input, &numSize1, &numSize2, &inputSize1, &inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
         //debugging---------------------------------------------
        /*for(int i = 0; i < (inputSize1+inputSize2); i++) {
            printf("%c\n", input[i]);
        }*/
        //--------------------------------------------------------
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        simplify(&inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 0);
        if(DEBUG == 1) printf("\nafter simplifying:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        leftRight = solve(var, &inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        if(DEBUG == 1) printf("\nafter solving:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        if(DEBUG == 1) printf("\n\nsize1 = %d, size2 = %d:\n", inputSize1, inputSize2);
        simplify(&inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 4);
        if(DEBUG == 1) printf("\nafter again:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        double *numArr = leftNumArr;
        int inputSize = inputSize1, numSize = numSize1;
        char *charArr = leftCharArr;
        if(leftRight == 0) {
            numArr = rightNumArr;
            charArr = rightCharArr;
            numSize = numSize2;
            inputSize = inputSize2;
        }
        displayResult(inputSize, numSize, numArr, charArr, var);
    //}
    return 0;
}

