#pragma once

/**
 * The core layer contains almost based functionalities for the whole engine,
 *      these functionalities are self-defined and must be used throughout the engine,
 *      while other default module will be hidden from the code inside the engine.
 *      Other third-party modules must have the adapter through this core layer
 *      (except special cases like test)
 *  - Logger
 *  - Assertion
 *  - String manipulation
 *  - Memory management
 *  - Async File I/O
 *  - Profiling
 *  - Data Parser
 *  - Engine Configurations
 *  - Math Libraries
 *  - Data Structures like Array, List, Map, etc.
 *  - Random Number Generator
 *  - Language Localization
 *  - Time Utils
 *
 */

#include "memory.hpp"
#include "logging.hpp"
#include "assertion.hpp"
#include "time.hpp"
#include "random.hpp"
#include "profiling.hpp"