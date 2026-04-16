#pragma once
 
#include "stm32c0xx_hal.h"
#include <cstdint>
 
namespace BSP {
 
/**
 * @file StopHandler.hpp
 * @brief Base class for STM32 Stop mode entry and wake source management.
 *
 * Provides a hardware-level abstraction for entering STM32 Stop mode and
 * identifying the wake source on exit. This class is intentionally thin —
 * it handles only the mechanics of Stop mode entry, clock restoration, and
 * wake source reporting. Sleep preparation policy (e.g. CAN bus teardown)
 * is the responsibility of the caller.
 *
 * Derived classes extend this interface to add wake source configuration
 * specific to their node role (e.g. RTC wake for core nodes, GPIO-only
 * for peripheral nodes).
 *
 * @note Both VDD and VIO must remain powered during Stop mode for the
 *       CAN transceiver wake filter to remain active.
 */
 
/**
 * @brief Identifies the hardware source that caused the MCU to wake
 *        from Stop mode.
 */
enum class WakeSource : uint8_t {
    GPIO,       ///< Wake triggered by GPIO/EXTI line (e.g. CAN RXD dominant pulse)
    RTC,        ///< Wake triggered by RTC wakeup timer
    Unknown     ///< Wake source could not be determined
};
 
/**
 * @brief Base Stop mode handler.
 *
 * Owns the Stop mode entry call, clock restoration on wake, and wake
 * source identification. Subclasses are responsible for configuring
 * their specific wake sources before calling enter().
 *
 * This class is non-copyable and non-movable as it represents a singular
 * hardware resource.
 */
class StopHandler {
public:
 
    /**
     * @brief Default constructor.
     */
    StopHandler() = default;
 
    /**
     * @brief Virtual destructor.
     */
    virtual ~StopHandler() = default;
 
    // Non-copyable, non-movable
    StopHandler(const StopHandler&)             = delete;
    StopHandler& operator=(const StopHandler&)  = delete;
    StopHandler(StopHandler&&)                  = delete;
    StopHandler& operator=(StopHandler&&)       = delete;
 
    /**
     * @brief Configure a GPIO pin as a wakeup source via EXTI.
     *
     * The specified pin must be EXTI-capable and should be connected to
     * the CAN transceiver RXD output. A falling edge (recessive to dominant
     * transition) will trigger the wakeup.
     *
     * Must be called before enter() for GPIO wake to function.
     *
     * @param port      Pointer to the GPIO port (e.g. GPIOA).
     * @param pin       GPIO pin mask (e.g. GPIO_PIN_0).
     */
    virtual void configureGPIOWake(GPIO_TypeDef* port, uint16_t pin) = 0;
 
    /**
     * @brief Enter STM32 Stop mode.
     *
     * Halts the CPU and gates most clocks. RAM contents and register state
     * are preserved. Execution resumes at the instruction following this
     * call after a configured wake source fires.
     *
     * On return, this method guarantees:
     *  - System clocks have been restored via SystemClock_Config()
     *  - The wake source has been identified and is available via
     *    lastWakeSource()
     *
     * @note The caller is responsible for all peripheral teardown and
     *       restoration around this call (e.g. FDCAN stop/start,
     *       transceiver STBY assertion).
     *
     * @return The WakeSource that caused the MCU to exit Stop mode.
     */
    virtual WakeSource enter() = 0;
 
    /**
     * @brief Returns the wake source from the most recent Stop mode exit.
     *
     * Valid after enter() returns. Returns WakeSource::Unknown if enter()
     * has not yet been called.
     *
     * @return WakeSource identifying what triggered the last wakeup.
     */
    WakeSource lastWakeSource() const { return _lastWakeSource; }
 
protected:
 
    /**
     * @brief Restores system clocks after Stop mode exit.
     *
     * Must be called immediately on wake, before any peripheral access.
     * HSI is running on wake but PLL is not relocked — peripheral timing
     * (including FDCAN baud rate) will be incorrect until this returns.
     */
    void restoreClocks();
 
    /**
     * @brief Identifies the wake source by inspecting hardware flags.
     *
     * Called internally by enter() implementations after wakeup to
     * populate _lastWakeSource.
     *
     * @return The identified WakeSource.
     */
    WakeSource identifyWakeSource();
 
    /// Wake source from the most recent Stop mode exit.
    WakeSource _lastWakeSource { WakeSource::Unknown };
};
 
} // namespace BSP