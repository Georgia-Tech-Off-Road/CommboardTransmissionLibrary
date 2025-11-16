#ifndef CMBTL_TEST_SENSORS_H
#define CMBTL_TEST_SENSORS_H

#include "SensorInfo.h"
#include "SensorData.h"
#include "Packets/PacketInstructions.h"
#include <tuple>
#include <string>

using std::array;

namespace cmbtl {
namespace test {
    //--------------- Define Sensor Indexes ----------------------------------------------------
    enum SensorIndex {
        TIMESTAMP,
        TEMPERATURE,
        PRESSURE,
        RPM,
        VOLTAGE,
        STATUS
    };

    //--------------- Define Name Getter Functions ---------------------------------------------
    std::string TimeStampGetName() { return "timestamp"; }
    std::string TemperatureGetName() { return "temperature"; }
    std::string PressureGetName() { return "pressure"; }
    std::string RPMGetName() { return "rpm"; }
    std::string VoltageGetName() { return "voltage"; }
    std::string StatusGetName() { return "status"; }

    //--------------- Define Sensor Info Types -------------------------------------------------
    // TimeStamp - 32-bit unsigned integer
    using TimeStamp_SENSOR_INFO = DefaultSensorWithName<
        boost::endian::big_uint32_t,
        uint32_t,
        32,
        TimeStampGetName
    >;

    // Temperature - 20-bit unsigned float with 6 decimal bits
    using Temperature_SENSOR_INFO = SensorInfo<
        float,
        float,
        20,
        unsignedFloatEncode<20, 6>,
        unsignedFloatDecode<20, 6>,
        defaultConvert<float, float>,
        JSONSerializeWithName<TemperatureGetName>
    >;

    // Pressure - 21-bit signed float with 6 decimal bits (can be negative)
    using Pressure_SENSOR_INFO = SensorInfo<
        float,
        float,
        21,
        floatEncode<21, 6>,
        floatDecode<21, 6>,
        defaultConvert<float, float>,
        JSONSerializeWithName<PressureGetName>
    >;

    // RPM - 16-bit unsigned integer
    using RPM_SENSOR_INFO = DefaultSensorWithName<
        boost::endian::big_uint16_t,
        uint16_t,
        16,
        RPMGetName
    >;

    // Voltage - 16-bit unsigned float with 8 decimal bits (high precision)
    using Voltage_SENSOR_INFO = SensorInfo<
        float,
        float,
        16,
        unsignedFloatEncode<16, 8>,
        unsignedFloatDecode<16, 8>,
        defaultConvert<float, float>,
        JSONSerializeWithName<VoltageGetName>
    >;

    // Status - 1-bit boolean
    using Status_SENSOR_INFO = SensorInfo<
        bool,
        bool,
        1,
        boolEncode,
        boolDecode,
        defaultConvert<bool, bool>,
        JSONSerializeWithName<StatusGetName>
    >;

    //--------------- Define Sensor Info Tuple -------------------------------------------------
    // The order that the sensors appear in the enum and this tuple MUST BE THE SAME
    using TestSensorInfoTuple = std::tuple<
        TimeStamp_SENSOR_INFO,
        Temperature_SENSOR_INFO,
        Pressure_SENSOR_INFO,
        RPM_SENSOR_INFO,
        Voltage_SENSOR_INFO,
        Status_SENSOR_INFO
    >;

    //--------------- Define SensorData Type ---------------------------------------------------
    using TestSensorDataType = SensorData<TestSensorInfoTuple>;

    //--------------- Define Packet Instructions -----------------------------------------------
    // AllSensors - Contains all 6 sensors
    using AllSensorsPacket = packet::PacketInstructions<6>;
    inline AllSensorsPacket createAllSensorsPacket() {
        AllSensorsPacket packet;
        packet.set(TIMESTAMP);
        packet.set(TEMPERATURE);
        packet.set(PRESSURE);
        packet.set(RPM);
        packet.set(VOLTAGE);
        packet.set(STATUS);
        return packet;
    }

    // CriticalData - Contains TimeStamp, Temperature, Pressure, RPM (4 sensors)
    using CriticalDataPacket = packet::PacketInstructions<6>;
    inline CriticalDataPacket createCriticalDataPacket() {
        CriticalDataPacket packet;
        packet.set(TIMESTAMP);
        packet.set(TEMPERATURE);
        packet.set(PRESSURE);
        packet.set(RPM);
        return packet;
    }

    // DiagnosticData - Contains Voltage and Status (2 sensors)
    using DiagnosticDataPacket = packet::PacketInstructions<6>;
    inline DiagnosticDataPacket createDiagnosticDataPacket() {
        DiagnosticDataPacket packet;
        packet.set(VOLTAGE);
        packet.set(STATUS);
        return packet;
    }

} // namespace test
} // namespace cmbtl

#endif
