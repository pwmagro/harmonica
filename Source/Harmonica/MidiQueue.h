/*
  ==============================================================================

    MidiQueue.h
    Created: 11 Oct 2022 6:15:26pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

typedef struct {
    int note = 0;
    double hz = 0;
    double mag = 0;
    int startSample = -1;
    int endSample = -1;

    enum midiNoteStatus_t {
        note_Playing,
        note_QueuedToStart,
        note_QueuedToEnd
    } status;

    int scanIndex = 0;
} midiNote_t;

typedef struct {
    void note_on(juce::MidiMessage msg) {
        DBG("note on");
        DBG(msg.getNoteNumber());
        // check that the note's not already on
        for (auto& note : midiQ) {
            if (note.note == msg.getNoteNumber()) return;
        }

        // make space if there is none
        while (midiQ.size() >= maxVoices)
            midiQ.pop_front();

        // push to back of queue
        midiNote_t newNote = {
            msg.getNoteNumber(),                                // newNote.note
            msg.getMidiNoteInHertz(msg.getNoteNumber()),        // newNote.hz
            msg.getVelocity() / 127.f,                          // newNote.mag
            (int)msg.getTimeStamp(),                            // newNote.startSample
            -1,                                                 // newNote.endSample
            midiNote_t::midiNoteStatus_t::note_QueuedToStart,   // newNote.status
            0                                                   // newNote.scanIndex
        };

        midiQ.push_back(newNote);
    }

    void note_off(juce::MidiMessage msg) {
        DBG("note off");
        DBG(msg.getNoteNumber());
        for (int i = 0; i < midiQ.size(); i++) {
            if (midiQ[i].note == msg.getNoteNumber()) {
                midiQ[i].endSample = (int)msg.getTimeStamp();
                midiQ[i].status = midiNote_t::midiNoteStatus_t::note_QueuedToEnd;
            }
        }
    }

    void update_notes(int sample) {
        for (int i = 0; i < midiQ.size(); i++) {
            if (midiQ[i].status == midiNote_t::midiNoteStatus_t::note_QueuedToStart && midiQ[i].startSample == sample) {
                midiQ[i].status = midiNote_t::midiNoteStatus_t::note_Playing;
            }

            if (midiQ[i].status == midiNote_t::midiNoteStatus_t::note_QueuedToEnd && midiQ[i].startSample == sample) {
                midiQ.erase(midiQ.begin() + i);
            }
        }
    }

    void panic() {
        midiQ.clear();
    }
    unsigned char maxVoices = 8;
    std::deque<midiNote_t> midiQ;
} midiQueue_t;