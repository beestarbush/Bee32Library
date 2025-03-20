#ifdef CONFIG_HAL_DRIVER_SCD40
#include "SCD40.h"
#include <Arduino.h>

#define NO_ERROR 0
#define UPDATE_INTERVAL (5000)

namespace HAL::SCD40
{
    Configuration::Configuration(uint8_t address) :
        _address(address)
    {
    }

    Driver::Driver(const Configuration& configuration, I2C::Driver& i2c) :
        _configuration(configuration),
        _i2c(i2c),
        _sensor(),
        _serialNumber(0),
        _co2Concentration(0),
        _temperature(0),
        _relativeHumidity(0),
        _updateInterval(0)
    {
    }

    bool Driver::begin()
    {
        _sensor.begin(*_i2c.getWire(), _configuration._address);

        // Ensure sensor is in clean state
        delay(30);

        if (_sensor.wakeUp() != NO_ERROR)
        {
            Serial.print("Error trying to execute wakeUp(): ");
            return false;
        }
        if (_sensor.stopPeriodicMeasurement() != NO_ERROR)
        {
            Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
            return false;
        }
        if (_sensor.reinit() != NO_ERROR)
        {
            Serial.print("Error trying to execute reinit(): ");
            return false;
        }
        
        // Read out information about the sensor
        if (_sensor.getSerialNumber(_serialNumber) != NO_ERROR)
        {
            Serial.print("Error trying to execute getSerialNumber(): ");
            return false;
        }

        // If temperature offset and/or sensor altitude compensation
        // is required, you should call the respective functions here.
        // Check out the header file for the function definitions.
        // Start periodic measurements (5sec interval)
        if (_sensor.startPeriodicMeasurement())
        {
            Serial.print("Error trying to execute startPeriodicMeasurement(): ");
            return false;
        }
        //
        // If low-power mode is required, switch to the low power
        // measurement function instead of the standard measurement
        // function above. Check out the header file for the definition.
        // For SCD41, you can also check out the single shot measurement example.
        //

        return true;
    }

    void Driver::run()
    {
        // Slow down the sampling to 0.2Hz.
        uint64_t now = millis();
        if (now - _updateInterval < UPDATE_INTERVAL)
        {
            return;
        }
        _updateInterval = now;
        
        bool dataReady = false;
        if (_sensor.getDataReadyStatus(dataReady) != NO_ERROR)
        {
            Serial.print("Error trying to execute getDataReadyStatus(): ");
            return;
        }

        if (!dataReady)
        {
            Serial.println("Data not ready yet.");
            return;
        }
        
        // If ambient pressure compenstation during measurement
        // is required, you should call the respective functions here.
        // Check out the header file for the function definition.
        if (_sensor.readMeasurement(_co2Concentration, _temperature, _relativeHumidity) != NO_ERROR)
        {
            Serial.print("Error trying to execute readMeasurement(): ");
            return;
        }
    }

    void Driver::end()
    {
        if (_sensor.stopPeriodicMeasurement() != NO_ERROR)
        {
            Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        }
    }

    uint64_t Driver::getSerialNumber()
    {
        return _serialNumber;
    }

    uint16_t Driver::getCO2Concentration()
    {
        // CO2 concentration in ppm
        return _co2Concentration;
    }

    float Driver::getTemperature()
    {
        // Temperature in degree Celsius
        return _temperature;
    }

    float Driver::getRelativeHumidity()
    {
        // Relative humidity
        return _relativeHumidity;
    }
}
#endif