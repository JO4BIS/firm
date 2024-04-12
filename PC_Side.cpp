#include <iostream>
#include <windows.h>
#include <chrono>

using namespace std;

const int BAUD_RATE = 2400;
const int DATA_SIZE = 1010; // Adjusted the size to match the textData array size

// Define the text data to be transmitted
const char textData[DATA_SIZE] = "Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one. In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs,\" Rajan said in the note.\" Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently.";

void print_speed(int bytes_transferred, chrono::steady_clock::time_point start_time, int data_size) {
    auto current_time = chrono::steady_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count() / 1000.0;
    double speed = (bytes_transferred * 8) / elapsed_time; // bits per second
    double completion_percentage = (static_cast<double>(bytes_transferred) / data_size) * 100;
    cout << "Progress: " << completion_percentage << "% | Transmission Speed: " << speed << " bits/second" << endl;
}

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytes_written, total_bytes_written = 0;
    char data[DATA_SIZE];

    // Copy textData to data array
    memcpy(data, textData, DATA_SIZE);

    hSerial = CreateFile(L"\\\\.\\COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error opening serial port" << endl;
        return 1;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error getting serial port state" << endl;
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting serial port state" << endl;
        CloseHandle(hSerial);
        return 1;
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        cerr << "Error setting serial port timeouts" << endl;
        CloseHandle(hSerial);
        return 1;
    }

    chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

    // Send data to MCU
    for (int i = 0; i < DATA_SIZE; i++) {
        WriteFile(hSerial, &data[i], 1, &bytes_written, NULL);
        total_bytes_written += bytes_written;
        print_speed(total_bytes_written, start_time, DATA_SIZE);
    }

    // Receive data from MCU
    DWORD bytes_read, total_bytes_read = 0;
    while (total_bytes_read < DATA_SIZE) {
        ReadFile(hSerial, &data[total_bytes_read], 1, &bytes_read, NULL);
        total_bytes_read += bytes_read;
        print_speed(total_bytes_read, start_time, DATA_SIZE);
    }

    cout << "Received Data: " << data << endl;

    CloseHandle(hSerial);

    return 0;
}
