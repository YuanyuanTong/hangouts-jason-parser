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

	// gets the attachment link
	char* GetLink() const{
		return link;
	}
	// gets the attachment type
	Type GetType() const{
		return attachment_type;
	}

	// output operator for Time
	friend ostream& operator<<(std::ostream& os, const Attachment& a) {
		os << "[" << a.name << "]" << " (" << a.link << ")";
		return os;
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
	//
	enum Type{TEXT, PHOTO, VIDEO, CALL, LOCATION, URL};
	// constructor
	Event(const char* si = "", long long ts = 0) {
		sender_id = new char[strlen(si)];
		strcpy(sender_id, si);
		timestamp = ts;
		is_hangout_event = false;
	}

	// if the event is a hangout call event
	bool IsHangoutEvent() const {
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

	// gets the timestamp of the event
	long long GetTimestamp() const {
		return timestamp;
	}

	// checks if the event contains the give type content
	bool IsEventType(Type type) const{
		switch (type)
		{

		case Event::TEXT:
			return text.size();
			break;

		case Event::PHOTO:
			for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
				if (it->GetType() == Attachment::Type::PHOTO) {
					return true;
				}
			}
			return false;
			break;

		case Event::VIDEO:
			for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
				if (it->GetType() == Attachment::Type::VIDEO) {
					return true;
				}
			}
			return false;
			break;

		case Event::CALL:
			return is_hangout_event;
			break;

		case Event::LOCATION:
			for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
				if (it->GetType() == Attachment::Type::LOCATION) {
					return true;
				}
			}
			return false;
			break;

		case Event::URL:
			for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
				if (it->GetType() == Attachment::Type::LINK) {
					return true;
				}
			}
			return false;
			break;

		default:
			return false;
			break;
		}
	}

	// gets the k'th attachment of a specific type
	Attachment GetTypeAttachment(Type t, int k = 0) {
		Attachment::Type attachment_type;
		switch (t)
		{
		case Event::PHOTO:
			attachment_type = Attachment::PHOTO;
			break;
		case Event::VIDEO:
			attachment_type = Attachment::VIDEO;
			break;
		case Event::LOCATION:
			attachment_type = Attachment::LOCATION;
			break;
		case Event::URL:
			attachment_type = Attachment::LINK;
			break;
		default:
			break;
		}
		vector<Attachment> type_attachments;
		for (vector<Attachment>::const_iterator it = attachments.begin(); it != attachments.end(); ++it) {
			if (it->GetType() == attachment_type) {
				type_attachments.push_back(*it);
			}
		}
		return type_attachments[k];
	}
};

