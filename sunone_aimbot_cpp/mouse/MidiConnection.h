#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

#include "RtMidi.h"

class MidiConnection
{
public:
    MidiConnection(const std::string& portName);
    ~MidiConnection();

    bool isOpen() const { return is_open_.load(); }

    void move(int dx, int dy);
    void press();
    void release();

    // состояние кнопок (аналог SerialConnection)
    bool aiming_active = false;
    bool shooting_active = false;

private:
    void sendCC(uint8_t controller, uint8_t value);
    void sendNoteOn(uint8_t note);
    void sendNoteOff(uint8_t note);

private:
    std::unique_ptr<RtMidiOut> midi_;
    std::atomic<bool> is_open_{ false };
    std::mutex write_mutex_;

    // CC номера
    static constexpr uint8_t CC_X = 20;
    static constexpr uint8_t CC_Y = 21;

    // Note номера
    static constexpr uint8_t NOTE_LMB = 40;
};
