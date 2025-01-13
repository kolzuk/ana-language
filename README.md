## Требования для языка:

1. Вычислить факториал числа `n`:
```c
int factorial(int n) {
  if (n <= 1)
    return 1;

  return n * factorial(n - 1);
}
```

2. Сортировка массива `arr`:
```c
int partition(int arr[], int l, int r) {
   int v = arr[(l + r) / 2];
   int i = l;
   int j = r;
   while (i <= j) {
     while (arr[i] < v)
       i = i + 1;
     while (arr[j] > v)
       j = j + 1;
     if (i >= j)
       break;

     int tmp = arr[i];
     arr[i] = arr[j];
     arr[j] = tmp;
   }

   return j;
}

void quicksort(int arr[], int l, int r) {
  if (l < r) {
    int q = partition(arr, l, r);
    quicksort(arr, l, q);
    quicksort(arr, q + 1, r);
  }
}
```

3. Вычисление всех простых чисел до `n`:
```c
int *getPrimeNumbers(int n) {
  int *numbers = (int *) malloc(n * sizeof(int));
  numbers[0] = 0;
  numbers[1] = 0;
  for (int i = 2; i <= n; ++i) {
    numbers[i] = 1;
  }

  for (int i = 2; i <= n; ++i) {
    if (numbers[i] == 1) {
      if (i * 1ll * i <= n) {
        for (int j = i * i; j <= n; j += i) {
          numbers[j] = 0;
        }
      }
    }
  }

  return numbers;
}
```

## Грамматика языка:

```
compilationUnit
    : ( declaration )*
declaration
    : variableDeclaration | functionDeclaration
functionDeclaration
    : "fun" identifier "(" argumentList ")" ( "->" type )? "{" statementSequence "}"
argumentList
    : ( type identifier ( "," type identifier )* )?
variableDeclaration
    : type identifier ("=" expression)? ";"
type
    : "integer" | "array"
integerType
    : "integer"
arrayType
    : "array" "[" integerLiteral "]"
statementSequence
    : statement*
statement
    : variableDeclaration
    | ifStatement
    | whileStatement
    | returnStatement
    | (assignStatement ";")
ifStatement
    : "if" "(" expression ")" "{" statementSequence "}"
    ( "else" "{" statementSequence "}" )?
whileStatement
    : "while" "(" expression ")" "{" statementSequence "}"
returnStatement
    : "return" ( expression )? ";"
assignStatement
    : expression ( "=" expression)?
expression
    : simpleExpression (relation simpleExpression)?
relation
    : "==" | "!=" | "<" | "<=" | ">" | ">="
simpleExpression
    : term (addOperator term)*
addOperator
    : "+" | "-"
term
    : mulOperand (mulOperator mulOperand)*
mulOperand
    : ("+" | "-")? factor
mulOperator
    : "*" | "/"
factor
    : integerLiteral
    | arrayInitialization
    | identifier
    | getByIndex
    | expressionFactor
    | functionCall
integerLiteral
    : [0-9]+
identifier
    : [a-zA-Z_][a-zA-Z0-9_]*
getByIndex
    : identifier "[" expression "]";
expressionFactor
    : "(" expression ")"
functionCall
    : identifier "(" (expressionList)? ")" ;
arrayInitialization
    : "[" (expression ("," expression)*) "]";
expressionList
    : expression ( "," expression )* ;
```
