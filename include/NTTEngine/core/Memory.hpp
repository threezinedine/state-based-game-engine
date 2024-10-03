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
    /**
     * Initialize all functionalities of the Memory module
     */
    void MemoryInit();

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
    using Scope = std::unique_ptr<T>;

    /**
     * The function to initialize a new unique pointer
     *
     * @param args The arguments to pass to the constructor of the object
     * @return The Scope pointer to the object
     */
    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

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

    constexpr Ref<T> CreateRef(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

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