#pragma once

#include <array>
#include <memory>

#include "LockedStruct.hpp"
#include "DigitalOut.hpp"
#include "I2C.hpp"
#include "SPI.hpp"
#include "GenericModule.hpp"
#include "MicroPackets.hpp"
#include "drivers/MCP23017.hpp"

class LEDModule : public GenericModule {
public:
    // How many times per second this module should run
    static constexpr float kFrequency = 10.0f; // Hz
    static constexpr std::chrono::milliseconds kPeriod{static_cast<int>(1000 / kFrequency)};
    static constexpr int kPriority = 1;

    LEDModule(std::shared_ptr<MCP23017> ioExpander,
              LockedStruct<BatteryVoltage>& batteryVoltage,
              LockedStruct<FPGAStatus>& fpgaStatus,
              LockedStruct<KickerInfo>& kickerInfo,
              LockedStruct<RadioError>& radioError);

    virtual void entry(void);

    // Specific LED pattern for fpga initialization
    void fpgaInitialized();

    // Specific LED pattern for radio initialization
    void radioInitialized();

    // Specific LED pattern for kicker initialization
    void kickerInitialized();

    // Specific LED pattern for fully initialized system
    void fullyInitialized();

    // Specific toggling pattern for missing the X ms
    // super loop timings
    //
    // All LED's on mtrain are blinking together
    //
    // AKA, some module is too slow
    void missedSuperLoop();

    // Specific toggling pattern for missing a module run
    // X times in a row
    //
    // LED 1&3 toggle opposite of LED 2&4
    //
    // AKA, due to priority and timing, some module never runs
    void missedModuleRun();

private:
    /**
     * Sets the color of the two dot stars
     *
     * @param led0 0..7 red
     *             8..15 green
     *             16..23 blue
     *             24..31 don't care
     * 
     * @param led1 0..7 red
     *             8..15 green
     *             16..23 blue
     *             24..31 don't care
     */
    void setColor(uint32_t led0, uint32_t led1);

    const static uint16_t IOExpanderErrorLEDMask = 0xFF00;

    LockedStruct<BatteryVoltage>& batteryVoltage;
    LockedStruct<FPGAStatus>& fpgaStatus;
    LockedStruct<KickerInfo>& kickerInfo;
    LockedStruct<RadioError>& radioError;

    // Dot stars were removed so we could use their SPI
    // bus for the kicker
    // On startup, the kicker was pulling the spi lines
    // incorrectly that caused the fpga to not boot correctly
    // Add back in once opto-isolators are added to the control
    // boards again
    // - Joe Aug 2019
    //SPI dot_star_spi;

    std::shared_ptr<MCP23017> ioExpander;

    std::array<DigitalOut, 4> leds;
    bool missedSuperLoopToggle;
    bool missedModuleRunToggle;
};