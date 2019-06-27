package u9;

import java.io.*;
import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;
import java.net.*;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

import static java.lang.System.out;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

public class Server {
	private static final int BUFFER_SIZE = 256;

	Vector<String> users = new Vector<String>();
	Vector<HandleChatClient> chatClients = new Vector<HandleChatClient>();
	Vector<Socket> audioClients = new Vector<Socket>();

	String path = "/home/bilbo/example.wav";
	String audioFormat = "wav";

	public void process() throws Exception {
		ServerSocket chatServer = new ServerSocket(9999, 10);
		out.println("Chat Server Started...");
		new HandleAudioClient();
		try {
			while (true) {
				Socket chatClient = chatServer.accept();
				HandleChatClient c = new HandleChatClient(chatClient);
				chatClients.add(c);
				c.sendMessage("Server", " Welcome");
				c.sendMessage("AudioFormat", audioFormat);
				System.out.println("AudioFormat transmitted.\n");
			}
		} catch (InterruptedException e) {
			chatServer.close();
			Thread.currentThread().interrupt();
		}
	}

	public static void main(String... args) throws Exception {
		new Server().process();
	}

	public void broadcast(String user, String message) throws Exception {
		for (HandleChatClient c : chatClients) {
			// send message to all connected users
			if (!c.getUserName().equals(user)) {
				c.sendMessage(user, message);
			}
		}
	}

	class HandleAudioClient extends Thread {

		public HandleAudioClient() {
			start();
		}

		public void run() {
			try {
				ServerSocket audioServer = new ServerSocket(6666, 10);
				out.println("Audio Server Started...");
				new AudioServer(audioServer);
				while (true) {
					Socket audioClient = audioServer.accept();
					audioClients.add(audioClient);
				}
			} catch (Exception e) {
			}
		}

	}

	class AudioServer extends Thread {

		byte[] audiobuffer;
		AudioInputStream ais;
		ServerSocket adserver;

		public AudioServer(ServerSocket server) {
			adserver = server;
		}

		public void run() {
			// prepare Audio
			while (true) {
				if (ais != null) {
					audiobuffer = new byte[BUFFER_SIZE];
					try {
						if (ais.read(audiobuffer) <= 0) {
							ais = AudioSystem.getAudioInputStream(new URL("file:" + path));
							ais.read(audiobuffer);
						}
					} catch (IOException e) {
						System.err.println("IO-ERROR while reading file");
						// adserver.close();
					} catch (UnsupportedAudioFileException e) {
						System.err.println("Unsupported Audiofile type.");
						// adserver.close();
					}
				} else {
					audiobuffer = null;
				}

				for (Socket c : audioClients) {
					try {
						OutputStream os = c.getOutputStream();
						AudioSystem.write(ais, AudioFileFormat.Type.WAVE, os);
					} catch (IOException e) {
						System.err.println("IO-ERROR while streaming audio bytes");
					}
				}
			}
		}
	}

	class HandleChatClient extends Thread {
		String name = "";
		BufferedReader input;
		PrintWriter output;

		public HandleChatClient(Socket client) throws Exception {
			// get input and output streams
			input = new BufferedReader(new InputStreamReader(client.getInputStream()));
			output = new PrintWriter(client.getOutputStream(), true);
			// read name
			name = input.readLine();
			users.add(name);
			start();
		}

		public void sendMessage(String uname, String msg) {
			output.println(uname + ":" + msg);
		}

		public String getUserName() {
			return name;
		}

		public void run() {
			String line;
			try {
				while (true) {
					line = input.readLine();
					if (line.equals("end")) {
						chatClients.remove(this);
						users.remove(name);
						break;
					}
					broadcast(name, line); // method of outer class - send messages to all
				}
			} catch (Exception e) {
				System.out.println(e.getMessage());
			}
		}
	}
}