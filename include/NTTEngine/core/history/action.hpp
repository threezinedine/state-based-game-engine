#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    class Action
    {
    public:
        virtual ~Action() = default;

        /**
         * Check the availability of the action.
         *
         * @return TRUE if the action can be executed, FALSE otherwise
         */
        virtual b8 CanExecute() = 0;

        /**
         * Execute the action. Should check the availability of the action
         *
         * Example:
         * ```
         *      if (action->CanExecute())
         *      {
         *          action->Execute();
         *      }
         * ```
         */
        virtual void Execute() = 0;

        /**
         * Used for reset the previous state of the action.
         */
        virtual void Undo() = 0;
    };
} // namespace ntt
