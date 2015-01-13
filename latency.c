
/*
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "ladspa.h"

typedef enum {
    INPUT,
    OUTPUT,
    LATENCY,
    NUM_PORTS
} PortIndex;

typedef struct {
    float* buffer;
    unsigned long latency;
    
    unsigned long frame;
    unsigned long size;
    
    struct {
        const float* input;
        float* output;
        const float* latency;
    } ports;
} Latency;

static LADSPA_Handle instantiate (const LADSPA_Descriptor* descriptor, unsigned long rate) {
    Latency* self = calloc (1, sizeof (Latency));
    
    self->size = rate * 2;
    self->buffer = calloc (self->size, sizeof (float));
    self->latency = 0;
    
    return (LADSPA_Handle) self;
}

static void connect_port (LADSPA_Handle instance, unsigned long port, LADSPA_Data* data) {
    Latency* self = instance;
    
    switch ((PortIndex) port) {
    case INPUT:
        self->ports.input = data;
        break;
    case OUTPUT:
        self->ports.output = data;
        break;
    case LATENCY:
        self->ports.latency = data;
        break;
    default:
        break;
    }
}

static void activate (LADSPA_Handle instance) {
}

static void run (LADSPA_Handle instance, unsigned long nframes) {
    Latency* self = instance;
    unsigned long i;
    
    self->latency = (unsigned long) *self->ports.latency;
    
    for (i = 0; i < nframes; ++i) {
        self->buffer[self->frame % self->size] = self->ports.input[i];
        self->ports.output[i] = self->buffer[(self->frame - self->latency) % self->size];
        ++self->frame;
    }
}

static void deactivate (LADSPA_Handle instance) {
}

static void cleanup (LADSPA_Handle instance) {
    Latency* self = instance;
    free (self->buffer);
    free (instance);
}

static const LADSPA_PortDescriptor port_descriptors[NUM_PORTS] = {
    LADSPA_PORT_AUDIO | LADSPA_PORT_INPUT,
    LADSPA_PORT_AUDIO | LADSPA_PORT_OUTPUT,
    LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT
};

static const char* const port_names[NUM_PORTS] = {
    "Input",
    "Output",
    "Latency"
};

static const LADSPA_PortRangeHint port_range_hints[NUM_PORTS] = {
    { 0, 0, 1 },
    { 0, 0, 1 },
    { LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_DEFAULT_0 | LADSPA_HINT_SAMPLE_RATE, 0.0, 1.0 },
};

static const LADSPA_Descriptor descriptor = {
    266,
    "latency",
    0,
    "Latency",
    "Greippi",
    "WTFPL",
    NUM_PORTS,
    port_descriptors,
    port_names,
    port_range_hints,
    NULL,
    instantiate,
    connect_port,
    activate,
    run,
    NULL,
    NULL,
    deactivate,
    cleanup
};

#ifdef _WIN32
__declspec(dllexport)
#endif
const LADSPA_Descriptor* ladspa_descriptor (unsigned long index) {
    switch (index) {
    case 0:
        return &descriptor;
    default:
        return NULL;
    }
}
