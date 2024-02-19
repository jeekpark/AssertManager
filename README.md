# AssertManager

## Overview
`AssertManager`는 C++ 개발에 사용가능한 assertion 시스템입니다. 이 시스템은 개발 과정에서 오류를 조기에 발견하고 해결하는데 도움을 줍니다. 

## Tip
> 개발 과정에서 문제가 있을 경우 최대한 빠르게 assert에 걸리도록 코드를 작성하는 것을 권장합니다.(문제가 있는 상태로 프로그램이 돌다가 나중에 크러시가 난다면 디버깅이 매우 어렵기 떄문에)

## Features
- **스트림으로 커스텀 메세지 작성**: `AssertManager`는 스트림을 통해 다양한 타입의 데이터를 쉽게 힌트 메세지로 추가할 수 있습니다.
- 자세한 오류 정보: Assertion 실패 시, 함수 이름, 파일 이름, 라인 번호 등의 상세 정보가 제공됩니다.
- NDEBUG 지원: C의 Assert와 동일하게 NDEBUG가 정의되어있다면 컴파일시 assertion이 포함되지 않습니다. (C의 Assert와 동일)
- header only: 헤더 파일만으로 구성되어 있어, 복잡한 설치나 링크 과정 없이 쉽게 사용할 수 있습니다.

## Getting Started
### Prerequisites
- C++ 컴파일러(C++98 이상)

### Installation
이 시스템은 별도의 설치 과정이 필요 없습니다. `AssertManager.hpp`를 프로젝트에 포함시킵니다.
1. 레포지토리 클론 or 서브모듈 추가:
```
git clone https://github.com/jeekpark/AssertManager.git
```
```
git submodule add https://github.com/jeekpark/AssertManager.git
```

### Usage
`ASSERT(msg)` 매크로를 사용하여 로그를 기록합니다
#### Code
```cpp
#include "AssertManager.hpp"

int main()
{
    int x = 5;
    ASSERT(x == 10) << "값이 예상과 다릅니다. x: " << x;
    return 0;
}
```

#### Result
```
>./a.out

[Assertion failure]
 - Assert:      (x == 10)
 - Message:     "값이 예상과 다릅니다. x: 5"
 - File:        main.cpp:5
 - Function:    int main()

zsh: abort      ./a.out
>
```
#### NDEBUG
릴리즈 버전에서는 Assert가 작동되지 않도록 `NDEBUG`를 정의하면 컴파일시 반영되지 않습니다.
```cpp
#define NDEBUG

#include "AssertManager.hpp"
int main()
{
    int x = 10;
    // NDEBUG로 인해 문장 무시됨. 
    ASSERT(x == 10) << "a: " << a;스트림도 무시.
    return 0;
}

```




