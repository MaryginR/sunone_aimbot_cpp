#include "MidiConnection.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MidiConnection::MidiConnection(const std::string& portName)
{
    try
    {
        midi_ = std::make_unique<RtMidiOut>();

        unsigned int ports = midi_->getPortCount();
        bool found = false;

        for (unsigned int i = 0; i < ports; i++)
        {
            std::string name = midi_->getPortName(i);
            if (name.find(portName) != std::string::npos)
            {
                midi_->openPort(i);
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "[MIDI] Port not found: " << portName << std::endl;
            return;
        }

        is_open_.store(true);
        std::cout << "[MIDI] Connected to port: " << portName << std::endl;
    }
    catch (RtMidiError& e)
    {
        std::cerr << "[MIDI] Error creating RtMidiOut: " << e.getMessage() << std::endl;
    }
}

MidiConnection::~MidiConnection()
{
    if (midi_)
    {
        try { midi_->closePort(); }
        catch (...) {}
    }
    is_open_.store(false);
}

void MidiConnection::sendCC(uint8_t controller, uint8_t value)
{
    if (!is_open_) return;
    std::lock_guard<std::mutex> lg(write_mutex_);

    std::vector<unsigned char> msg(3);
    msg[0] = 0xB0;        // CC on channel 0
    msg[1] = controller;
    msg[2] = value;

    try { midi_->sendMessage(&msg); }
    catch (...) {}
}

void MidiConnection::sendNoteOn(uint8_t note)
{
    if (!is_open_) return;
    std::lock_guard<std::mutex> lg(write_mutex_);

    std::vector<unsigned char> msg = { 0x90, note, 127 };
    try { midi_->sendMessage(&msg); }
    catch (...) {}
}

void MidiConnection::sendNoteOff(uint8_t note)
{
    if (!is_open_) return;
    std::lock_guard<std::mutex> lg(write_mutex_);

    std::vector<unsigned char> msg = { 0x80, note, 0 };
    try { midi_->sendMessage(&msg); }
    catch (...) {}
}

void MidiConnection::move(int dx, int dy)
{
    if (!is_open_) return;

    // приведение в MIDI-диапазон 0Ц127
    int x = std::clamp(dx + 64, 0, 127);
    int y = std::clamp(dy + 64, 0, 127);

    sendCC(CC_X, (uint8_t)x);
    sendCC(CC_Y, (uint8_t)y);
}

void MidiConnection::press()
{
    shooting_active = true;
    sendNoteOn(NOTE_LMB);
}

void MidiConnection::release()
{
    shooting_active = false;
    sendNoteOff(NOTE_LMB);
}
