#pragma once
#include <string.h>

// **************** Participant ********************************
class Participant {
	char* gaia_id;
	char* participant_name;
public:

	// constructor
	Participant() {
		gaia_id = new char[4];
		strcpy(gaia_id, "N.A.");
		participant_name = new char[4];
		strcpy(participant_name, "N.A.");
	}

	// constructs with parameters
	Participant(const char* id, const char* name = "") {
		gaia_id = new char[strlen(id)];
		strcpy(gaia_id, id);
		participant_name = new char[strlen(name)];
		strcpy(participant_name, name);
	}

	// gets the name of the participant
	char * GetName() const {
		return participant_name;
	}

	// gets the gaia id of the participant
	char * GetId() const {
		return gaia_id;
	}
};
