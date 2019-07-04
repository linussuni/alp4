package alpv.mwp;

import java.rmi.RemoteException;
import java.rmi.NotBoundException;

public class Main {

    static String defaultHost = "127.0.0.1";
    static int defaultPort = 6666;
    static int defaultWorkers = 4;

    public static void main(String[] args) {
        try {

            if (args.length == 0) {
                System.err.println("no.");
            } else if (args[0].equals("server")) {
                MasterServer master = new MasterServer(defaultPort);

            } else if (args[0].equals("client")) {
                ClientMath client = new ClientMath(defaultHost, defaultPort);
                client.execute();
            } else if (args[0].equals("worker")) {
                WorkerMain worker = new WorkerMain(defaultHost, defaultPort);
                Thread workerThread = new Thread(worker);
                workerThread.start();
            }
        } catch (RemoteException e) {
            System.err.println(e);
        } catch (NotBoundException e) {
            System.err.println(e);
        }
        
    }

}