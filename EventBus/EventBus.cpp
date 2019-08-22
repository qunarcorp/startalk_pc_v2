/*
 * Copyright (c) 2014, Dan Quist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "EventBus.h"
#include "../include/ThreadPool.h"
#include <memory>

// Declare the static instance since this can't be done in the header file
EventBus* EventBus::instance = nullptr;


EventBus * const EventBus::GetInstance()
{
    if (instance == nullptr) {
        instance = new EventBus();
    }
    return instance;
}

// asynchronous 异步请求
void EventBus::FireEvent(Event & e, bool asynchronous)
{
    EventBus *instance = GetInstance();

    Registrations *registrations = instance->handlers[typeid(e)];

    // If the registrations list is null, then no handlers have been registered for this event
    if (registrations == nullptr) {
        return;
    }

    // Iterate through all the registered handlers and dispatch to each one if the sender
    // matches the source or if the sender is not specified
    for (auto &reg : *registrations) {
        //if ((reg->getSender() == nullptr) || (reg->getSender() == &e.getSender())) {

            // This is where some magic happens. The void * handler is statically cast to an event handler
            // of generic type Event and dispatched. The dispatch function will then do a dynamic
            // cast to the correct event type so the matching onEvent method can be called
            static_cast<EventHandler<Event> *>(reg->getHandler())->dispatch(e);
        //}
    }
}

EventBus::EventBus()
{

}
