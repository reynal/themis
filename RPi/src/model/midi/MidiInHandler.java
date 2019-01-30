package model.midi;

import java.io.IOException;

import javax.sound.midi.Transmitter;
import javax.sound.midi.Receiver;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Synthesizer;

/*	If the compilation fails because this class is not available,
	get gnu.getopt from the URL given in the comment below.
*/
//import gnu.getopt.Getopt;



/**	<titleabbrev>MidiInDump</titleabbrev>
	<title>Listens to a MIDI port and dump the received event to the console</title>

	<formalpara><title>Purpose</title>
	<para>Listens to a MIDI port and dump the received event to the console.</para></formalpara>

	<formalpara><title>Usage</title>
	<para>
	<cmdsynopsis>
	<command>java MidiInDump</command>
	<arg choice="plain"><option>-l</option></arg>
	</cmdsynopsis>
	<cmdsynopsis>
	<command>java MidiInDump</command>
	<arg choice="plain"><option>-d <replaceable>devicename</replaceable></option></arg>
	<arg choice="plain"><option>-n <replaceable>device#</replaceable></option></arg>
	</cmdsynopsis>
	</para></formalpara>

	<formalpara><title>Parameters</title>
	<variablelist>
	<varlistentry>
	<term><option>-l</option></term>
	<listitem><para>list the availabe MIDI devices</para></listitem>
	</varlistentry>
	<varlistentry>
	<term><option>-d <replaceable>devicename</replaceable></option></term>
	<listitem><para>reads from named device (see <option>-l</option>)</para></listitem>
	</varlistentry>
	<varlistentry>
	<term><option>-n <replaceable>device#</replaceable></option></term>
	<listitem><para>reads from device with given index (see <option>-l</option>)</para></listitem>
	</varlistentry>
	</variablelist>
	</formalpara>

	<formalpara><title>Bugs, limitations</title>
	<para>
	For the Sun J2SDK 1.3.x or 1.4.0, MIDI IN does not work. See the <olink targetdoc="faq_midi" targetptr="faq_midi">FAQ</olink> for alternatives.
	</para></formalpara>

	<formalpara><title>Source code</title>
	<para>
	<ulink url="MidiInDump.java.html">MidiInDump.java</ulink>,
	<ulink url="DumpReceiver.java.html">DumpReceiver.java</ulink>,
	<ulink url="MidiCommon.java.html">MidiCommon.java</ulink>,
	<ulink url="http://www.urbanophile.com/arenn/hacking/download.html">gnu.getopt.Getopt</ulink>
	</para>
	</formalpara>

*/
public class MidiInHandler
{
	/**	Flag for debugging messages.
	 	If true, some messages are dumped to the console
	 	during operation.
	*/
	private static boolean		DEBUG = true;


	public static void main(String[] args) throws Exception {

		/*
		 *	The device name/index to listen to.
		 */
		String	strDeviceName = null;
		int	nDeviceIndex = 0;
		boolean bUseDefaultSynthesizer = false;
		// TODO: synchronize options with MidiPlayer
		
		MidiCommon.listDevicesAndExit(true, false,true);
		
		MidiDevice.Info	info = MidiCommon.getMidiDeviceInfo(nDeviceIndex);
		MidiDevice	inputDevice = MidiSystem.getMidiDevice(info);
		inputDevice.open();
		Receiver r = new DumpReceiver(System.out);
		Transmitter	t = inputDevice.getTransmitter();
		t.setReceiver(r);
		Thread.sleep(100000);	
		inputDevice.close();
	}



	private static void out(String strMessage)
	{
		System.out.println(strMessage);
	}



	private static void out(Throwable t)
	{
		if (DEBUG) {
			t.printStackTrace();
		} else {
			out(t.toString());
		}
	}
}



/*** MidiInDump.java ***/

