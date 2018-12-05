#include "conversations.h"

// ************** Time ***********************

// prints the date
void Time::PrintDate() const {
	cout << month << "/" << day << "/" << year;
}

// checks if the given timestamp is on the same day with the current Time
bool Time::SameDate(long long ts) {
	Time time = ConvertTime(ts);
	return (day == time.day && month == time.month && year == time.year);
}

// checks if the two Times is on the same date
bool SameDate(long long ts1, long long ts2) {
	Time t1 = ConvertTime(ts1);
	Time t2 = ConvertTime(ts2);
	return (t1.day == t2.day && t1.month == t2.month && t1.year == t2.year);
}

// converts the timestamp to standard time format in local time
Time ConvertTime(long long timestamp) {

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

// counts the events in a conversation
int Conversation::EventCount() const {
	return events.size();
}

// prints all the participants
void Conversation::PrintParticipants() const {
	cout << "< ";
	for (vector<Participant>::const_iterator it = participants.begin(); it != participants.end(); ++it) {
		cout << it->GetName() << " ";
	}
	cout << ">";
}

// checks if the conversation was started by self
bool Conversation::SelfStarted() const {
	return strcmp(self_id, inviter_id) == 0;
}

// gets the invite timestamp of a conversation
long long Conversation::GetInviteTimestamp() const {
	return invite_timestamp;
}

// gets all the events
vector<Event> Conversation::GetEvents() const {
	return events;
}

// gets all the participants
vector<Participant> Conversation::GetParticipants() const {
	return participants;
}

// gets events on the same date with the given Time
vector<Event> Conversation::GetEvents(Time t) const {
	vector<Event> time_events;
	for (vector<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (t.SameDate(it->GetTimestamp())) {
			time_events.push_back(*it);
		}
	}
	return time_events;
}

// gets a specific type of events
vector<Event> Conversation::GetEvents(Event::Type t) const {
	vector<Event> type_events;
	for (vector<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (it->IsEventType(t)) {
			type_events.push_back(*it);
		}
	}
	return type_events;
}


// **************** ConversationList ********************************

// gets the total number of conversations
int ConversationList::GetNumber() const {
	return conversations.size();
}

// adds a new conversation to the list
void ConversationList::AddConversation(const Conversation & con) {
	conversations.push_back(con);
}

// gets the most frequently contacted person, returns the frequency
int ConversationList::GetFrequentContact(Conversation &con) const {
	int max_frequency = 0;
	int index = 0;
	int max_index = index;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->EventCount() > max_frequency) {
			max_frequency = it->EventCount();
			max_index = index;
		}
		++index;
	}

	con = conversations[max_index];
	return max_frequency;
}

// gets the earliest started conversation
Time ConversationList::GetFirstContact(Conversation &con) const {
	long long first_timestamp = std::numeric_limits<long long>::max();
	int index = 0;
	int first_index = index;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->GetInviteTimestamp() < first_timestamp) {
			first_timestamp = it->GetInviteTimestamp();
			first_index = index;
		}
		++index;
	}

	con = conversations[first_index];
	return ConvertTime(first_timestamp);
}

// gets the most chatting day (Time) in terms of messages exchanged in a single conversation
Time ConversationList::GetMostChattingDay(Conversation &con) const {
	int day_events_cnt = 0;
	int max_cnt = 0;
	int index = 0;
	int max_idx = index;
	long long tmp_timestamp = 0;
	long long max_timestamp = tmp_timestamp;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		vector<Event> events = it->GetEvents();
		for (vector<Event>::const_iterator eit = events.begin(); eit != events.end(); ++eit) {
			if (SameDate(tmp_timestamp, eit->GetTimestamp())) {
				++day_events_cnt;
				if (day_events_cnt > max_cnt) {
					max_cnt = day_events_cnt;
					max_timestamp = tmp_timestamp;
					max_idx = index;
				}
			}
			else {
				tmp_timestamp = eit->GetTimestamp();
				day_events_cnt = 1;
			}
		}
		++index;
	}

	con = conversations[max_idx];
	return ConvertTime(max_timestamp);
}

// gets a random event of a specific type
bool ConversationList::GetType(Event::Type t, Conversation &con, Event &ev) const {
	vector<Conversation> type_conversations;
	for (vector<Conversation>::const_iterator it = conversations.begin(); it != conversations.end(); ++it) {
		if (it->GetEvents(t).size()) {
			type_conversations.push_back(*it);
		}
	}
	if (type_conversations.size()) {
		srand(time(NULL));
		int index = rand() % type_conversations.size();
		con = type_conversations[index];
		vector<Event> events = con.GetEvents(t);
		index = rand() % events.size();
		ev = events[index];
		return true;
	}
	else {
		return false;
	}
}