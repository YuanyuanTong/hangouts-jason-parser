# hangouts-jason-parser
This is a C++ parser for Hangouts.json from Google takeout. It is based on RapidJSON parser.

This parser can read a Hangouts.json file and store chat history data into a ConversationList object.

Chat history data include:
* Conversation basic information: participant ids, start time, latest read time, inviter id, etc.
* Detailed conversation contents:
  * Basic text messages.
  * Text messages including URLs.
  * Photo and video attachments.
  * Shared locations (with link to Google Maps).

It should support group chat with more than two people.
