#pragma once
#include <memory>
#include <NTTEngine/defines.hpp>
#include <functional>

/**
 * The Memory module provides functionalities for dealing with
 *      object allocation and deallocation in the engine.
 * Users do not need to delete the object manually, the engine will handle it.
 * The Memory module provides 2 types of smart pointers:
 *      - Scope pointer: The object can be owned by only 1 context.
 *      - Ref pointer: The object can be owned by multiple contexts.
 *
 * Provides the mechanism for checking memory leaks in the engine.
 */
namespace ntt::memory
{
    struct PointerInfo
    {
        const char *file; ///< The file which the pointer is created
        u16 line;         ///< The line which the pointer is created
        u32 size;         ///< The size of the pointer
    };

    /**
     * Initialize all functionalities of the Memory module
     */
    void MemoryInit();

    // /**
    //  * Store needed information for each pointer which is
    //  *      created by the engine
    //  */
    // void RegisterPointer(void *ptr, const PointerInfo &info);

    // /**
    //  * Remove the information of the pointer if it is deleted
    //  */
    // void UnregisterPointer(void *ptr);

    /**
     * The function which is called when a new object is created (should be used for
     *      checking memory leaks)
     */
    void AllocateCalled();

    /**
     * The function which is called when an object is deleted (should be used for
     *      checking memory leaks)
     */
    void DeallocateCalled();

    /**
     * The Scope pointer which can be owned by only 1 context,
     *      when the ownership of the object is moved, the object will be deleted
     */
    template <typename T>
    // #ifdef _DEBUG
    //     using Scope = std::unique_ptr<T, std::function<void(T *)>>;
    // #else
    using Scope = std::unique_ptr<T>;
    // #endif

    /**
     * The function to initialize a new unique pointer
     *
     * @param args The arguments to pass to the constructor of the object
     * @return The Scope pointer to the object
     */
    template <typename T, typename... Args>
    // #ifdef _DEBUG
    //     constexpr Scope<T> CreateScope(const char *file, u16 line, Args &&...args)
    //     {
    //         T *ptr = new T(std::forward<Args>(args)...);
    //         RegisterPointer(ptr, {file, line, sizeof(T)});
    //         return ::std::unique_ptr<T, void (*)(T *)>(ptr, [](T *ptr)
    //                                                    {
    //             UnregisterPointer(ptr);
    //             delete ptr; });
    //     }
    // #define CREATE_SCOPE(type, ...) CreateScope<type>(__FILE__, __LINE__, ##__VA_ARGS__)
    // #else
    constexpr Scope<T> CreateScope(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
#define CREATE_SCOPE(type, ...) CreateScope<type>(__VA_ARGS__)
    // #endif

    /**
     * The shared pointer which can be owned by multiple contexts,
     *      when the ownership of the object is moved, the object will remain
     */
    template <typename T>
    using Ref = std::shared_ptr<T>;

    /**
     * The function to initialize a new shared pointer
     *
     * @param args The arguments to pass to the constructor of the object
     * @return The Ref pointer to the object
     */
    template <typename T, typename... Args>
    // #ifdef _DEBUG
    //     constexpr Ref<T> CreateRef(const char *message, u16 line, Args &&...args)
    //     {
    //         auto ptr = new T(std::forward<Args>(args)...);
    //         RegisterPointer(ptr, {message, line, sizeof(T)});
    //         return ::std::shared_ptr<T>(ptr, [](T *ptr)
    //                                     {
    //             UnregisterPointer(ptr);
    //             delete ptr; });
    //     }
    // #define CREATE_REF(type, ...) CreateRef<type>(__FILE__, __LINE__, ##__VA_ARGS__)
    // #else
    constexpr Ref<T> CreateRef(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

#define CREATE_REF(type, ...) CreateRef<type>(__VA_ARGS__)
    // #endif

    /**
     * End all functionalities of the Memory module
     */
    void MemoryShutdown();
} // namespace ntt

#ifdef _DEBUG
#define CREATE_NEW() AllocateCalled()
#define DELETE_OBJ() DeallocateCalled()
#else
#define CREATE_NEW()
#define DELETE_OBJ()
#endif