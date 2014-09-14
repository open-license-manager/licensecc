/*
 * EventRegistry.h
 *
 *  Created on: Mar 30, 2014
 *      
 */

#ifndef EVENTREGISTRY_H_
#define EVENTREGISTRY_H_

#include "../api/datatypes.h"
#include <vector>
#include <string>

namespace license {
using namespace std;
/*
AuditEvent error_event_builder(EVENT_TYPE event);
AuditEvent audit_event_builder(EVENT_TYPE event, SEVERITY severity);
AuditEvent audit_event_builder(EVENT_TYPE event, SEVERITY severity,
		const string& eventParameter);*/

class EventRegistry {
private:
	friend EventRegistry& operator<<(EventRegistry&, AuditEvent&);
	friend EventRegistry& operator<<(EventRegistry&, EventRegistry&);
	//TODO change into map
	vector<AuditEvent> logs;
	//Forbid copy
	//EventRegistry(const EventRegistry& that) = delete;
public:
	EventRegistry();
	//operator <<
	void append(const EventRegistry& eventRegistry);
	void turnLastEventIntoError();
	bool turnEventIntoError(EVENT_TYPE event);
	bool turnErrosIntoWarnings();
	/**
	 * @return NULL if no failures are found.
	 */
	AuditEvent const * getLastFailure() const;
	bool isGood() const;

	void addError(EVENT_TYPE event);
	void addEvent(EVENT_TYPE event, SEVERITY severity);
	void addEvent(EVENT_TYPE event, SEVERITY severity,
			const string& eventParameter);
	void exportLastEvents(AuditEvent* auditEvents,int nlogs);

};
}
#endif /* EVENTREGISTRY_H_ */
