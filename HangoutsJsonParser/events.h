#pragma once
#include <iostream>
#include <vector>
#include "participants.h"
using namespace std;

// ******************** Attachment ***************************************
// An attachment can be a photo or video or maybe other files??
class Attachment {
	char * name;
	char * link;
public:
	enum Type { PHOTO, VIDEO, LOCATION, LINK };
	// 
	Attachment(const char* au, Type at, const char* an = ""){
		name = new char[strlen(an)];
		strcpy(name, an);
		link = new char[strlen(au)];
		strcpy(link, au);
		attachment_type = at;
	}

	// get the attachment link
	char* GetLink() {
		return link;
	}
	// get the attachment type
	Type GetType() {
		return attachment_type;
	}

private:
	Type attachment_type;
};

// ******************* HangoutEvent **********************
class HangoutEvent {
	int duration_secs;
	vector<Participant> participants;
public:
	enum EventType{ START_HANGOUT, END_HANGOUT };
	enum MediaType{ AUDIO_ONLY };

	// constructor
	HangoutEvent(EventType et = START_HANGOUT, MediaType mt = AUDIO_ONLY, int ds = 0) {
		event_type = et;
		media_type = mt;
		duration_secs = ds;
	}

	// adds participants to the hangout event
	void AddParticipant(const char* participant_id) {
		Participant new_participant(participant_id);
		participants.push_back(new_participant);
	}
private:
	EventType event_type;
	MediaType media_type;
};

// ****************** Event (linked with a single message) *********************************
// An event is basically a text message, but can also contain urls, photos and videos.
// It can also be location information, linked to a Google Maps url.
// It can also be a Hangout call, video or audio.
class Event {

	char* sender_id;
	long long timestamp;
	bool is_hangout_event;
	vector<char*> text;
	vector<Attachment> attachments;
	HangoutEvent* hangout_event;

public:
	// constructor
	Event(const char* si = "", long long ts = 0) {
		sender_id = new char[strlen(si)];
		strcpy(sender_id, si);
		timestamp = ts;
		is_hangout_event = false;
	}
	// if the event is a hangout call event
	bool IsHangoutEvent() {
		return is_hangout_event;
	}
	// adds a new text segment
	void AddText(const char* txt) {
		char * new_text = new char[strlen(txt)];
		strcpy(new_text, txt);
		text.push_back(new_text);
	}

	// adds an attachment
	void AddAttachment(const char* a_link, Attachment::Type a_type, const char* a_name = "") {
		Attachment new_attachment(a_link, a_type, a_name);
		attachments.push_back(new_attachment);
	}

	// sets the current event as a hangout call event
	void SetHangoutEvent(HangoutEvent* he) {
		is_hangout_event = true;
		hangout_event = he;
	}
};

