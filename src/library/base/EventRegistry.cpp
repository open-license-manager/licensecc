/*
 * EventRegistry.cpp
 *
 *  Created on: Mar 30, 2014
 *      
 */

#include "EventRegistry.h"
#include <cstddef>
#include <string.h>
#include <algorithm>

using namespace std;

namespace license {
EventRegistry::EventRegistry() {
}

EventRegistry& operator<<(EventRegistry& eventRegistry,
		AuditEvent& securityEvent) {
	eventRegistry.logs.push_back(securityEvent);
	return eventRegistry;
}

EventRegistry& operator<<(EventRegistry& eventRegistry1,
		EventRegistry& otherRegistry) {
	eventRegistry1.append(otherRegistry);
	return eventRegistry1;
}

void EventRegistry::append(const EventRegistry& eventRegistry) {
	logs.insert(logs.end(), eventRegistry.logs.begin(),
			eventRegistry.logs.end());
}

void EventRegistry::turnLastEventIntoError() {
	if (logs.size() > 0) {
		logs.back().severity = SVRT_ERROR;
	}
}

bool EventRegistry::turnEventIntoError(EVENT_TYPE event) {
	bool eventFound = false;
	for (auto it = logs.begin(); it != logs.end(); ++it) {
		if (it->event_type == event) {
			it->severity = SVRT_ERROR;
			eventFound = true;
		}
	}
	return eventFound;
}

AuditEvent const * EventRegistry::getLastFailure() const {
	const AuditEvent* result = NULL;
	if (logs.size() == 0) {
		return result;
	}
	auto it = logs.end();
	do {
		--it;
		if (it->severity == SVRT_ERROR) {
			result = &(*it);
			break;
		}
	} while (it != logs.begin());
	return result;

}

bool EventRegistry::isGood() const {
	bool isGood = true;
	for (auto it = logs.begin(); it != logs.end(); ++it) {
		if (it->severity == SVRT_ERROR) {
			isGood = false;
			break;
		}
	}
	return isGood;
}

void EventRegistry::addError(EVENT_TYPE event) {
	this->addEvent(event, SVRT_ERROR);
}

void EventRegistry::addEvent(EVENT_TYPE event, SEVERITY severity) {
	AuditEvent audit;
	audit.severity = severity;
	audit.event_type = event;
	audit.param1[0] = '\0';
	audit.param2[0] = '\0';
	logs.push_back(audit);
}

void EventRegistry::addEvent(EVENT_TYPE event, SEVERITY severity,
		const string& eventParameter) {
	AuditEvent audit;
	audit.severity = severity;
	audit.event_type = event;
	strncpy(audit.param1, eventParameter.c_str(), 255);
	audit.param2[0] = '\0';
	logs.push_back(audit);
}

bool EventRegistry::turnErrosIntoWarnings() {
	bool eventFound = false;
	for (auto it = logs.begin(); it != logs.end(); ++it) {
		if (it->severity == SVRT_ERROR) {
			it->severity = SVRT_WARN;
			eventFound = true;
		}
	}
	return eventFound;
}

void EventRegistry::exportLastEvents(AuditEvent* auditEvents, int nlogs) {
	int sizeToCopy = min(nlogs, (int) logs.size());
	std::copy(logs.begin(), logs.begin() + sizeToCopy, auditEvents);
}
}

