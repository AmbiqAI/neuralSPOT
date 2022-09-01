# WIP Notes (Pay no attention to anything below this line)
## Adding malloc support
Malloc support is non-deterministic, and should only be used for debug environments such as erpc

It has 3 components
1. ns-malloc
2. example/basic_tf_stub modified to include it
3. ns-harness maps standard malloc/etc to ns-malloc equivalents

### Status
Added malloc and usb to ambiqsuite.a
Created ns-usb with init, send, and rx routines
Next
1. create ns-util malloc stuff, get it compiling (should be easy wrapper around freertos malloc) (DONE)
2. Create new nest in erpc, taking care to not obliterate work in current nest (DONE)
3. Modify erpc transport code to use all of the above
4. Test in main.cc

### New Next
Did 1, 2, 3, and a bit of 4 (don't really have a usb test). Next:
1. Now that ERPC client is compiling, get the python server instantiated
2. Add a simple TempAlarm invocations from evb to python.
3. Profit! Or, more realistically, implement a bulk dump of an audiobuffer.
4. Can we get rid of all the #ifdef DEBUG-style stuff and make it more inline? e.g. use a macro to only dump if a DEFINE is enabled?

### Doozy of a Bug
OK, so the 'don't really have a usb test' above turned out to be a problem. 
USB wasn't working at all, for the following reasons:
1. I wasn't calling the usb_service task (needs to be called in app loop)
2. Missing src files popped up when I added that.
3. TinyUSB needed a missing define, added that to makefile
4. Malloc and TinyUSB weren't getting along. If I did a malloc before initializing USB, nothing happened
	1. This was a doozy of a bug. It turns out FreeRTOS malloc assumes the task manager has been initialized, which sets, among other things, a variable for tracking nested entry/exit of critical regions, during which interrupts are disabled. If the variable wasn't initialized, it disabled interrupts without complaint, but it didn't reenable them.
	2. My solution was to create pvTasklessPort Malloc and vTasklessFree, and modify the original functions to only suspend/resume tasks if a new parameter was set.

TLDR: remember that heap_4.c has been modified, and portable.h! Also, when we add task support to NS, we need to add a config variable to ns-malloc that calls the right malloc/free.

Before I was derailed by Mr. Doozy, I made some progress on ERPC, researching the python endpoint. I switched to the matrix_multiple example which is simpler than tempalarm and closer to what we need anyway.

Now that I have this kind of working, go back to New Next step 2, above.

Regarding nests: we really need an upgrade mechanism (record date of nest creation, look for files in the nest that are newer to that so might get overridden, warn if I find any (maybe back them up automatically))

### New new next
Quick status: kind of have ERPC server/client talking. Server sees RPC after a couple of tries from client, and sees the right data. Eventually it starts getting CRC errors. The RPC response is all zeros on client side.
1. Debug why it takes a few requests.
	1. Clue: the read after the request right doesn't seem to get any data. It could be that the server isn't seeing the first few.
2. Add prints everywhere. Send/receive on client and server sides.
4. Fix send/receive status to return error if data doesn't match requested size
5. Figure out why crc errors start happening, probably some corruption going on.

almost there!

### Works!
I was missing a flush in the usb send. Here is a list of cleanup items:
1. Add flush to ns-usb.h (DONE)
2. Get rid of all the prints I sprinkled everywhere
3. Put NS-specific erpc code in it's own directory for documentation purposes
4. Implement audio interface (instead of matrix multiply) (STARTED)
5. Move code into ns (see below)

### ERPC Flow
1. NS/extern/erpc contains base code needed for ERPC clients, including erpc python
2. NS/ns-audio-dump - add ns-audio-dump.h and all erpc audio-specific code (including erpc for documentation)
3. NS/tools - add audio_erpc_example.py

### Status
1. Cleaned up erpc, created an audio.erpc and a python app that is listening

Next:
1. create new nest, massage code needed to only copy new stuff over
2. Modify main.cc to call audio dump in real world inference loop
3. Copy erpc code over to ns as described above
4. Clean up main.cc by breaking out into multiple files

### Latest Status
RPC for audio works and has been checked in and merged. Now working on i2c drivers.

Next:
1. Get i2c mpu driver running DONE
2. Get i2c dumping via RPC running
3. Write decent documentation on all of that
4. Start clearing out my small item backlog


## NS Upgrades and Versions
NS is changing often, including API-impacting changes, and won't stabilize for a while. Even after we're 'stable', we need a way to track major and minor changes, and an upgrade path.

### Versioning
Do we do an overall NS version, or do we version the individual libraries, or what?
Having only the overall version means that your code may be impacted by NS changes that you don't care about.
Having per-component versions adds complexity (both to user code and to our maintenance - what versions work with what?)
How do we handle extern versions? Do we allow mix-and-match? That would mean that for new versions of TF, for example, I would have to add support for that to older versions of NS.

#### Proposal
- For now, only latest version of NS is supported (I don't want to start adding #ifdefs everywhere)
- New extern versions force new version of NS
*GAH*, this is a pain. I'm going to punt to team for input/discussion.

### Upgrades
This is primarily a nest thing. When you want to pull the latest NS code into an existing nest, how do you do it?
Almost no src (other than headers) is copied over - basically just the stuff in example/src.