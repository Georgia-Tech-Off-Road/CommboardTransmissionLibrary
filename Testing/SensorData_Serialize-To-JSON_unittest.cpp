#include "SensorInfo.h"
#include "SensorData.h"
#include "TestSensors.h"
#include "gtest/gtest.h"
#include "boost_endian_arithmetic.hpp"
#include "Packets/PacketInstructions.h"
#include <cmath>
#include <iostream>
#include <cstring>

using namespace cmbtl;
using namespace cmbtl::test;

// ================================================================================================
// Tests for serializeDataToJSON
// ================================================================================================

// Test JSON serialization of all sensor data
TEST(SensorDataTests, SerializeDataToJSON) {
    TestSensorDataType sensorData;

    // Set test values for all sensors
    sensorData.setData<TIMESTAMP>(1234567890);
    sensorData.setData<TEMPERATURE>(75.5f);
    sensorData.setData<PRESSURE>(-10.25f);
    sensorData.setData<RPM>(3500);
    sensorData.setData<VOLTAGE>(13.8f);
    sensorData.setData<STATUS>(true);

    // Serialize to JSON
    std::string json = sensorData.serializeDataToJSON();

    // Verify JSON contains sensor names (basic check)
    ASSERT_NE(json.find("timestamp"), std::string::npos);
    ASSERT_NE(json.find("temperature"), std::string::npos);
    ASSERT_NE(json.find("pressure"), std::string::npos);
    ASSERT_NE(json.find("rpm"), std::string::npos);
    ASSERT_NE(json.find("voltage"), std::string::npos);
    ASSERT_NE(json.find("status"), std::string::npos);

    // Verify JSON structure (should start with { and end with })
    ASSERT_EQ(json.front(), '{');
    ASSERT_EQ(json.back(), '}');

    // Output for manual inspection (optional)
    std::cout << "Full JSON:\n" << json << std::endl;
}

// ================================================================================================
// Tests for serializeDataToJSONPacket
// ================================================================================================

// Test JSON serialization of AllSensors packet
TEST(SensorDataTests, SerializeAllSensorsPacketToJSON) {
    TestSensorDataType sensorData;

    // Set test values for all sensors
    sensorData.setData<TIMESTAMP>(1234567890);
    sensorData.setData<TEMPERATURE>(75.5f);
    sensorData.setData<PRESSURE>(-10.25f);
    sensorData.setData<RPM>(3500);
    sensorData.setData<VOLTAGE>(13.8f);
    sensorData.setData<STATUS>(true);

    // Create packet instructions
    AllSensorsPacket packet = createAllSensorsPacket();

    // Serialize to JSON
    std::string json = sensorData.serializeDataToJSONPacket(packet);

    // Verify JSON contains all sensor names
    ASSERT_NE(json.find("timestamp"), std::string::npos);
    ASSERT_NE(json.find("temperature"), std::string::npos);
    ASSERT_NE(json.find("pressure"), std::string::npos);
    ASSERT_NE(json.find("rpm"), std::string::npos);
    ASSERT_NE(json.find("voltage"), std::string::npos);
    ASSERT_NE(json.find("status"), std::string::npos);

    // Verify JSON structure
    ASSERT_EQ(json.front(), '{');
    ASSERT_EQ(json.back(), '}');

    // Output for manual inspection (optional)
    std::cout << "AllSensors JSON:\n" << json << std::endl;
}

// Test JSON serialization of CriticalData packet
TEST(SensorDataTests, SerializeCriticalDataPacketToJSON) {
    TestSensorDataType sensorData;

    // Set test values for all sensors
    sensorData.setData<TIMESTAMP>(1234567890);
    sensorData.setData<TEMPERATURE>(75.5f);
    sensorData.setData<PRESSURE>(-10.25f);
    sensorData.setData<RPM>(3500);
    sensorData.setData<VOLTAGE>(13.8f);
    sensorData.setData<STATUS>(true);

    // Create packet instructions
    CriticalDataPacket packet = createCriticalDataPacket();

    // Serialize to JSON
    std::string json = sensorData.serializeDataToJSONPacket(packet);

    // Verify JSON contains only critical sensor names
    ASSERT_NE(json.find("timestamp"), std::string::npos);
    ASSERT_NE(json.find("temperature"), std::string::npos);
    ASSERT_NE(json.find("pressure"), std::string::npos);
    ASSERT_NE(json.find("rpm"), std::string::npos);

    // Verify JSON does NOT contain diagnostic sensor names
    ASSERT_EQ(json.find("voltage"), std::string::npos);
    ASSERT_EQ(json.find("status"), std::string::npos);

    // Verify JSON structure
    ASSERT_EQ(json.front(), '{');
    ASSERT_EQ(json.back(), '}');

    // Output for manual inspection (optional)
    std::cout << "CriticalData JSON:\n" << json << std::endl;
}

// Test JSON serialization of DiagnosticData packet
TEST(SensorDataTests, SerializeDiagnosticDataPacketToJSON) {
    TestSensorDataType sensorData;

    // Set test values for all sensors
    sensorData.setData<TIMESTAMP>(1234567890);
    sensorData.setData<TEMPERATURE>(75.5f);
    sensorData.setData<PRESSURE>(-10.25f);
    sensorData.setData<RPM>(3500);
    sensorData.setData<VOLTAGE>(13.8f);
    sensorData.setData<STATUS>(true);

    // Create packet instructions
    DiagnosticDataPacket packet = createDiagnosticDataPacket();

    // Serialize to JSON
    std::string json = sensorData.serializeDataToJSONPacket(packet);

    // Verify JSON contains only diagnostic sensor names
    ASSERT_NE(json.find("voltage"), std::string::npos);
    ASSERT_NE(json.find("status"), std::string::npos);

    // Verify JSON does NOT contain critical sensor names
    ASSERT_EQ(json.find("timestamp"), std::string::npos);
    ASSERT_EQ(json.find("temperature"), std::string::npos);
    ASSERT_EQ(json.find("pressure"), std::string::npos);
    ASSERT_EQ(json.find("rpm"), std::string::npos);

    // Verify JSON structure
    ASSERT_EQ(json.front(), '{');
    ASSERT_EQ(json.back(), '}');

    // Output for manual inspection (optional)
    std::cout << "DiagnosticData JSON:\n" << json << std::endl;
}