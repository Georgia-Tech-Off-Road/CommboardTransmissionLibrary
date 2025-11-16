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

/* This file contains test pertainting to the read/record data cycle. 
    More specifically it test the following functions:
     - printCofig()
     - recordData()
     - readData()
     - getRecordedDataSize()
*/


// ================================================================================================
// Tests for printConfiguration
// ================================================================================================

// Test configuration printing with all sensors
TEST(SensorDataTests, PrintConfigurationAllSensors) {
    TestSensorDataType sensorData;

    // Print configuration to stringstream
    std::stringstream ss;
    sensorData.printConfig(createAllSensorsPacket(), ss);
    std::string configOutput = ss.str();

    // Print for manual inspection
    std::cout << "\n=== All Sensors Configuration ===\n" << configOutput << std::endl;

    // Verify CSV header is present
    ASSERT_NE(configOutput.find("sensor name, type, bits"), std::string::npos);

    // Verify all sensor names are present (without quotes)
    ASSERT_NE(configOutput.find("timestamp"), std::string::npos);
    ASSERT_NE(configOutput.find("temperature"), std::string::npos);
    ASSERT_NE(configOutput.find("pressure"), std::string::npos);
    ASSERT_NE(configOutput.find("rpm"), std::string::npos);
    ASSERT_NE(configOutput.find("voltage"), std::string::npos);
    ASSERT_NE(configOutput.find("status"), std::string::npos);

    // Verify no quotes around sensor names
    ASSERT_EQ(configOutput.find("\"timestamp\""), std::string::npos);
    ASSERT_EQ(configOutput.find("\"temperature\""), std::string::npos);
    ASSERT_EQ(configOutput.find("\"pressure\""), std::string::npos);

    // Verify bit sizes are present
    ASSERT_NE(configOutput.find("32"), std::string::npos);  // timestamp
    ASSERT_NE(configOutput.find("20"), std::string::npos);  // temperature
    ASSERT_NE(configOutput.find("21"), std::string::npos);  // pressure
    ASSERT_NE(configOutput.find("16"), std::string::npos);  // rpm & voltage
    ASSERT_NE(configOutput.find("1"), std::string::npos);   // status
}

// Test configuration printing with critical data packet only
TEST(SensorDataTests, PrintConfigurationCriticalDataOnly) {
    TestSensorDataType sensorData;

    // Print configuration to stringstream
    std::stringstream ss;
    sensorData.printConfig(createCriticalDataPacket(), ss);
    std::string configOutput = ss.str();

    // Print for manual inspection
    std::cout << "\n=== Critical Data Configuration ===\n" << configOutput << std::endl;

    // Verify CSV header is present
    ASSERT_NE(configOutput.find("sensor name, type, bits"), std::string::npos);

    // Verify only critical sensor names are present
    ASSERT_NE(configOutput.find("timestamp"), std::string::npos);
    ASSERT_NE(configOutput.find("temperature"), std::string::npos);
    ASSERT_NE(configOutput.find("pressure"), std::string::npos);
    ASSERT_NE(configOutput.find("rpm"), std::string::npos);

    // Verify diagnostic sensors are NOT present
    ASSERT_EQ(configOutput.find("voltage"), std::string::npos);
    ASSERT_EQ(configOutput.find("status"), std::string::npos);
}

// Test configuration printing with diagnostic data packet only
TEST(SensorDataTests, PrintConfigurationDiagnosticDataOnly) {
    TestSensorDataType sensorData;

    // Print configuration to stringstream
    std::stringstream ss;
    sensorData.printConfig(createDiagnosticDataPacket(), ss);
    std::string configOutput = ss.str();

    // Print for manual inspection
    std::cout << "\n=== Diagnostic Data Configuration ===\n" << configOutput << std::endl;

    // Verify CSV header is present
    ASSERT_NE(configOutput.find("sensor name, type, bits"), std::string::npos);

    // Verify only diagnostic sensor names are present
    ASSERT_NE(configOutput.find("voltage"), std::string::npos);
    ASSERT_NE(configOutput.find("status"), std::string::npos);

    // Verify critical sensors are NOT present
    ASSERT_EQ(configOutput.find("timestamp"), std::string::npos);
    ASSERT_EQ(configOutput.find("temperature"), std::string::npos);
    ASSERT_EQ(configOutput.find("pressure"), std::string::npos);
    ASSERT_EQ(configOutput.find("rpm"), std::string::npos);
}

// ================================================================================================
// Tests for getRecordedDataSize() for each packet type
// ================================================================================================

// Test getRecordedDataSize() for AllSensors packet
TEST(SensorDataRecordReadTests, GetRecordedDataSizeAllSensors) {
    TestSensorDataType sensorData;
    AllSensorsPacket packet = createAllSensorsPacket();

    size_t expectedSize = sizeof(boost::endian::big_uint32_t)  // TIMESTAMP
                        + sizeof(float)                         // TEMPERATURE
                        + sizeof(float)                         // PRESSURE
                        + sizeof(boost::endian::big_uint16_t)   // RPM
                        + sizeof(float)                         // VOLTAGE
                        + sizeof(bool);                         // STATUS

    size_t actualSize = sensorData.getRecordedDataSize(packet);

    ASSERT_EQ(actualSize, expectedSize);
}

// Test getRecordedDataSize() for CriticalData packet
TEST(SensorDataRecordReadTests, GetRecordedDataSizeCriticalData) {
    TestSensorDataType sensorData;
    CriticalDataPacket packet = createCriticalDataPacket();

    size_t expectedSize = sizeof(boost::endian::big_uint32_t)  // TIMESTAMP
                        + sizeof(float)                         // TEMPERATURE
                        + sizeof(float)                         // PRESSURE
                        + sizeof(boost::endian::big_uint16_t);  // RPM

    size_t actualSize = sensorData.getRecordedDataSize(packet);

    ASSERT_EQ(actualSize, expectedSize);
}

// Test getRecordedDataSize() for DiagnosticData packet
TEST(SensorDataRecordReadTests, GetRecordedDataSizeDiagnosticData) {
    TestSensorDataType sensorData;
    DiagnosticDataPacket packet = createDiagnosticDataPacket();

    size_t expectedSize = sizeof(float)     // VOLTAGE
                        + sizeof(bool);     // STATUS

    size_t actualSize = sensorData.getRecordedDataSize(packet);

    ASSERT_EQ(actualSize, expectedSize);
}

// ================================================================================================
// Tests for recordData() and readData() cycle for each packet type
// ================================================================================================

// Test record/read data cycle for AllSensors packet
TEST(SensorDataRecordReadTests, RecordReadCycleAllSensors) {
    TestSensorDataType sensorData;

    // Set test values for all sensors
    sensorData.setData<TIMESTAMP>(1234567890);
    sensorData.setData<TEMPERATURE>(75.5f);
    sensorData.setData<PRESSURE>(-10.25f);
    sensorData.setData<RPM>(3500);
    sensorData.setData<VOLTAGE>(13.8f);
    sensorData.setData<STATUS>(true);

    AllSensorsPacket packet = createAllSensorsPacket();

    // Allocate buffer
    size_t bufferSize = sensorData.getRecordedDataSize(packet);
    unsigned char* buffer = new unsigned char[bufferSize];

    // Record data
    sensorData.recordData(buffer, packet);

    // Create new sensor data and read
    TestSensorDataType readSensorData;
    readSensorData.readData(buffer, packet);

    // Verify all sensor values
    ASSERT_EQ(readSensorData.getData<TIMESTAMP>(), sensorData.getData<TIMESTAMP>());
    ASSERT_EQ(readSensorData.getData<TEMPERATURE>(), sensorData.getData<TEMPERATURE>());
    ASSERT_EQ(readSensorData.getData<PRESSURE>(), sensorData.getData<PRESSURE>());
    ASSERT_EQ(readSensorData.getData<RPM>(), sensorData.getData<RPM>());
    ASSERT_EQ(readSensorData.getData<VOLTAGE>(), sensorData.getData<VOLTAGE>());
    ASSERT_EQ(readSensorData.getData<STATUS>(), sensorData.getData<STATUS>());

    delete[] buffer;
}

// Test record/read data cycle for CriticalData packet
TEST(SensorDataRecordReadTests, RecordReadCycleCriticalData) {
    TestSensorDataType sensorData;

    // Set test values for critical sensors
    sensorData.setData<TIMESTAMP>(9876543);
    sensorData.setData<TEMPERATURE>(105.25f);
    sensorData.setData<PRESSURE>(20.5f);
    sensorData.setData<RPM>(7200);

    CriticalDataPacket packet = createCriticalDataPacket();

    // Allocate buffer
    size_t bufferSize = sensorData.getRecordedDataSize(packet);
    unsigned char* buffer = new unsigned char[bufferSize];

    // Record data
    sensorData.recordData(buffer, packet);

    // Create new sensor data and read
    TestSensorDataType readSensorData;
    readSensorData.readData(buffer, packet);

    // Verify critical sensor values
    ASSERT_EQ(readSensorData.getData<TIMESTAMP>(), sensorData.getData<TIMESTAMP>());
    ASSERT_EQ(readSensorData.getData<TEMPERATURE>(), sensorData.getData<TEMPERATURE>());
    ASSERT_EQ(readSensorData.getData<PRESSURE>(), sensorData.getData<PRESSURE>());
    ASSERT_EQ(readSensorData.getData<RPM>(), sensorData.getData<RPM>());

    delete[] buffer;
}

// Test record/read data cycle for DiagnosticData packet
TEST(SensorDataRecordReadTests, RecordReadCycleDiagnosticData) {
    TestSensorDataType sensorData;

    // Set test values for diagnostic sensors
    sensorData.setData<VOLTAGE>(5.123f);
    sensorData.setData<STATUS>(false);

    DiagnosticDataPacket packet = createDiagnosticDataPacket();

    // Allocate buffer
    size_t bufferSize = sensorData.getRecordedDataSize(packet);
    unsigned char* buffer = new unsigned char[bufferSize];

    // Record data
    sensorData.recordData(buffer, packet);

    // Create new sensor data and read
    TestSensorDataType readSensorData;
    readSensorData.readData(buffer, packet);

    // Verify diagnostic sensor values
    ASSERT_EQ(readSensorData.getData<VOLTAGE>(), sensorData.getData<VOLTAGE>());
    ASSERT_EQ(readSensorData.getData<STATUS>(), sensorData.getData<STATUS>());

    delete[] buffer;
}

// ================================================================================================
// Test recording and reading six different SensorData states sequentially
// ================================================================================================

TEST(SensorDataRecordReadTests, RecordReadSixSequentialStates) {
    // Create six different sensor data states
    TestSensorDataType state1, state2, state3, state4, state5, state6;

    // State 1: Morning warmup
    state1.setData<TIMESTAMP>(1000000);
    state1.setData<TEMPERATURE>(15.0f);
    state1.setData<PRESSURE>(5.5f);
    state1.setData<RPM>(1000);
    state1.setData<VOLTAGE>(12.5f);
    state1.setData<STATUS>(true);

    // State 2: Acceleration
    state2.setData<TIMESTAMP>(2000000);
    state2.setData<TEMPERATURE>(45.5f);
    state2.setData<PRESSURE>(15.25f);
    state2.setData<RPM>(4500);
    state2.setData<VOLTAGE>(13.2f);
    state2.setData<STATUS>(true);

    // State 3: Cruising
    state3.setData<TIMESTAMP>(3000000);
    state3.setData<TEMPERATURE>(65.0f);
    state3.setData<PRESSURE>(10.0f);
    state3.setData<RPM>(3000);
    state3.setData<VOLTAGE>(13.8f);
    state3.setData<STATUS>(true);

    // State 4: High performance
    state4.setData<TIMESTAMP>(4000000);
    state4.setData<TEMPERATURE>(95.75f);
    state4.setData<PRESSURE>(25.5f);
    state4.setData<RPM>(8000);
    state4.setData<VOLTAGE>(14.1f);
    state4.setData<STATUS>(true);

    // State 5: Deceleration
    state5.setData<TIMESTAMP>(5000000);
    state5.setData<TEMPERATURE>(85.25f);
    state5.setData<PRESSURE>(-5.0f);
    state5.setData<RPM>(2000);
    state5.setData<VOLTAGE>(13.5f);
    state5.setData<STATUS>(true);

    // State 6: Cooldown with error
    state6.setData<TIMESTAMP>(6000000);
    state6.setData<TEMPERATURE>(50.0f);
    state6.setData<PRESSURE>(0.0f);
    state6.setData<RPM>(800);
    state6.setData<VOLTAGE>(12.0f);
    state6.setData<STATUS>(false);

    // Create packet instruction for all sensors
    AllSensorsPacket packet = createAllSensorsPacket();

    // Calculate buffer size for one state
    size_t singleStateSize = state1.getRecordedDataSize(packet);

    // Allocate buffer for six states
    size_t totalBufferSize = singleStateSize * 6;
    unsigned char* buffer = new unsigned char[totalBufferSize];

    // Record all six states sequentially
    state1.recordData(buffer + (0 * singleStateSize), packet);
    state2.recordData(buffer + (1 * singleStateSize), packet);
    state3.recordData(buffer + (2 * singleStateSize), packet);
    state4.recordData(buffer + (3 * singleStateSize), packet);
    state5.recordData(buffer + (4 * singleStateSize), packet);
    state6.recordData(buffer + (5 * singleStateSize), packet);

    // Read back all six states sequentially
    TestSensorDataType read1, read2, read3, read4, read5, read6;

    read1.readData(buffer + (0 * singleStateSize), packet);
    read2.readData(buffer + (1 * singleStateSize), packet);
    read3.readData(buffer + (2 * singleStateSize), packet);
    read4.readData(buffer + (3 * singleStateSize), packet);
    read5.readData(buffer + (4 * singleStateSize), packet);
    read6.readData(buffer + (5 * singleStateSize), packet);

    // Verify State 1
    ASSERT_EQ(read1.getData<TIMESTAMP>(), state1.getData<TIMESTAMP>());
    ASSERT_EQ(read1.getData<TEMPERATURE>(), state1.getData<TEMPERATURE>());
    ASSERT_EQ(read1.getData<PRESSURE>(), state1.getData<PRESSURE>());
    ASSERT_EQ(read1.getData<RPM>(), state1.getData<RPM>());
    ASSERT_EQ(read1.getData<VOLTAGE>(), state1.getData<VOLTAGE>());
    ASSERT_EQ(read1.getData<STATUS>(), state1.getData<STATUS>());

    // Verify State 2
    ASSERT_EQ(read2.getData<TIMESTAMP>(), state2.getData<TIMESTAMP>());
    ASSERT_EQ(read2.getData<TEMPERATURE>(), state2.getData<TEMPERATURE>());
    ASSERT_EQ(read2.getData<PRESSURE>(), state2.getData<PRESSURE>());
    ASSERT_EQ(read2.getData<RPM>(), state2.getData<RPM>());
    ASSERT_EQ(read2.getData<VOLTAGE>(), state2.getData<VOLTAGE>());
    ASSERT_EQ(read2.getData<STATUS>(), state2.getData<STATUS>());

    // Verify State 3
    ASSERT_EQ(read3.getData<TIMESTAMP>(), state3.getData<TIMESTAMP>());
    ASSERT_EQ(read3.getData<TEMPERATURE>(), state3.getData<TEMPERATURE>());
    ASSERT_EQ(read3.getData<PRESSURE>(), state3.getData<PRESSURE>());
    ASSERT_EQ(read3.getData<RPM>(), state3.getData<RPM>());
    ASSERT_EQ(read3.getData<VOLTAGE>(), state3.getData<VOLTAGE>());
    ASSERT_EQ(read3.getData<STATUS>(), state3.getData<STATUS>());

    // Verify State 4
    ASSERT_EQ(read4.getData<TIMESTAMP>(), state4.getData<TIMESTAMP>());
    ASSERT_EQ(read4.getData<TEMPERATURE>(), state4.getData<TEMPERATURE>());
    ASSERT_EQ(read4.getData<PRESSURE>(), state4.getData<PRESSURE>());
    ASSERT_EQ(read4.getData<RPM>(), state4.getData<RPM>());
    ASSERT_EQ(read4.getData<VOLTAGE>(), state4.getData<VOLTAGE>());
    ASSERT_EQ(read4.getData<STATUS>(), state4.getData<STATUS>());

    // Verify State 5
    ASSERT_EQ(read5.getData<TIMESTAMP>(), state5.getData<TIMESTAMP>());
    ASSERT_EQ(read5.getData<TEMPERATURE>(), state5.getData<TEMPERATURE>());
    ASSERT_EQ(read5.getData<PRESSURE>(), state5.getData<PRESSURE>());
    ASSERT_EQ(read5.getData<RPM>(), state5.getData<RPM>());
    ASSERT_EQ(read5.getData<VOLTAGE>(), state5.getData<VOLTAGE>());
    ASSERT_EQ(read5.getData<STATUS>(), state5.getData<STATUS>());

    // Verify State 6
    ASSERT_EQ(read6.getData<TIMESTAMP>(), state6.getData<TIMESTAMP>());
    ASSERT_EQ(read6.getData<TEMPERATURE>(), state6.getData<TEMPERATURE>());
    ASSERT_EQ(read6.getData<PRESSURE>(), state6.getData<PRESSURE>());
    ASSERT_EQ(read6.getData<RPM>(), state6.getData<RPM>());
    ASSERT_EQ(read6.getData<VOLTAGE>(), state6.getData<VOLTAGE>());
    ASSERT_EQ(read6.getData<STATUS>(), state6.getData<STATUS>());

    delete[] buffer;
}
