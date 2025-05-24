/**
 * @file AssertManager.hpp
 * @brief Defines the AssertManager class and related utilities for a C++ assertion system.
 *
 * This header provides a flexible assertion mechanism for C++ projects.
 * Key features:
 *  - Custom assertion messages using stream syntax (operator<<).
 *  - Detailed error information including function name, file name, and line number.
 *  - Assertions are conditionally compiled: they are active in debug builds and
 *    removed in release builds (when NDEBUG or _NDEBUG is defined).
 *  - Header-only: Easy to integrate by just including this file.
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
 * @brief Manages assertion logic and reporting.
 *
 * This class is the central point for the assertion logic.
 * It primarily houses the static `Assert` method, which is invoked when an assertion fails.
 * This method is responsible for recording and reporting detailed information about the assertion failure,
 * including the expression, message, file, line number, and function name.
 */
class AssertManager
{
public:
    /**
     * @brief Handles an assertion failure.
     * @param condition The boolean result of the assertion. If true, the function returns. If false, an assertion failure is processed.
     * @param expressionString A string representation of the asserted expression.
     * @param message A custom message string provided by the user.
     * @param functionName The name of the function where the assertion occurred.
     * @param fileName The name of the file where the assertion occurred.
     * @param lineNumber The line number where the assertion occurred.
     *
     * Checks the `condition`. If true, it does nothing and returns.
     * If `condition` is false, it formats an error message including all the provided parameters and prints it to `std::cerr`.
     * After printing the message, it calls `std::abort()` to terminate the program.
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
     * @brief Helper class for building assertion messages using stream syntax.
     *
     * This class is used by the `ASSERT` macro. It captures the assertion
     * condition and related information (expression, function name, file, line number)
     * upon construction. It then allows appending custom messages via the
     * overloaded `operator<<`. The actual assertion logic (calling
     * `AssertManager::Assert`) is invoked in the destructor if the condition
     * was false, ensuring the complete message is built before evaluation.
     */
    class AssertStream
    {
    public:
        /**
         * @brief Constructs an AssertStream object.
         * @param condition The boolean result of the assertion expression.
         * @param expressionString A string representation of the asserted expression.
         * @param functionName The name of the function where the assertion occurred.
         * @param fileName The name of the file where the assertion occurred.
         * @param lineNumber The line number where the assertion occurred.
         *
         * Initializes the `AssertStream` object by storing all the provided parameters
         * (`condition`, `expressionString`, `functionName`, `fileName`, `lineNumber`)
         * as member variables. These stored values are used by the destructor to
         * perform the assertion check and report details if the assertion fails.
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
         * @brief Destructor for AssertStream.
         *
         * This is where the actual assertion check happens. If the `bCondition`
         * (stored from the constructor) is false, this destructor calls
         * `AssertManager::Assert` with the originally captured expression,
         * the accumulated message from `mStream`, and the context (function,
         * file, line).
         */
        ~AssertStream()
        {
            if (!bCondition)
                AssertManager::Assert(bCondition, mExpressionString,
                                      mStream.str(), mFunctionName,
                                      mFileName, mLineNumber);
        }
        /**
         * @brief Appends a message to the assertion stream.
         * @tparam T The type of the message to append.
         * @param message The message content to append to the stream.
         * @return A reference to the current AssertStream object, allowing for
         *         chained `<<` operations.
         *
         * This operator allows various data types to be streamed into the
         * assertion message. The content is appended to an internal
         * `std::ostringstream` (`mStream`). It returns `*this` to enable
         * chaining of `operator<<` calls (e.g., `ASSERT(x > 0) << "x must be positive, but was " << x;`).
         */
        template<typename T>
        AssertStream& operator<<(const T& message)
        {
            mStream << message;
            return *this;
        }
    
    private:
        bool bCondition; ///< Stores the boolean result of the assertion expression.
        const char* mExpressionString; ///< Stores the stringified assertion expression (e.g., "x == 10").
        const char* mFunctionName;     ///< Stores the name of the function where the assertion occurred.
        const char* mFileName;         ///< Stores the name of the file where the assertion occurred.
        const int mLineNumber;         ///< Stores the line number where the assertion occurred.
        std::ostringstream mStream;    ///< An `std::ostringstream` used to accumulate the custom assertion message parts.
    };
};

#endif

/* Public API */

#ifdef ASSERT
#undef ASSERT
#endif

/**
 * @def ASSERT(e)
 * @brief Main assertion macro. Users should use this to check conditions.
 *
 * In debug builds (when NDEBUG and _NDEBUG are not defined), this macro evaluates
 * the expression `e`. If `e` is false, it triggers an assertion failure, printing
 * the expression, source location (file, line, function), and any custom message
 * streamed into it.
 * The macro creates an `AssertManager::AssertStream` object, which captures the
 * boolean result of `e`, the string representation of `e` (`#e`), and the
 * context (`__PRETTY_FUNCTION__`, `__FILE__`, `__LINE__`).
 * This stream-based approach allows for appending custom messages:
 * Example: `ASSERT(ptr != nullptr) << "Pointer should not be null";`
 *
 * In release builds (when NDEBUG or _NDEBUG is defined), this macro compiles to
 * `if(false) std::cerr`. This effectively removes the assertion, and the
 * expression `e` is not evaluated. This ensures assertions do not impact
 * performance in production code.
 *
 * The function name is captured using `__PRETTY_FUNCTION__` (available on GCC/Clang)
 * or `__FUNCTION__` (a standard macro, used as a fallback if `__PRETTY_FUNCTION__`
 * is not available, e.g., on MSVC).
 */
#if !(defined(NDEBUG) || defined(_NDEBUG))
#define ASSERT(e) AssertManager::AssertStream(e, #e, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define ASSERT(e) if(false) std::cerr
#endif


