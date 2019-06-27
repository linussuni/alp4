package u9;

import java.io.*;
import java.util.*;
import java.net.*;
import javax.swing.*;
import javax.xml.transform.Source;

import java.awt.*;
import java.awt.event.*;
import static java.lang.System.out;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.DataLine;

public class Client extends JFrame implements ActionListener {

    private static final int BUFFER_SIZE = 256;

    String uname;
    PrintWriter pw;
    BufferedReader br;
    JTextArea taMessages;
    JTextField tfInput;
    JButton btnSend, btnExit;
    Socket chatClient;
    Socket audioClient;
    String audioFormat;
    AudioFormat specificAudioFormat;
    byte[] audioBuffer; 

    public Client(String uname, String servername)  throws Exception {
        super(uname); // set title for frame
        this.uname = uname;
        File file = new File("/home/bilbo/example.wav");
        specificAudioFormat = AudioSystem.getAudioFileFormat(file).getFormat();
        
        // Chat related
        chatClient = new Socket(servername, 9999);
        br = new BufferedReader(new InputStreamReader(chatClient.getInputStream()));
        pw = new PrintWriter(chatClient.getOutputStream(), true);
        pw.println(uname); // send name to server
        buildInterface();
        new MessagesThread().start(); // create thread to listen for messages
        
        // Audio related
        new AudioStreamer().start();
        audioClient = new Socket(servername, 6666);
        System.out.println("Started up all connections\n");
    }

    // inner class for Audio Thread
    class AudioStreamer extends Thread {
        public void run() {
            try {
                while (true) {
                    playback();
                }
            } catch (Exception ex) {
            }
        }
    }

    public void playback() throws Exception{
        try {
        InputStream audio = audioClient.getInputStream();
        InputStream bufferIn = new BufferedInputStream(audio);

        AudioInputStream ais = AudioSystem.getAudioInputStream(bufferIn);
        DataLine.Info info = new DataLine.Info(SourceDataLine.class, specificAudioFormat);
        SourceDataLine audioLine = (SourceDataLine) AudioSystem.getLine(info);

        audioLine.open(specificAudioFormat);
        audioLine.start();

        audioBuffer = new byte[BUFFER_SIZE];
        int counter = -1;

        while ((counter = ais.read(audioBuffer)) != -1){
            audioLine.write(audioBuffer, 0, counter);
        }

        audioLine.drain();
        audioLine.close();
        ais.close();
        } catch (Exception e){}

    }

    public void buildInterface() throws Exception {
        btnSend = new JButton("Send");
        btnExit = new JButton("Exit");
        taMessages = new JTextArea();
        taMessages.setRows(10);
        taMessages.setColumns(50);
        taMessages.setEditable(false);
        tfInput = new JTextField(50);
        JScrollPane sp = new JScrollPane(taMessages, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        add(sp, "Center");
        JPanel bp = new JPanel(new FlowLayout());
        bp.add(tfInput);
        bp.add(btnSend);
        bp.add(btnExit);
        add(bp, "South");
        btnSend.addActionListener(this);
        btnExit.addActionListener(this);
        setSize(500, 300);
        setVisible(true);
        pack();
    }

    public void actionPerformed(ActionEvent evt) {
        if (evt.getSource() == btnExit) {
            pw.println("end"); // send end to server so that server knows about the termination
            System.exit(0);
        } else {
            // send message to server
            pw.println(tfInput.getText());
        }
    }

    public static void main(String... args) throws Exception {

        // take username from user
        String name = JOptionPane.showInputDialog(null, "Enter your name :", "Username", JOptionPane.PLAIN_MESSAGE);
        String servername = "localhost";
        try {
            new Client(name, servername);
        } catch (Exception ex) {
            out.println("Error --> " + ex.getMessage());
        }

    }

    // inner class for Messages Thread
    class MessagesThread extends Thread {
        public void run() {
            String line;
            try {
                while (true) {
                    line = br.readLine();
                    if(line.length() > 13 && line.substring(0,12).equals("AudioFormat:")){
                        audioFormat = line.substring(12);
                        if (audioFormat.equals("wav")){
                            taMessages.append("Succesfully set AudioFormat to >" + specificAudioFormat + "<\n");
                        }else {
                            taMessages.append("Unknown Audio Format\n");
                        }
                        continue;
                    }
                    taMessages.append(line + "\n");
                }
            } catch (Exception ex) {
            }
        }
    }
}