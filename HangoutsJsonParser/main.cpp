#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/encodedstream.h"
#include "conversations.h"

using namespace rapidjson;

void WriteJSONFile(FILE* fp, UTFType type, bool putBOM, const Document& d) {
	char writeBuffer[256];
	FileWriteStream bos(fp, writeBuffer, sizeof(writeBuffer));
	typedef AutoUTFOutputStream<unsigned, FileWriteStream> OutputStream;
	OutputStream eos(bos, type, putBOM);
	Writer<OutputStream, UTF8<>, AutoUTF<unsigned> > writer;
	d.Accept(writer);
}

int main() {

	FILE* fp1 = fopen("Hangouts.json", "rb");
	char readBuffer[256];
	FileReadStream bis(fp1, readBuffer, sizeof(readBuffer));
	AutoUTFInputStream<unsigned, FileReadStream> eis(bis);
	Document d;         // Document default uses GenericDocument<UTF8<> >
	d.ParseStream<0, AutoUTF<unsigned> >(eis).HasParseError();
	fclose(fp1);


	// Hangouts.json is stored as an object with a single array "conversations".

	SizeType n = d["conversations"].Size();
	const Value & a = d["conversations"];

	// a list to store all conversations in the json file
	ConversationList conversation_list;

	// insert all conversation into the "conversations" list
	for (SizeType i = 0; i < n; ++i) {
		// basic information of a conversation
		const Value & conversation_state = a[i]["conversation"]["conversation"]["self_conversation_state"];
		const char * self_id = conversation_state["self_read_state"]["participant_id"]["gaia_id"].GetString();
		long long latest_read_timestamp = atoll(conversation_state["self_read_state"]["latest_read_timestamp"].GetString());
		const char * inviter_id = conversation_state["inviter_id"]["gaia_id"].GetString();
		long long invite_timestamp = atoll(conversation_state["invite_timestamp"].GetString());
		
		Conversation  cur_conversation(inviter_id, invite_timestamp, latest_read_timestamp, self_id);
		
		const Value & participant_data = a[i]["conversation"]["conversation"]["participant_data"];
		for (Value::ConstValueIterator itr = participant_data.Begin(); itr != participant_data.End(); ++itr) {
			//assert(itr->GetObject().HasMember("id"));
			const char * participant_id = itr->GetObject()["id"]["gaia_id"].GetString();

			// participants besides self
			if (strcmp(participant_id, self_id)) {
				const char * participant_name = itr->GetObject()["fallback_name"].GetString();
				cur_conversation.AddParticipant(participant_id, participant_name);
			}
			// self
			else {
				const char * self_name = itr->GetObject()["fallback_name"].GetString();
				cur_conversation.SelfName(self_name);
			}
		}
		
		// all detailed events (messages) of a conversation
		const Value & events = a[i]["events"];
		for (Value::ConstValueIterator itr = events.Begin(); itr != events.End(); ++itr) {
			const char* sender_id = itr->GetObject()["sender_id"]["gaia_id"].GetString();
			long long send_timestamp = atoll(itr->GetObject()["timestamp"].GetString());
			Event new_event(sender_id, send_timestamp);

			// hangout call event
			if (itr->GetObject().HasMember("hangout_event")) {
				HangoutEvent::EventType event_type;
				HangoutEvent::MediaType media_type;
				if (strcmp(itr->GetObject()["hangout_event"]["event_type"].GetString(), "START_HANGOUT") == 0) {
					event_type = HangoutEvent::EventType::START_HANGOUT;
				}
				if (strcmp(itr->GetObject()["hangout_event"]["event_type"].GetString(), "END_HANGOUT") == 0) {
					event_type = HangoutEvent::EventType::END_HANGOUT;
				}
				if (strcmp(itr->GetObject()["hangout_event"]["media_type"].GetString(), "AUDIO_ONLY") == 0) {
					media_type = HangoutEvent::MediaType::AUDIO_ONLY;
				}

				HangoutEvent * new_hangout_event;
				if (itr->GetObject()["hangout_event"].HasMember("hangout_duration_secs")) {
					int duration_secs = atoi(itr->GetObject()["hangout_event"]["hangout_duration_secs"].GetString());
					new_hangout_event = new HangoutEvent(event_type, media_type, duration_secs);
					new_event.SetHangoutEvent(new_hangout_event);
				}
				else {
					new_hangout_event = new HangoutEvent(event_type, media_type);
					new_event.SetHangoutEvent(new_hangout_event);
				}

				// adding participants of the hangout call
				const Value & hangout_participants = itr->GetObject()["hangout_event"]["participant_id"];
				for (Value::ConstValueIterator pitr = hangout_participants.Begin(); pitr != hangout_participants.End(); ++pitr) {
					new_hangout_event->AddParticipant( pitr->GetObject()["gaia_id"].GetString() );
				}
				
				new_event.SetHangoutEvent(new_hangout_event);
			}

			// regular chat message
			if (itr->GetObject().HasMember("chat_message")) {
				const Value & message_content = itr->GetObject()["chat_message"]["message_content"];

				// basic text messages (including urls)
				if (message_content.HasMember("segment")) {

					const Value & segments = message_content["segment"];

					for (Value::ConstValueIterator sitr = segments.Begin(); sitr != segments.End(); ++sitr) {

						if ( strcmp( sitr->GetObject()["type"].GetString(), "LINK" ) ) {
							if ( strcmp( sitr->GetObject()["type"].GetString(), "LINE_BREAK" ) ) {
								new_event.AddText(sitr->GetObject()["text"].GetString());
							}
							else {
								new_event.AddText("\n");
							}
						}

						// texts including urls (seen as attachments)
						else {
							new_event.AddText(sitr->GetObject()["text"].GetString());
							new_event.AddAttachment(sitr->GetObject()["text"].GetString(), Attachment::Type::LINK );
						}
					}
				}

				// messages with photo, video or location attachments
				if (message_content.HasMember("attachment")) {
					// all the attachments
					const Value & attachments = message_content["attachment"];

					for (Value::ConstValueIterator aitr = attachments.Begin(); aitr != attachments.End(); ++aitr) {
						// 
						if (strcmp(aitr->GetObject()["embed_item"]["type"][0].GetString(), "PLUS_PHOTO") ) {
							// location attachment
							if (!strcmp(aitr->GetObject()["embed_item"]["type"][0].GetString(), "PLACE_V2")) {
								const Value & place = aitr->GetObject()["embed_item"]["place_v2"];
								new_event.AddAttachment(place["url"].GetString(), Attachment::Type::LOCATION, place["name"].GetString());
							}
						}

						// photo or video attachment
						else {
							const Value & plus_photo = aitr->GetObject()["embed_item"]["plus_photo"];
							const char * url = plus_photo["thumbnail"]["url"].GetString();
							Attachment::Type attachment_type;
							// photo
							if ( !strcmp(plus_photo["media_type"].GetString(), "PHOTO") ) {
								attachment_type = Attachment::Type::PHOTO;
							}

							if (!strcmp(plus_photo["media_type"].GetString(), "VIDEO") ) {
								attachment_type = Attachment::Type::VIDEO;
							}
							new_event.AddAttachment(url, attachment_type);

						}
					}
				}
			}

			// adding the current event to the conversation
			cur_conversation.AddEvent(new_event);
		}

		// adding the current conversation to the conversation list
		conversation_list.AddConversation(cur_conversation);
	}

	system("pause");
	return 0;
}