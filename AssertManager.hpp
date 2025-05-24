/**
 * @file AssertManager.hpp
 * @brief C++ 단언(assertion) 시스템을 위한 AssertManager 클래스 및 관련 유틸리티를 정의합니다.
 *
 * 이 헤더는 C++ 프로젝트를 위한 유연한 단언 메커니즘을 제공합니다.
 * 주요 기능:
 *  - 스트림 문법(operator<<)을 사용한 사용자 정의 단언 메시지.
 *  - 함수 이름, 파일 이름, 줄 번호를 포함한 상세한 오류 정보.
 *  - 단언은 조건부로 컴파일됩니다: 디버그 빌드에서는 활성화되고,
 *    릴리스 빌드(NDEBUG 또는 _NDEBUG 정의 시)에서는 제거됩니다.
 *  - 헤더 전용: 이 파일만 포함하면 쉽게 통합할 수 있습니다.
 */

/* Private API */

#ifndef ASSERT_MANAGER_HPP
#define ASSERT_MANAGER_HPP

#define ASSERT_MANAGER_VERSION "1.0.0"

#include <sstream>
#include <iostream>

#if !defined(__clang__) && !defined(__GNUC__)
    #define __PRETTY_FUNCTION__ __FUNCTION__
#endif

/**
 * @class AssertManager
 * @brief 단언 로직 및 보고 기능을 관리합니다.
 *
 * 이 클래스는 단언 로직의 핵심 요소입니다.
 * 주로 단언 실패 시 호출되는 정적 `Assert` 메서드를 포함합니다.
 * 이 메서드는 표현식, 메시지, 파일, 줄 번호, 함수 이름 등 단언 실패에 대한 상세 정보를
 * 기록하고 보고하는 역할을 합니다.
 */
class AssertManager
{
public:
    /**
     * @brief 단언 실패를 처리합니다.
     * @param condition 단언의 boolean 결과입니다. true이면 함수가 반환되고, false이면 단언 실패가 처리됩니다.
     * @param expressionString 단언된 표현식의 문자열 표현입니다.
     * @param message 사용자가 제공한 사용자 정의 메시지 문자열입니다.
     * @param functionName 단언이 발생한 함수의 이름입니다.
     * @param fileName 단언이 발생한 파일의 이름입니다.
     * @param lineNumber 단언이 발생한 줄 번호입니다.
     *
     * `condition`을 확인합니다. true이면 아무것도 하지 않고 반환합니다.
     * `condition`이 false이면, 제공된 모든 매개변수를 포함한 오류 메시지를 형식화하여 `std::cerr`에 출력합니다.
     * 메시지를 출력한 후 `std::abort()`를 호출하여 프로그램을 종료합니다.
     */
    static void Assert(bool condition, const char* expressionString, const std::string& message,
                const char* functionName, const char* fileName, const int lineNumber)
    {
        if (condition)
            return ;
        else
        {
            std::cerr << "\n[Assertion failure]\n" 
                      << " - Assert:\t(" << expressionString << ")\n"
                      << (message.length() > 0 ? (" - Message:\t\"" + message + "\"\n") : "")
                      << " - File:\t" << fileName << ':' << lineNumber <<  '\n'
                      << " - Function:\t" << functionName << "\n\n"
                      << std::flush;
            std::abort();
        }
    }

    /**
     * @class AssertStream
     * @brief 스트림 문법을 사용하여 단언 메시지를 구성하기 위한 헬퍼 클래스입니다.
     *
     * 이 클래스는 `ASSERT` 매크로에 의해 사용됩니다. 생성 시 단언
     * 조건 및 관련 정보(표현식, 함수 이름, 파일, 줄 번호)를 캡처합니다.
     * 그런 다음 오버로드된 `operator<<`를 통해 사용자 정의 메시지를 추가할 수 있도록 합니다.
     * 실제 단언 로직(`AssertManager::Assert` 호출)은 조건이
     * false였을 경우 소멸자에서 호출되어, 평가 전에 전체 메시지가 구성되도록 보장합니다.
     */
    class AssertStream
    {
    public:
        /**
         * @brief AssertStream 객체를 생성합니다.
         * @param condition 단언 표현식의 boolean 결과입니다.
         * @param expressionString 단언된 표현식의 문자열 표현입니다.
         * @param functionName 단언이 발생한 함수의 이름입니다.
         * @param fileName 단언이 발생한 파일의 이름입니다.
         * @param lineNumber 단언이 발생한 줄 번호입니다.
         *
         * 제공된 모든 매개변수(`condition`, `expressionString`, `functionName`, `fileName`, `lineNumber`)를
         * 멤버 변수에 저장하여 `AssertStream` 객체를 초기화합니다.
         * 이 저장된 값들은 소멸자가 단언 검사를 수행하고, 단언 실패 시 상세 정보를 보고하는 데 사용됩니다.
         */
        AssertStream(bool condition, const char* expressionString,
                     const char* functionName, const char* fileName,
                     const int lineNumber)
        : bCondition(condition)
        , mExpressionString(expressionString)
        , mFunctionName(functionName)
        , mFileName(fileName)
        , mLineNumber(lineNumber)
        {}
        /**
         * @brief AssertStream의 소멸자입니다.
         *
         * 실제 단언 검사가 여기에서 수행됩니다. `bCondition` (생성자에서 저장된 값)이
         * false이면, 이 소멸자는 원본 표현식, `mStream`에 누적된 메시지,
         * 그리고 컨텍스트(함수, 파일, 줄 번호)와 함께 `AssertManager::Assert`를 호출합니다.
         */
        ~AssertStream()
        {
            if (!bCondition)
                AssertManager::Assert(bCondition, mExpressionString,
                                      mStream.str(), mFunctionName,
                                      mFileName, mLineNumber);
        }
        /**
         * @brief 단언 스트림에 메시지를 추가합니다.
         * @tparam T 추가할 메시지의 타입입니다.
         * @param message 스트림에 추가할 메시지 내용입니다.
         * @return 현재 AssertStream 객체에 대한 참조를 반환하여,
         *         연쇄적인 `<<` 연산을 가능하게 합니다.
         *
         * 이 연산자를 사용하면 다양한 데이터 타입을 단언 메시지로 스트리밍할 수 있습니다.
         * 내용은 내부 `std::ostringstream` (`mStream`)에 추가 기록됩니다.
         * `operator<<` 호출을 연쇄적으로 사용할 수 있도록 `*this`를 반환합니다
         * (예: `ASSERT(x > 0) << "x는 양수여야 합니다. 현재 값: " << x;`).
         */
        template<typename T>
        AssertStream& operator<<(const T& message)
        {
            mStream << message;
            return *this;
        }
    
    private:
        bool bCondition; ///< 단언 표현식의 boolean 결과를 저장합니다.
        const char* mExpressionString; ///< 문자열로 변환된 단언 표현식을 저장합니다 (예: "x == 10").
        const char* mFunctionName;     ///< 단언이 발생한 함수의 이름을 저장합니다.
        const char* mFileName;         ///< 단언이 발생한 파일의 이름을 저장합니다.
        const int mLineNumber;         ///< 단언이 발생한 줄 번호를 저장합니다.
        std::ostringstream mStream;    ///< 사용자 정의 단언 메시지의 각 부분을 누적하는 데 사용되는 `std::ostringstream` 객체입니다.
    };
};

#endif

/* Public API */

#ifdef ASSERT
#undef ASSERT
#endif

/**
 * @def ASSERT(e)
 * @brief 주 단언 매크로입니다. 사용자는 이 매크로를 사용하여 조건을 검사해야 합니다.
 *
 * 디버그 빌드에서 (NDEBUG 및 _NDEBUG가 정의되지 않은 경우), 이 매크로는
 * 표현식 `e`를 평가합니다. `e`가 false이면 단언 실패를 트리거하여
 * 표현식, 소스 위치(파일, 줄 번호, 함수) 및 스트림으로 전달된 모든 사용자 정의 메시지를 출력합니다.
 * 이 매크로는 `AssertManager::AssertStream` 객체를 생성하여 `e`의 boolean 결과,
 * `e`의 문자열 표현 (`#e`), 그리고 컨텍스트(`__PRETTY_FUNCTION__`, `__FILE__`, `__LINE__`)를 캡처합니다.
 * 이러한 스트림 기반 접근 방식은 사용자 정의 메시지 추가를 가능하게 합니다:
 * 예: `ASSERT(ptr != nullptr) << "포인터는 nullptr이 아니어야 합니다.";`
 *
 * 릴리스 빌드에서 (NDEBUG 또는 _NDEBUG가 정의된 경우), 이 매크로는
 * `if(false) std::cerr`로 컴파일됩니다. 이는 효과적으로 단언 기능을 비활성화하며,
 * 표현식 `e`는 평가되지 않습니다. 이를 통해 단언문이 운영 코드의 성능에
 * 영향을 주지 않도록 보장합니다.
 *
 * 함수 이름은 `__PRETTY_FUNCTION__` (GCC/Clang에서 사용 가능) 또는
 * `__FUNCTION__` (표준 매크로, `__PRETTY_FUNCTION__`을 사용할 수 없는 경우 MSVC 등에서 대체 사용)을 통해 캡처됩니다.
 */
#if !(defined(NDEBUG) || defined(_NDEBUG))
#define ASSERT(e) AssertManager::AssertStream(e, #e, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define ASSERT(e) if(false) std::cerr
#endif
