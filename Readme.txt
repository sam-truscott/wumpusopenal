Wumpus Out OpenAL is copyright (c) 2018 by Sam Truscott.
=======================================================

Wumpus Out OpenAL v1.1.0
===========================

	Install
	=======
	Place the out_openal.dll file in Winamp\Plugins directory
	and select it as the Output plug-in (Plug-ins\Output).

	Features
	========
	* Mono, Stereo, Multi-channel (4, 5.1, 7.1) audio at 8bit/16bit.
	* Hardware acceleration support
	* Low CPU utilisation even with software rendering 
	* Expand Mono and Stereo to 4.0 (small performance hit)
	* 3D, move your speakers around
	* Reverb effects
	* XRAM support

	Known Issues
	============
	* No 24bit audio support, OpenAL doesn't support it
	* With 3D mode enabled, the streams go out of time
	
Development
===========
out_wumpus.sln		The Project to load
OPENAL_SDK_DIR		Environment variable pointing to the OpenAL SDK installation
						directory. On my system it's:
						C:\Development\OpenAL 1.1 SDK
WA_SDK_DIR			The 'Winamp' folder of the Winamp SDK. This contains some of the stuff
						we need to talk to Winamp. On my system it's:
						C:\Program Files\Winamp SDK\Winamp

Change Log
==========

	v1.0.1
	======
	* Rebuilt with Vs2012
	* Minor tweaks

	v1.0.0
	======
	* Updated - Minor clean-ups

	v0.9.11.2
	=========
	* Minor fix to try and sync multiple speakers to stop the out-of-phase or
		delay between the left & right channel issue

	v0.9.11.1
	=========
	* Added - Synchronised the start of audio (affects multiple renderers)
	* Added - x/y/z for listener position and direction
	* Added - Restore default settings for speaker matrix
	* Updated - Removed algorithm for finding next free buffer.
		This should reduce CPU usage quite a bit

	v0.9.10
	=======
	* [Attempt] Fixed - 3D issue where it went out of phase.

	v0.9.9.1
	========
	* Fixed - Issue with synchronisation and the output time for track position

	v0.9.9
	======
	* Fixed - Memory leak when closing regarding Effects
	* Fixed - Skipping past the end of the file
	* Fixed - Skipping and sync issues, skipping over-run
	
	v0.9.8
	======
	* Fixed - Doesn't re-play the end of the track when skipped
	* Fixed - Sync issues with FLAC files
	* Fixed - Mono source won't be affected by Matrix values (left/right etc)

	v0.9.7.1
	======
	* Added - mono & stereo expansion back in
	* Changed - maximum buffer size to 6 seconds (6000ms)
		to better support XRAM
	
	v0.9.6
	======
	* Fixed - XRAM detection/utilisation
	* Fixed - Audio cut off at the end

	v0.9.5
	======
	* Added - Some proper XRAM notices
	* Fixed - 3D seperate renderers such as sync
	* Fixed - Panning

	v0.9.4
	======
	* Added - Effects (reverb)

	v0.9.3
	======
	* Added - seperate renderer support
	* Splitting of data to renderers

	v0.9.2
	======
	* Added - initial Effects support

	v0.9.1
	======
	* Updated - minor optimisations to Mono/Stereo expansion so 
		it copies the data using single instructions rather than
		memcpy System Calls which should reduce CPU usage.

	v0.9.0
	======
	* Fixed - Corrupt audio on VBR (Varible Bit Rate) Audio
	* Fixed - Memory leak with Mono/Stereo Expansion enabled
	* Fixed - Loading of the buffer size on the Configuration Window
	* Fixed - Loading of the volume
	* Fixed - Lockups when opening/skipping tracks
	* Fixed - XRAM cannot be enabled if it's absent
	* Added - XRAM utilisation
	* Updated - no internal buffering, passed straight to OpenAL to reduce memory usage
		and CPU utilisation. Even software rendering should have virtually no CPU usage.

	v0.8.2
	======
	* Fixed - Pausing issue

	v0.8.1
	======
	* Fixed - If the file is skipped when pause playback will resume (Thanks Goujon)

	v0.8.0
	======
	* Added - Stereo expansion
	* Added - Mono expansion
	* Added - GUI bits to control Mono/Stereo expansion
	* Fixed - Expansion Fixed (no static)
	* Fixed - GUI changes for expansion actioned instantly, not after Winamp restart
	* Fixed - Skipping of FLAC files no longer causes halts/crashes
	* Added [disabled] - 3D Positioning, doesn't work (yet, 0.9 hopefully)
	* Updated - Will buffer 3 writes rather than 1 to stop buffer under-runs on startup
	* Fixed - Out of range volumes ignored, should solve the jump-loud problem
	* Fixed - Initial volume loaded properly rather than using maximum which may be wrong

	v0.7.4
	======
	* Fixed - use correct Winamp.ini file
	* Fixed - validates buffer length is within bounds

	v0.7.3
	======
	* Fixed bug if Winamp.ini had invalid values or didnt exist

	v0.7.2
	======
	* Minor fix and tidy up

	v0.7.1
	======
	* Updated: Use more but much smaller buffers (8k).
		This seems to work better than fewer larger ones although
		the affect on CPU performance is unknown. Still to test with
		H/W support
	* Fixed: Can change the buffer size during playback (may cause 
		a pause while it clears all the bufers out)

	v0.7.0
	======
	* Updated: Use fewer much larger buffers to test
		if this was more stable/optimal

	v0.6.2
	======
	* Optimisations

	v0.6.1
	* Fixed: Very small files (<2sec) now work

	v0.6.0
	======
	* Fixed: Memory over-run issue (causes blips, silence & crashses)
	* Updated: Have done some work on stereo expansion and EFX (currently disabled)

	v0.5.3.1
	========
	* Fixed: Thread/sync dead lock

	v0.5.3
	======
	* Fixed: Changing devices and options keeps playing tracks now
	* Fixed: Reduced maximum Monitor Interval to 30ms
	* Fixed: Low quality/short files now work
	* Fixed: Better stability when switching tracks or changing options

	v0.5.2
	======
	* Fixed: Changing options doesn't crash Winamp anymore

	v0.5.1
	======
	* Added: Configure Length, Buffer Sizes (Total Buffers = length/size) & Monitor Time
		The lower the monitor time the more processing is done
		40ms is fine as this is shorter than a single buffer time
	* Fixed: Loading/Saving of some values
	* Fixed: Changing Audio Device fixes
	* Fixed: Closing Configuration Windows and re-opening doesn't crash anymore
	* Fixed: Skipping correctly continues visualisation

	v0.5.0
	======
	* Added: Initial Effects support (disabled for now)
	* Added: Error messages
	* Fixed: Generic Software Support

	v0.4.0
	======
	* Added: OpenAL Extensions are now shown
	* Added: Notice when 24bit playback is attempted
	* Updated: OpenAL SDK version being used updated
		-> This means Vista's users will see all EndPoints now
		-> Advise you stick to hardware though (this is usually shown as "default")
	* Updated: Changing a Device is applied instantly (will stop playback)
	* Fixed: Only one instance of the Configuration Window can be loaded
	* Fixed: Few bugs which cause Winamp to crash

	v0.3.2
	======
	* Updated so prebuffering will kick off after 200ms rather than 2000ms
	* Updated internal monitor thread to clock at 50ms not 10ms

	v0.3.1
	======
	* Added configuration/status screen

	v0.3.0
	======
	* Fixed buffering issues for better throughput
	* Fixed sync & timing issues
	* Fixed skipping (fw/rw)
	* Optimisations

	v0.2.1
	======
	* Fixed bug where volume not restored for next track

	v0.2.0
	======
	* Multi-channel support
	* Volume change support

	v0.1.0
	======
	* Initial build
	* Mono/Stereo Playback support