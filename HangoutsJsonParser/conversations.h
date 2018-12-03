#pragma once
#include <iostream>
#include <string.h>
#include <time.h>
#include <vector>
#include "events.h"
#include "participants.h"

using namespace std;

// Time
class Time {

	int month;
	int day;
	int year;
	int hour;
	int minute;
	int second;

public:
	// constructor
	Time(int mo = 0, int dy = 0, int yr = 0, int hr = 0, int min = 0, int sec = 0) {
		month = mo;
		day = dy;
		year = yr;
		hour = hr;
		minute = min;
		second = sec;
	}

	// copy constructor
	Time(const Time &tt) {
		month = tt.month;
		day = tt.day;
		year = tt.year;
		hour = tt.hour;
		minute = tt.minute;
		second = tt.second;
	}

	// check if the two Times is on the same date
	friend bool SameDate(const Time &t1, const Time &t2);

	// convert the timestamp to standard time format in local time
	friend Time CovertTime(long long timestamp);

	// output operator for Time
	friend std::ostream& operator<<(std::ostream& os, const Time& t);

};

bool SameDate(const Time &t1, const Time &t2);
Time CovertTime(long long timestamp);
std::ostream& operator<<(std::ostream& os, const Time& t);


// **************** Conversation ********************************
class Conversation {

	char* self_id;
	char* self_name;

	//the inviter of the conversation
	char* inviter_id;
	// when were the participants first invited to the conversation
	long long invite_timestamp;
	// when was the conversation last read
	long long latest_read_timestamp;
	// if the current conversation is a group chat (more than 2 people)
	bool group_chat;

	// a list of events
	vector<Event> events;
	// a list of participants in the current conversation
	vector<Participant> participants;

public:
	// default constructor
	Conversation();
	// constructs a conversation from parameters
	Conversation(const char* ii, long long it, long long lt, const char* si = "User0", bool group = false);
	// copy constructor
	Conversation(const Conversation& con);
	// adds a new participant to the participant list (excluding self)
	void AddParticipant(const char* participant_id, const char* participant_name);
	// adds a new event to the event list
	void AddEvent(Event event);
	// edits the self name
	void SelfName(const char* sn = "N.A.");
};

// **************** ConversationList ********************************
class ConversationList {
	vector<Conversation> conversations;
public:
	ConversationList() {}
	void AddConversation(const Conversation & con);
};


