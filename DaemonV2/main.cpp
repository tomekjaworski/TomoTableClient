#include <system_error>
#include <cassert>

#include "ProgramBase.hpp"
#include "SerialPort.hpp"
#include <list>
#include "TableDevice.hpp"
#include "AnsiCodes.h"

class App : public ProgramBase {
public:
    App(int argc, const char** argv, const char** env)
        : ProgramBase(argc, argv, env) {

    }

    int Main(const std::vector<std::string>& arguments) override;

public:
    void ShowAvailableSerialPorts(void);
    std::list<SerialPort::Ptr> OpenAllSerialPorts(void);
};

void App::ShowAvailableSerialPorts(void) {
    std::string s = "";
    auto port_name_list = SerialPort::GetSerialDevices();
    for (const auto& pname : port_name_list) // może boost?
        if (!s.empty())
            s += ", " + pname;
        else
            s += pname;

    printf("Available serial ports (%zu): %s\n", port_name_list.size(), s.c_str());
}

std::list<SerialPort::Ptr> App::OpenAllSerialPorts(void) {
    std::list<SerialPort::Ptr> ports;
    for (const auto& pname : SerialPort::GetSerialDevices()) {
        try {
            printf("Opening port %s... ", pname.c_str());
            SerialPort::Ptr sp(new SerialPort(pname, 19200));
            ports.push_back(sp);

            printf(AGREEN "Done.\n" ARESET);
        } catch (const std::exception &ex)
        {
            printf(ARED "FAILED: " AYELLOW "%s\n" ARESET, ex.what());
        }
    }
    return ports;
}
/*


*/
#include "PhotoModule.hpp"
#include "InputMessageBuilder.hpp"

#include "OutputMessage.hpp"
#include "TimeoutError.h"

class Communication {

public:
    static void Transcive(SerialPort::Ptr serial, const OutputMessage& query, InputMessage& response, int timeout);
};

void Communication::Transcive(SerialPort::Ptr serial, const OutputMessage& query, InputMessage& response, int timeout) {

    if (serial == nullptr)
        throw std::invalid_argument("serial");

    InputMessageBuilder mr;
    std::array<uint8_t, 256> recv_buffer;

    serial->DiscardAllData();
    serial->Send(query.GetDataPointer(), query.GetDataCount());

    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    // now wait for response
    int loops = 0;
    ssize_t ss;
    do {
        fd_set rfd;
        FD_ZERO(&rfd);
        FD_SET(serial->GetHandle(), &rfd);

        loops++;
        timeval tv = {.tv_sec = 0, .tv_usec = 50 * 1000};
        int sret = ::select(serial->GetHandle() + 1, &rfd, nullptr, nullptr, &tv);

        if (sret == -1)
            throw std::runtime_error("select");

        if (sret > 0) {
            ssize_t recv_bytes = serial->Receive(recv_buffer);
            mr.AddCollectedData(recv_buffer, 0, recv_bytes);

            if (mr.GetMessage(response))
                break;
        }

        // check for timeout
        //_check_timeout:;
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count() > timeout) {
            throw TimeoutError(serial->GetPortName());
        }

    } while (true);
}

//
//void Communication::Transcive(std::list<SerialPort::Ptr> serials, const OutputMessage& query, InputMessage& response, int timeout) {
//
//    if (serial == nullptr)
//        throw std::invalid_argument("serial");
//
//    InputMessageBuilder mr;
//    std::array<uint8_t, 256> recv_buffer;
//
//    serial->DiscardAllData();
//    serial->Send(query.GetDataPointer(), query.GetDataCount());
//
//    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
//
//    // now wait for response
//    int loops = 0;
//    ssize_t ss;
//    do {
//        fd_set rfd;
//        FD_ZERO(&rfd);
//        FD_SET(serial->GetHandle(), &rfd);
//
//        loops++;
//        timeval tv = {.tv_sec = 0, .tv_usec = 50 * 1000};
//        int sret = ::select(serial->GetHandle() + 1, &rfd, nullptr, nullptr, &tv);
//
//        if (sret == -1)
//            throw std::runtime_error("select");
//
//        if (sret > 0) {
//            ssize_t recv_bytes = serial->Receive(recv_buffer);
//            mr.AddCollectedData(recv_buffer, 0, recv_bytes);
//
//            if (mr.GetMessage(response))
//                break;
//        }
//
//        // check for timeout
//        _check_timeout:;
//        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
//        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count() > timeout) {
//            throw TimeoutError(serial->GetPortName());
//        }
//
//    } while (true);
//}

#include <unistd.h>
#include <boost/algorithm/string.hpp>


int App::Main(const std::vector<std::string>& arguments) {

    //
    // Intro
    printf("Smart Table Reconstruction Daemon, by Tomasz Jaworski, 2017\n");
    printf("Built on %s @ %s %lu\n\n", __DATE__, __TIME__, sizeof(void*));
    setbuf(stdout, NULL);

    //
    // Show available serial ports
    this->ShowAvailableSerialPorts();

    //
    // Open all serial ports
    std::list<SerialPort::Ptr> ports = this->OpenAllSerialPorts();

    //
    // Initialize table description
    TableDevice tdev;
    tdev.ShowTopology();


    //
    // Initialize communication on each searial port and detect device id
    while(1)
    for (SerialPort::Ptr serial_port : ports) {

        OutputMessage message_ping = OutputMessage(MessageType::PingRequest);
        int successes = 0;
        bool last_was_ok;
        printf("Serial %s: ", serial_port->GetPortName().c_str());

        for (int i = 0; i < 5; i++) {
            usleep(50 * 1000);
            serial_port->DiscardAllData();

            InputMessage response;
            try {
                last_was_ok = false;
                printf(".");
                Communication::Transcive(serial_port, message_ping, response, 250);
            } catch(const TimeoutError& te) {
                printf("t");
                continue;
            }

            if (response.GetMessageType() != MessageType::PingResponse || response.GetPayloadSize() != 0) {
                printf("e");
                continue;
            }

            successes++;
            last_was_ok = true;
        }
        printf("\n");
        //
        // assess the communication
        if (successes == 0) {
            // The port is dead; there is probably no interesting equipment
            printf(AYELLOW "Serial port %s has NO compatible devices attached.\n" ARESET, serial_port->GetPortName().c_str());
            continue;
        }

        if (successes < 3 || !last_was_ok) {
            // There were some successful communications, however not enough and what is more - last one has failed.
            printf(ARED "Serial port %s communication problems; intervention required.\n" ARESET, serial_port->GetPortName().c_str());
            continue;
        }

        //
        // Ok, looks like we have a working photo device there.
        // Lets get its data
        InputMessage response;
        OutputMessage message_device_query = OutputMessage(MessageType::DeviceIdentifierRequest);

        try {
            last_was_ok = false;
            Communication::Transcive(serial_port, message_device_query, response, 250);
        } catch(const TimeoutError& te) {
            printf(ARED "Timeout in %s during DeviceIdentifierRequest\n" ARESET, serial_port->GetPortName().c_str());
            continue;
        }

        std::string device_info(
                reinterpret_cast<const char*>(response.GetPayloadPointer()),
                reinterpret_cast<const char*>(response.GetPayloadPointer()) + response.GetPayloadSize());


        std::vector<std::string> entries;
        boost::split(entries, device_info, boost::is_any_of(";"));
        std::map<std::string, std::string> info;
        for (const std::string& entry : entries) {
            std::vector<std::string> tokens;
            boost::split(tokens, entry, boost::is_any_of("="));
            if (tokens.size() == 2) {
                info[tokens[0]] = tokens[1];
            }
        }

        printf(AYELLOW "   Port %s, device %s: version=[%s], date=[%s], time=[%s]\n" ARESET,
            serial_port->GetPortName().c_str(),
            info["device"].c_str(),
            info["version"].c_str(),
            info["date"].c_str(),
            info["time"].c_str()
        );

        device_identifier_t devid = static_cast<device_identifier_t>(std::stoi(info["device"]));

        //try {
            PhotoModule::Ptr pmodule = tdev.GetPhotoModuleByID(devid);
        //} catch
    }

    SerialPort::Ptr sp = ports.front();
    sp->DiscardAllData();

    std::array<uint8_t, 5>  buffer1 = {0xAB, 0x04, 0x00, 0x00, 0x00};
    std::array<uint8_t, 5>  buffer2  = {0xAB, 0x02, 0x00, 0x00, 0x00};
    sp->Send(buffer1);
    sp->Send(buffer2);
    sp->Send(buffer1);
    sp->Send(buffer2);

    std::array<uint8_t, 100> b{};
    InputMessageBuilder mr;

    int ret;
    ret = sp->Receive(b);
    mr.AddCollectedData(b, 0, ret);
    ret = sp->Receive(b);
    mr.AddCollectedData(b, 0, ret);

    InputMessage msg;
    bool status;
    status = mr.GetMessage(msg);
    status = mr.GetMessage(msg);
    status = mr.GetMessage(msg);
    status = mr.GetMessage(msg);

    OutputMessage q(MessageType::DeviceIdentifierRequest);
    while(true) {
        try {
            Communication::Transcive(sp, q, msg, 500);
        } catch(const TimeoutError& te) {
            printf("t");
        }
    }
    return 0;
}

int main(int argc, const char** argv, const char** env) {
    App app(argc, argv, env);
    app.Run();
    return app.GetErrorCode();
}