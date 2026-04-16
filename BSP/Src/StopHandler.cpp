/**
 * @file StopHandler.cpp
 * @brief Base implementation for STM32 Stop mode entry and wake source management.
 *
 * Implements clock restoration and wake source identification via EXTI
 * pending flags. Stop mode entry and wake source configuration are
 * delegated to derived classes.
 *
 * @note SystemClock_Config() is declared extern here as it is generated
 *       by CubeMX in Core/Src/main.c. Ensure it is not declared static.
 */
 
#include "StopHandler.hpp"
 
// CubeMX generated clock configuration — defined in Core/Src/main.c
extern "C" void SystemClock_Config(void);
 
namespace BSP {
 
// ----------------------------------------------------------------------------
// Protected methods
// ----------------------------------------------------------------------------
 
/**
 * @brief Restores system clocks after Stop mode exit.
 *
 * On wake from Stop mode the MCU runs on HSI with the PLL unlocked.
 * This must be called before any peripheral access to restore correct
 * clock frequencies and HAL tick timing.
 */
void StopHandler::restoreClocks() {
    SystemClock_Config();
 
    // Reinitialise HAL tick source — SysTick may have been affected
    // by the clock change during Stop mode entry
    HAL_InitTick(TICK_INT_PRIORITY);
}
 
/**
 * @brief Identifies the wake source by inspecting EXTI pending flags.
 *
 * Checks the configured GPIO wake pin and RTC alarm EXTI line flags
 * to determine what triggered the wakeup. Flags are cleared after
 * reading to prevent spurious re-entry.
 *
 * @return WakeSource::GPIO    if the configured GPIO/EXTI pin is pending.
 * @return WakeSource::RTC     if the RTC alarm EXTI line is pending.
 * @return WakeSource::Unknown if neither flag is set.
 */
WakeSource StopHandler::identifyWakeSource() {
    // Check GPIO EXTI flag first — most common wake source
    if (_wakePort != nullptr &&
        __HAL_GPIO_EXTI_GET_FLAG(_wakePin)) {
        __HAL_GPIO_EXTI_CLEAR_FLAG(_wakePin);
        return WakeSource::GPIO;
    }
 
    // Check RTC alarm EXTI line (line 17 on STM32C0)
    if (__HAL_RTC_ALARM_EXTI_GET_FLAG()) {
        __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
        return WakeSource::RTC;
    }
 
    return WakeSource::Unknown;
}
 
} // namespace BSP