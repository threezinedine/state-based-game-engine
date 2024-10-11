#include <NTTEngine/core/history/history.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <stack>

namespace ntt
{
    using namespace log;
    using namespace event;

    namespace
    {
        std::stack<Ref<Action>> s_actions;

        void ClearStack()
        {
            while (!s_actions.empty())
            {
                s_actions.pop();
            }
        }
    } // namespace

    void HistoryManager_Init()
    {
        ClearStack();
    }

    b8 HistoryManager_IsEmpty()
    {
        return s_actions.size() == 0;
    }

    void HistoryManager_Clear()
    {
        if (s_actions.size() == 0)
        {
            return;
        }

        ClearStack();
        TriggerEvent(NTT_HISTORY_EMPTY);
    }

    b8 HistoryManager_AddAction(Ref<Action> action)
    {
        if (!action->CanExecute())
        {
            return FALSE;
        }

        action->Execute();
        s_actions.push(action);

        if (s_actions.size() == 1)
        {
            TriggerEvent(NTT_HISTORY_NOT_EMPTY);
        }
        return TRUE;
    }

    void HistoryManager_Undo()
    {
        if (s_actions.size() == 0)
        {
            NTT_ENGINE_WARN("Cannot undo the action because the history stack is empty");
            return;
        }

        Ref<Action> action = s_actions.top();
        action->Undo();
        s_actions.pop();

        if (s_actions.size() == 0)
        {
            TriggerEvent(NTT_HISTORY_EMPTY);
        }
    }

    void HistoryManager_Shutdown()
    {
        ClearStack();
    }
} // namespace ntt
