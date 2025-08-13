# Introduction
Welcome to GTOR's Commboard Transmission Library. This library is designed to help store sensor data and compress to reduce transmitted packet sizes. It can also convert the data to a JSON format for easy storage and or graphing. This library is designed for specifically for BAJA SAE Offroad competition, but we feel it could be useful for a wide variety of other purposes! This library is open source, and we would love for other teams or individuals to use, contribute, and or provide feedback to this piece of software. If you have any questions, do not hestitate to contact us.

# How to Use
This library is designed as an Arduino header only library. We have provided our usage of it in the src/DAQInfrastructure directory. It uses heavily C++'s template features for Template Meta Programming ("TMP"). The goal of this is to allow the user, in one place, define the common context of their sensors: data type, compression functions, etc., and have the necessary changes to accomodate this be carried out by the program. TMP can become quickly convoluted (chiefly because it depth of usage was never an intended language feature), but in essence, it allows the programmer to specify create classes, functions, etc. that are self modifying, dependent on the template arguments supplied by the library user. This is my effort at a poor explanation, you can read more here if you are interested: [Template Meta Programming'](https://en.wikibooks.org/wiki/C%2B%2B_Programming/Templates/Template_Meta-Programming). The advantage of this system is that you only need to specifiy your sensor configuration once, SensorData<> class will handle the rest. The downside is, because TMP resolves during compile-time, if there is an error, it will cascade into a ginormous waterfall of errors after (failed) substitutions through SensorData<> are performed. My advice is to work slowly. In src/DAQInfrastructure you will find DAQSensors.h. My advice is to start by defining your first sensor in a seperate header file, see src/DAQInfrastructure/Sensors for examples. Then in your equivalent of DAQSensors.h, define a type (with using) of std::tuple<YourNewSensorClassType>. This is metaprogramming and everything is done through type definitions (because types are known at compile time). Then define another type (corresponding with SensorInfoTuple in src/DAQInfrastructure/DAQSensors.h) of type SensorData<MyTupleTypeIDefinedInThePreviousStep>. Now COMPILE! If this works, add another sensor, rinse, wash, and repeat. To compile, this library will need access to boost. We maintain a fork of BoostForArduino that can be found [here](https://github.com/Georgia-Tech-Off-Road/BoostForArduin). If you are compiling with Arduino, install this library and everything should work. Please let me know if you have any questions, we would be glad to help you.

# Testing
We test all of our libraries in our [repo](https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ). If you would like to test this library locally, please download our version of [BoostForArduino](https://github.com/Georgia-Tech-Off-Road/BoostForArduino). Add BoostForArduino's src dir to this libraries include path. We already reference a library named BoostForArduino in this repo's CMakeLists.txt to work with our [main repo](https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ). If you just like to test this library locally (and not integrate with your larger code base). Follow these steps.

1.) Download [BoostForArduino](https://github.com/Georgia-Tech-Off-Road/BoostForArduino) and place it in this repo's main folder.
1.) Add the following to lines to ./CMakeLists.txt *BEFORE* ``add_library(CommboardTransmissionLibrary INTERFACE)``:
> ```
> add_library(BoostForArduino INTERFACE)
> target_include_directories(BoostForArduino INTERFACE BoostForArduino/src)
> ```
1.) Initialize and build CMake as normal. Ex:
> ```
> cmake . -B build
> cmake --build .
> ```
