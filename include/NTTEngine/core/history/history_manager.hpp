#pragma once
#include <NTTEngine/defines.hpp>
#include "action.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    /**
     * Starting the history manager
     */
    void HistoryManager_Init();

    /**
     * Check the current history stack is empty or not
     */
    b8 HistoryManager_IsEmpty();

    /**
     * Clean all the history stack and the `NTT_HISTORY_EMPTY` event will be triggered
     *     if the origin history stack is not empty else nothing will be changed
     */
    void HistoryManager_Clear();

    /**
     * Add an action to the history stack. If the action can be executed
     *      then it is executed and be push into the stack, if not, nothing
     *      happend.
     *
     * If the current history stack is empty and the action can be executed
     *      then the `NTT_HISTORY_NOT_EMPTY` event will be triggered
     *
     * @param action: the action that will be added to the history stack
     *
     * @return TRUE if the action can be executed, otherwise
     */
    b8 HistoryManager_AddAction(Ref<Action> action);

    /**
     * Turn back the whole system to the previous state before the last action.
     * If the history stack is empty then nothing will be changed and the
     *      warning will be logged
     *
     * After undoing, if the history stack is empty then the `NTT_HISTORY_EMPTY`
     *      event will be triggered
     */
    void HistoryManager_Undo();

    /**
     * Release all the internal resources of the history manager
     */
    void HistoryManager_Shutdown();
} // namespace ntt
