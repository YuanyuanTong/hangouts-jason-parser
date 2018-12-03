#include "conversations.h"

// ************** Time ***********************

// checks if the two times is on the same date
bool SameDate(const Time &t1, const Time &t2) {
	return (t1.day == t2.day && t1.month == t2.month && t1.year == t2.year);
}

// converts the timestamp to standard time format in local time
Time CovertTime(long long timestamp) {

	time_t tick = (time_t)(timestamp / 1000000);
	struct tm tm;
	char s[100];

	Time local_time;

	tm = *localtime(&tick);

	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);

	local_time.year = atoi(s);
	local_time.month = atoi(s + 5);
	local_time.day = atoi(s + 8);
	local_time.hour = atoi(s + 11);
	local_time.minute = atoi(s + 14);
	local_time.second = atoi(s + 17);

	return local_time;
}

// output operator for Time
std::ostream& operator<<(std::ostream& os, const Time& t) {
	os << t.hour << ":" << t.minute << ":" << t.second << "\t" << t.month << "/" << t.day << "/" << t.year;
	return os;
}


// ************** Conversation ***********************

// default constructor
Conversation::Conversation()
	:invite_timestamp(0ll), latest_read_timestamp(0ll) {
	self_id = new char[4];
	strcpy(self_id, "N.A.");
	inviter_id = new char[4];
	strcpy(inviter_id, "N.A.");
	self_name = new char[4];
	strcpy(self_name, "N.A.");
	group_chat = false;
}

// constructs a conversation from parameters
Conversation::Conversation(const char* ii, long long it, long long lt, const char* si, bool group)
	:invite_timestamp(it), latest_read_timestamp(lt) {
	inviter_id = new char[strlen(ii)];
	strcpy(inviter_id, ii);
	self_id = new char[strlen(si)];
	strcpy(self_id, si);
	group_chat = group;
}

// copy constructor
Conversation::Conversation(const Conversation& con) {
	participants = con.participants;
	events = con.events;
	self_id = con.self_id;
	self_name = con.self_name;
	group_chat = con.group_chat;
	invite_timestamp = con.invite_timestamp;
	latest_read_timestamp = con.latest_read_timestamp;
}

// adds a new participant to the participant list
void Conversation::AddParticipant(const char* participant_id, const char* participant_name) {
	// already one participant (other than self)
	if (participants.size() == 1) {
		group_chat = true;
	}
	Participant new_participant(participant_id, participant_name);
	participants.push_back(new_participant);
}

// adds a new event to the events list
void Conversation::AddEvent(Event new_event) {
	events.push_back(new_event);
}

// edits the self name
void Conversation::SelfName(const char* sn) {
	self_name = new char[strlen(sn)];
	strcpy(self_name, sn);
}


// **************** ConversationList ********************************
void ConversationList::AddConversation(const Conversation & con) {
	conversations.push_back(con);
}
