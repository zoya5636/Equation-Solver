#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INPUT_MAX 100

int charType(char equation[INPUT_MAX], int index);
void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, 
             double *numAr1, double *numAr2, char *cAr1, char *cAr2);
int operate(char oType, double *numArr, char *charArr, int *inputSize, int *numSize, int order, int start, int end, int numCount);
int simplify(int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
              char *leftCharArr, char *rightCharArr, int start, int arrStart, int arrEnd, int numCount);
int move(double *numArr, double *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite);
int solve(char var, int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
           char *leftCharArr, char *rightCharArr);
void displayResult(int inputSize, int numSize, double *numArr, char *charArr, char var);
int errorCheck(char *input, int *inputSize, char var);
int numInChar(char *arr, int index, int inputSize);
void shiftArray(char *input1, double *input2, int *inputSize, int index, int a, char b);
int notSolvable(double num1, char oType, double num2);
void bracketSolve(double *numArr, char *charArr, int *numSize, int *inputSize, int openB, int closedB, int numCount, int leftRight);
void bracketSolve2(double *numArr, char *charArr, int *numSize, int *inputSize, int leftRight);

const int ALPHA = 1;
const int NUM = 2;
const int OPERATION = 3;
const int OPEN_BRACKET = 4;
const int CLOSED_BRACKET = 5;
const int EQUAL_SIGN = 6;
const int EXPONENT = 7;

const int DEBUG = 0;
#define OPER_NUM 5

char operations[OPER_NUM][2] = {
    {'^', '^'},
    {'/', '*'},
    {'*', '/'},
    {'+', '-'},
    {'-', '+'}
};

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
    if(oType == '^') return 1;
    else if(oType == '/' || oType == '*') end = 1;
    else if(oType == '-' || oType == '+') end = 3;
    for(int i = 0; i < end; i++) {
        if(arr[index] == operations[i][0]) return 0;
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
   /*for(int i = (cIndex1+1); i < cIndex2; i++) {
       if(arr[i] != '@' && arr[i] != oType) return -1;
   }*/
   //x = 2+3*a
   if(order == 1) {
       if((elementInArr(oType, arr, cIndex1-1) == 0) && found == 0) return -1;
       if((elementInArr(oType, arr, cIndex2+1) == 0) && found == 0) return -1;
       
       /*for(int i = (cIndex1+1); i > cIndex2; i--) {
           if(arr[i] == '$') found = 1;
           if((elementInArr(oType, arr, i) == 0) && found == 0) return -1;
       }
       found = 0;
       for(int i = (cIndex2+1); i < inputSize; i++) {
           if(arr[i] == '$') found = 1;
           if((elementInArr(oType, arr, i) == 0) && found == 0) return -1;
       }*/
   }
   return 1;
}

int operate(char oType, double *numArr, char *charArr, int *inputSize, int *numSize, int order, int start, int end, int numCount) {
    int oldNumCount;
    int wholeArr = 0;
    if(end == -1) {
        start = 0;
        end = (*inputSize) - 1;
        numCount = -1;
        wholeArr = 1;
    } else {
        oldNumCount = numCount;
    }
    double result;
    for(int i = start; i < end; i++) {
        if(charArr[i] == '$') numCount++;
        if(numCount == ((*numSize)-1)) return 0;
        if(charArr[i] == oType && numCount != -1) {
            int index = numCount+1;
            int cIndex1 = numInChar(charArr, index, *inputSize);
            int valid = validOperation(charArr, numCount, index, (*inputSize), oType, order);
            if(valid != -1) {
                if(notSolvable(numArr[numCount], oType, numArr[index]) == 1) return 1;
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
                shiftArray(charArr, NULL, inputSize, i, 0, 0);
                shiftArray(charArr, NULL, inputSize, cIndex1-1, 0, 0);
                numArr[index] = result;
                shiftArray(NULL, numArr, numSize, numCount, 0, 0);
                i = start;
                if(wholeArr == 0) {
                    end = end - 2; // fix
                    numCount = oldNumCount;
                } else {
                    numCount = -1;
                }
            }
        }
    }
    return 0;
}

// returns 0 if any other type
int charType(char equation[INPUT_MAX], int index) {
    if( (equation[index] <= 90 && equation[index] >= 65) ||
        (equation[index] <= 122 && equation[index] >= 96) ) {
        return ALPHA;   
    } else if(equation[index] <= 57 && equation[index] >= 48) {
        return NUM;
    } else if(equation[index] == 43 || equation[index] == 42 || equation[index] == 45 || equation[index] == 47 || equation[index] == '^') {
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

// returns 1 if num, 0 if not
int numOrNot(char input[INPUT_MAX], int bIndex) {
    int found = 0;
    for(int i = (bIndex+1); i < (INPUT_MAX-bIndex-1); i++) {
        if(charType(input, i) == CLOSED_BRACKET) {
            found = 1;
            break;
        } else if(charType(input, i) != NUM && found == 0) {
            return 0;
        }
    }
    return 1;
}

void convert(char input[INPUT_MAX], int *numSize1, int *numSize2, int *inputSize1, int *inputSize2, double *leftNumArr,
             double *rightNumArr, char *leftCharArr, char *rightCharArr) {
    int currNum = -1, numCount = 0, charCount = 0, currState, brackNum = 0, negNum = 0, signs = 0,
        size = ((*inputSize1) + (*inputSize2) + 1), *currSize = inputSize1;
    double *currNumArr = leftNumArr;
    char *currCharArr = leftCharArr;
    if(input[0] == '-') {
        if(charType(input, 1) == NUM) {
            negNum = 1;
            signs++;
        } else if(charType(input, 1) == ALPHA) {
            currCharArr[charCount++] = '$';
            currCharArr[charCount++] = '*';
            currNumArr[numCount++] = -1;
        }
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
                if(charType(input, i+2) == NUM) {
                    negNum = 1;
                    signs++;
                } else if(charType(input, i+2) == ALPHA) {
                    currCharArr[charCount++] = '$';
                    currCharArr[charCount++] = '*';
                    currNumArr[numCount++] = -1;
                }
            }
            continue;
        }
        currState = charType(input, i);
        if(currState == ALPHA) {
            if(i != 0 && charType(input, i-1) == NUM) {
                currCharArr[charCount++] = '*';
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
            if( i!=0 && charType(input, i-1) == OPEN_BRACKET && charType(input, i+1) == NUM) {
                brackNum = 1;
                if(input[i] == '-') {
                    negNum = 1;
                    signs++;
                }
            } else if(input[i] == '-' && charType(input, i+1) == ALPHA) {
                currCharArr[charCount++] = '+';
                currCharArr[charCount++] = '$';
                currCharArr[charCount++] = '*';
                currNumArr[numCount++] = -1;
                signs--;
            } else if(brackNum == 0 && negNum == 0 && ( (i!=0 && input[i-1] != '=') || charType(input, i+1) != ALPHA ) ) {
                currCharArr[charCount++] = input[i];
            }
        } else if(currState == OPEN_BRACKET) {
            if(numOrNot(input, i) == 0) {
                currCharArr[charCount++] = '(';
            } else if(charType(input, i-1) == NUM) {
                currCharArr[charCount++] = '*';
            } else {
                signs++;
            }
        } else if(currState == EQUAL_SIGN) {
            currCharArr[charCount++] = '=';
        } else if(currState == EXPONENT) {
            currCharArr[charCount++] = '^';
        } else if(currState == CLOSED_BRACKET) {
            if(charType(input, i-2) == OPEN_BRACKET) {
                if(charType(input, i+1) == OPEN_BRACKET || charType(input, i+1) == NUM) {
                    currCharArr[charCount++] = '*';
                } else {
                    signs++;
                }
            } else {
                currCharArr[charCount++] = ')';
            }
        }
        if(currState == NUM && (i == (size - 1) || charType(input, i+1) != NUM)) {
            currCharArr[charCount++] = '$';
            if(negNum == 1) currNum *= -1;
            currNumArr[numCount++] = currNum;
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
int simplify(int *inputSize1, int *inputSize2, int *numSize1, int *numSize2, double *leftNumArr, double *rightNumArr,
              char *leftCharArr, char *rightCharArr, int start, int arrStart, int arrEnd, int numCount) {
    if(start == 0) {
        for(int i = 0; i < OPER_NUM; i++) {
            if(operate(operations[i][0], leftNumArr, leftCharArr, inputSize1, numSize1, 1, arrStart, arrEnd, numCount) == 1 ||
            ((rightNumArr == NULL) || operate(operations[i][0], rightNumArr, rightCharArr, inputSize2, numSize2, 1, arrStart, arrEnd, numCount) == 1)) {
                if(rightNumArr != NULL) return 1;
            }
        }  
    } else {
        for(int i = OPER_NUM-1; i >= 0; i--) {
            if(operate(operations[i][0], leftNumArr, leftCharArr, inputSize1, numSize1, -1, arrStart, arrEnd, numCount) == 1 ||
            ((rightNumArr == NULL) || operate(operations[i][0], rightNumArr, rightCharArr, inputSize2, numSize2, -1, arrStart, arrEnd, numCount) == 1)) {
                if(rightNumArr != NULL) return 1;
            }
        }
    }
    return 0;
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

// may have to change i when size is adjusted!
// returns 1 if variable side has changed
int move(double *numArr, double *oNumArr, char *charArr, char *oCharArr, int *numSize, int *oNumSize,
          int *inputSize, int *oInputSize, char var, char oType, char opposite) {
    int numCount = -1, nIndex, cIndex, check1=0, check2=0;
    double moveNum;
    char moveChar;
    if(DEBUG == 1) printArr(*numSize, *oNumSize, *inputSize, *oInputSize, numArr, oNumArr, charArr, oCharArr);
    for(int i = 0; i < (*inputSize); i++) {
        if(charArr[i] == '$') numCount++;
        if(charType(charArr, 0) == OPERATION && charArr[1] == '$') {
            if(charArr[0] == '-') numArr[0] *= (-1);
            shiftArray(charArr, NULL, inputSize, 0, 0, 0);
            continue;
        } else if(oType == '+' && charArr[0] == '$' && charArr[1] == '-') {
            moveNum = numArr[0];
            shiftArray(NULL, numArr, numSize, 0, 0, 0);
            shiftArray(charArr, NULL, inputSize, 0, 0, 0);
            addElement(oNumArr, NULL, oNumSize, moveNum, 0);
            addElement(NULL, oCharArr, oInputSize, 0, opposite);
            addElement(NULL, oCharArr, oInputSize, 0, '$');
            continue;
        } else if(charArr[0] == '-' && charType(charArr, 1) == ALPHA) {
            shiftArray(charArr, NULL, inputSize, 0, 0, '*');
            shiftArray(charArr, NULL, inputSize, 0, 0, '$');
            if(DEBUG == 1) {
                printArr(*numSize, *oNumSize, *inputSize, *oInputSize, numArr, oNumArr, charArr, oCharArr);
            }
        }
        if(i!=0 && charArr[i-1] == oType) {
            check1 = elementInArr(oType, charArr, i+1);
            check2 = elementInArr(oType, charArr, i-3);
            if(oType != '^' && ( (charArr[i] == '$' &&  ((i == (*inputSize)-1) || check1 == 1)) ||
              (charArr[i-2] == '$' && (check2 == 1 || i <= 2) && (oType == '+' || oType == '*')) )) {
                if(charArr[i] == '$' && check1 == 1) {
                    cIndex = i;
                    nIndex = numCount;
                } else if(check2 == 1){
                    cIndex = (i-2);
                    nIndex = numCount;
                    if(charArr[i] == '$') nIndex -= 1;
                }
                moveNum = numArr[nIndex];
                shiftArray(charArr, NULL, inputSize, i-1, 0, 0);
                shiftArray(NULL, numArr, numSize, nIndex, 0, 0);
                shiftArray(charArr, NULL, inputSize, cIndex, 0, 0);
                addElement(oNumArr, NULL, oNumSize, moveNum, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, '$');
            } else if(oType != '^' && ((charType(charArr, i) == ALPHA && charArr[i] != var) ||
                     (charArr[i] == var && (oType == '/' || oType == '-')))) {
                if(DEBUG == 1) printArr(*numSize, *oNumSize, *inputSize, *oInputSize, numArr, oNumArr, charArr, oCharArr);
                moveChar = charArr[i];
                shiftArray(charArr, NULL, inputSize, i, 0, 0);
                shiftArray(charArr, NULL, inputSize, i-1, 0, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, moveChar);
                if(DEBUG == 1) printf("PRINTING ARRAYS BEFORE MOVING\n");
                if(DEBUG == 1) printArr(*numSize, *oNumSize, *inputSize, *oInputSize, numArr, oNumArr, charArr, oCharArr);
                if(charArr[i] == var){
                    if(DEBUG == 1) printf("oType = %c ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", oType);
                    if(DEBUG == 1) printArr(10, 10, 10, 10, numArr, oNumArr, charArr, oCharArr);
                    return 1;
                }
            } else if(oType == '^' && charArr[i] == '$') {
                shiftArray(charArr, NULL, inputSize, i, 0, 0);
                double num = numArr[numCount];
                moveNum = (1.0 / num);
                shiftArray(NULL, numArr, numSize, numCount, 0, 0);
                addElement(oNumArr, NULL, oNumSize, moveNum, 0);
                addElement(NULL, oCharArr, oInputSize, 0, opposite);
                addElement(NULL, oCharArr, oInputSize, 0, '$');
            }
        }
    }
    if(DEBUG == 1) printf("oType = %c ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", oType);
    if(DEBUG == 1) printArr(*numSize, *oNumSize, *inputSize, *oInputSize, numArr, oNumArr, charArr, oCharArr);
    return 0;
}


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
               if(simplify(inputSize1, inputSize2, numSize1, numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 4, -1, -1, -1) == 1) {
                   return 2;
               }
               i = (OPER_NUM-1);
            }
        } else {
            if(move(rightNumArr, leftNumArr, rightCharArr, leftCharArr, numSize2, numSize1, inputSize2, inputSize1,
                 var, operations[i][0], operations[i][1]) == 1) {
              leftRight = !leftRight;
              if(simplify(inputSize1, inputSize2, numSize1, numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 4, -1, -1, -1) == 1) {
                  return 2;
              }
              i = (OPER_NUM-1);
            }
        }
    }
    return leftRight;
}

void displayResult(int inputSize, int numSize, double *numArr, char *charArr, char var) {
    int numCount = -1;
    printf("%c = ", var);
    for(int i = 0; i < inputSize; i++) {
        if(charArr[i] == '$') {
            printf("%.2lf ", numArr[++numCount]);
        } else if (charArr[i] != '@') {
            printf("%c ", charArr[i]);
        }
    }
    printf("\n");
}

void shiftArray(char *input1, double *input2, int *inputSize, int index, int nElement, char cElement) {
    int done = 0, delta;
    if(cElement == 0 && nElement == 0){
        (*inputSize)--;
    }
    else{
        (*inputSize)++;
    }
    if(input1 == NULL) {
        double *temp = input2;
        for(int i = 0; i < (*inputSize); i++) {
            if(i == index) {
                done = 1;
                if(nElement != 0) {
                    delta = -1;
                } else {
                    delta = 1;
                }
            }
            if(done == 0) {
                input2[i] = temp[i];
            } else {
                input2[i] = temp[i+delta];
            }
        }
    }
    else {
        char *temp = input1;
        for(int i = 0; i < (*inputSize); i++) {
            if(i == index) {
                done = 1;
                if(cElement != 0) {
                    delta = -1;
                } else {
                    delta = 1;
                }
            }
            if(done == 0) {
                input1[i] = temp[i];
            } else {
                input1[i] = temp[i+delta];
            }
        }
    }
    // debugging
    if(DEBUG == 1) {
        printf("###################### PRINTING SHIFTED ARRAY\n");
        for(int i = 0; i < (*inputSize); i++) {
            if(input1 == NULL) printf("%lf\n", input2[i]);
            else printf("%c\n", input1[i]);
        }
        printf("inputSize = %d\n", (*inputSize));
    }
}
// returns 1 if error found in equation, 0 if not, 2 if user wishes to stop
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
    if(var == '=' && (strcmp(input, "stop\n") == 0 || strcmp(input, "Stop\n") == 0)) {
        return 2;
    }
    int size = (*inputSize), varFound = 0;
    for(int i = 0; i < size; i++) {
        if(var == '=') {
            if(input[i] == ' ' && (*inputSize) != 0) {
                shiftArray(input, NULL, inputSize, i, 0, 0);
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

void noVarEq(char *rightCharArr, char *leftCharArr, double *leftNumArr, double *rightNumArr) {
    if(leftNumArr[0] == rightNumArr[0]){
        printf("True\n");
    } else {
        printf("False\n");
    }
}

// returns 1 if not solvable
int notSolvable(double num1, char oType, double num2) {
    if(oType == '/') {
        if(num2 == 0) {
            if(DEBUG == 1) printf("Cannot divide by 0\n");
            return 1;
        }
    } else if(oType == '^') {
        int exp = (1/num2);
        if( (exp > 1) && (exp % 2 == 0) && num1 < 0 ) {
            if(DEBUG == 1) printf("Cannot do (%lf.2)^(%lf.2)\n", num1, num2);
            return 1;
        }
    }
    return 0;
}

void bracketSolve2(double *numArr, char *charArr, int *numSize, int *inputSize, int leftRight) {
    int numCount = -1;
    for(int i = 0; i < (*inputSize); i++) {
        if(charType(charArr, i) == OPEN_BRACKET) {
            bracketSolve(numArr, charArr, numSize, inputSize, i, -1, numCount, leftRight);
            numCount++;
        } else if(charArr[i] == '$') numCount++;
    }
}

void bracketSolve(double *numArr, char *charArr, int *numSize, int *inputSize, int openB, int closedB, int numCount, int leftRight) {
    int newNumCount = 0;
    for(int i = (openB + 1); i < (*inputSize); i++) {
        if(charType(charArr, i) == OPEN_BRACKET) {
            bracketSolve(numArr, charArr, numSize, inputSize, i, closedB, numCount+newNumCount, leftRight);
        }
        if(charType(charArr, i) == CLOSED_BRACKET) {
            closedB = i;
            if(leftRight == 0) {
                simplify(inputSize, NULL, numSize, NULL, numArr, NULL, charArr, NULL, 0, openB+1, closedB, numCount);
            } else {
                simplify(NULL, inputSize, NULL, numSize, NULL, numArr, NULL, charArr, 0, openB+1, closedB, numCount);
            }
            shiftArray(charArr, NULL, inputSize, openB, 0, 0);
            shiftArray(charArr, NULL, inputSize, openB+1, 0, 0);
            return;
        }
        if(charArr[i] == '$') newNumCount++;
    }
}

int main(void) {
    while(1) {
        char input[INPUT_MAX], leftCharArr[INPUT_MAX], rightCharArr[INPUT_MAX], var, varCount = 0;
        int inputSize1 = 0, inputSize2 = 0, numSize1 = 0, numSize2 = 0, done = 0, cType,
            leftRight, result, notSolvable; 
        double leftNumArr[INPUT_MAX], rightNumArr[INPUT_MAX];
        do {
            printf("Please input your equation with only one letter max and no decimals, and input 'stop' to end: ");
            fgets(input, INPUT_MAX, stdin);
            inputSize1 = strlen(input) - 1;
            result = errorCheck(input, &inputSize1, '=');
            if(result == 2) return 0;
        }while(result == 1);
        for(int i = 0; i < inputSize1; i++) {
            if(charType(input, i) == ALPHA) {
                varCount++;
                var = input[i];
            }
            if(varCount > 1) break;
        }
        if(varCount > 1) {
            do {
                printf("What variable would you like to isolate? (case-sensitive): ");
                scanf("\n%c", &var);
            }while(errorCheck(input, &inputSize1, var) == 1);
        }
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
        convert(input, &numSize1, &numSize2, &inputSize1, &inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        
        bracketSolve2(leftNumArr, leftCharArr, &numSize1, &inputSize1, 0);
        //bracketSolve(leftNumArr, leftCharArr, &numSize1, &inputSize1, -1, -1, -1, 0);
        if(DEBUG == 1) printf("After bracket solve left:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        bracketSolve2(rightNumArr, rightCharArr, &numSize2, &inputSize2, 1);
        //bracketSolve(rightNumArr, rightCharArr, &numSize2, &inputSize2, -1, -1, -1, 1);
        if(DEBUG == 1) printf("After bracket solve right:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        
        
        notSolvable = simplify(&inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 0, -1, -1, -1);
        if(DEBUG == 1) printf("\nafter simplifying:\n");
        if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        if(varCount == 0 && notSolvable == 0) {
            noVarEq(rightCharArr, leftCharArr, leftNumArr, rightNumArr);
        } else if(notSolvable == 0) {
            leftRight = solve(var, &inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
            if(leftRight == 2) notSolvable = 1;
            if(DEBUG == 1) printf("\nafter solving:\n");
            if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
            if(DEBUG == 1) printf("\n\nsize1 = %d, size2 = %d:\n", inputSize1, inputSize2);
            if(notSolvable == 0) notSolvable = simplify(&inputSize1, &inputSize2, &numSize1, &numSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr, 4, -1, -1, -1);
            if(DEBUG == 1) printf("\nafter again:\n");
            if(DEBUG == 1) printArr(numSize1, numSize2, inputSize1, inputSize2, leftNumArr, rightNumArr, leftCharArr, rightCharArr);
        }
        if(notSolvable == 1) {
            printf("This is not solvable.\n");
        } else if(varCount != 0) {
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
        }
        memset(input, 0, sizeof(input));
        memset(leftCharArr, 0, sizeof(leftCharArr));
        memset(rightCharArr, 0, sizeof(rightCharArr));
        memset(leftNumArr, 0, sizeof(leftNumArr));
        memset(rightNumArr, 0, sizeof(rightNumArr));
        inputSize1 = 0;
        inputSize2 = 0;
        numSize1 = 0;
        numSize2 = 0;
        done = 0;
        varCount = 0;
    }
    return 0;
}

