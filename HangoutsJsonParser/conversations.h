#pragma once
#include <iostream>
#include <string.h>
#include <stdlib.h>
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

	// prints the date
	void PrintDate() const;

	// checks if the given timestamp is on the same day with the current Time
	bool SameDate(long long ts);

	// checks if the two Times is on the same date
	friend bool SameDate(long long ts1, long long ts2);

	// converts the timestamp to standard time format in local time
	friend Time ConvertTime(long long timestamp);

	// output operator for Time
	friend std::ostream& operator<<(std::ostream& os, const Time& t);

};

// checks if the two times is on the same date
bool SameDate(long long ts1, long long ts2);
// converts the timestamp to standard time format in local time
Time ConvertTime(long long timestamp);
// output operator for Time
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
	// counts the events in a conversation
	int EventCount() const;
	
	// prints all the participants
	void PrintParticipants() const;
	// checks if the conversation was started by self
	bool SelfStarted() const;
	// gets the invite timestamp
	long long GetInviteTimestamp() const;
	// gets all the participants
	vector<Participant> GetParticipants() const;
	// gets all the events
	vector<Event> GetEvents() const;
	// gets events on the same date with the given Time
	vector<Event> GetEvents(Time t) const;
	// gets a specific type of events
	vector<Event> GetEvents(Event::Type t) const;


};

// **************** ConversationList ********************************
class ConversationList {
	vector<Conversation> conversations;
public:
	ConversationList() {}
	// gets the total number of conversations
	int GetNumber() const;
	// adds a new conversation to the list
	void AddConversation(const Conversation & con);
	// gets the most frequently visited conversation
	int GetFrequentContact(Conversation &con) const;
	// gets the earliest started conversation
	Time GetFirstContact(Conversation &con) const;
	// gets the most chatting day (Time) in terms of messages exchanged in a single conversation
	Time GetMostChattingDay(Conversation &con) const;
	// gets a random event of a specific type, return false if there is no
	bool GetType(Event::Type t, Conversation &con, Event &ev) const;
};


